#include "stdafx.h"
#include "CalcCenter.h"
#include <iostream>
#include "process.h"
using namespace std;
#include "IOReq.h"

CCalcCenter::CCalcCenter()
{
	m_hCompletionPort = INVALID_HANDLE_VALUE;
	m_nThreadNum = 0;
	m_bProcessing = false;
	m_pThreadHandles = NULL;
	m_pThreadData = NULL;
}


CCalcCenter::~CCalcCenter()
{
}

int CCalcCenter::InitObj(int nthreadnumber)
{
	m_nThreadNum = nthreadnumber;
	if (nthreadnumber <= 0)
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		m_nThreadNum = sysinfo.dwNumberOfProcessors;
	}

	//create completion port hangle
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_nThreadNum);
	if (m_hCompletionPort == INVALID_HANDLE_VALUE)
		return -1;

	m_pThreadHandles = new HANDLE[m_nThreadNum * 2];
	memset(m_pThreadHandles, 0, m_nThreadNum * 2*sizeof(HANDLE));
	m_pThreadData = new unsigned int[m_nThreadNum * 2];

	//create nthreadnumber threads
	m_bProcessing = true;
	for (int i = 0; i < m_nThreadNum * 2; i++)
	{
		m_pThreadHandles[i] = (HANDLE)_beginthreadex(NULL, 0, InitProcessThread, this, 0, &m_pThreadData[i]);
		if (m_pThreadHandles[i] == 0)
			return -2;
	}

	return 0;
}

int CCalcCenter::UninitObject()
{
	m_bProcessing = false;
	if (m_hCompletionPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = INVALID_HANDLE_VALUE;
	}
	
	for (int i = 0; i < m_nThreadNum * 2; i++)
	{
		if (m_pThreadHandles[i] != 0)
		{
			CloseHandle(m_pThreadHandles[i]);
			m_pThreadHandles[i] = 0;
		}
	}
	delete m_pThreadHandles;
	delete m_pThreadData;

	return 0;
}

unsigned WINAPI CCalcCenter::InitProcessThread(LPVOID pv)
{
	CCalcCenter* lpInst = (CCalcCenter*)pv;
	return lpInst->ProcessThread();
}

unsigned CCalcCenter::ProcessThread()
{
	int ret;
	BOOL bsuc;
	DWORD dwnumbytes;
	OVERLAPPED *poverlap;
	ULONG_PTR completionkey;

	DWORD threadid = GetCurrentThreadId();
	int curthreadindex = 0;
	for (int i = 0; i < m_nThreadNum * 2; i++)
		if (m_pThreadData[i] == threadid)
		{
			curthreadindex = i;
			break;
		}

	DEBUGOUTPUT("thread %d start \n", curthreadindex);
	DWORD dwerror;
	while (m_bProcessing)
	{
		bsuc = GetQueuedCompletionStatus(m_hCompletionPort, &dwnumbytes, &completionkey, &poverlap, INFINITE);
		if (bsuc)
		{
			((CIOReq*)poverlap)->ReportResult(dwnumbytes, completionkey);
		}
		else
		{
			dwerror = GetLastError();
			if (poverlap != NULL)
			{ 
				((CIOReq*)poverlap)->ReportResult(dwnumbytes, completionkey, dwerror);
			}
			else
			{
				CString tempstr;
				tempstr.Format(_T("CCalcCenter::ProcessThread unknown error errorcode=%u\n"), dwerror);
				OutputDebugString(tempstr);
				if (6 == dwerror || 735 == dwerror) //invalid handle
					break;
			}
		}
	}

	DEBUGOUTPUT("thread %d end \n", curthreadindex);

	return 0;
}

BOOL CCalcCenter::AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) 
{
	return (CreateIoCompletionPort(hDevice, m_hCompletionPort, CompKey, 0) != NULL);
}
