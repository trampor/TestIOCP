#include "stdafx.h"
#include "commparam.h"

bool CTcpIOCPTool::m_bInited = false;
list<IO_OPER_DATA*> CTcpIOCPTool::m_BigList;
list<IO_OPER_DATA*> CTcpIOCPTool::m_FreeList;
CRITICAL_SECTION CTcpIOCPTool::m_FreeSection;

int CTcpIOCPTool::InitObj(int maxconnectnums)
{
	if (m_bInited)
		return 0;
	m_bInited = true;
	InitializeCriticalSection(&m_FreeSection);

	IO_OPER_DATA* pdata = new IO_OPER_DATA[maxconnectnums];
	if (pdata != NULL)
	{
		m_BigList.push_back(pdata);
		for (int i = 0; i < maxconnectnums; i++)
			m_FreeList.push_back(pdata + i);
	}

	return 0;
}

int CTcpIOCPTool::UninitObj()
{
	for (auto iter = m_BigList.begin(); iter != m_BigList.end(); iter++)
		delete[] * iter;
	m_BigList.clear();
	DeleteCriticalSection(&m_FreeSection);
	return 0;
}

IO_OPER_DATA* CTcpIOCPTool::GetIOOperData()
{
	IO_OPER_DATA *pdata = NULL;
	EnterCriticalSection(&m_FreeSection);
	if (m_FreeList.size() > 0)
	{
		pdata = *(m_FreeList.begin());
		m_FreeList.pop_front();
	}
	else
	{
		pdata = new IO_OPER_DATA[1000];
		if (pdata != NULL)
		{
			OutputDebugString(_T("alloc 1000 client in manager"));
			m_BigList.push_back(pdata);
			for (int i = 1; i < 1000; i++)
				m_FreeList.push_back(pdata + i);
		}
	}
	LeaveCriticalSection(&m_FreeSection);
	return pdata;
}

int CTcpIOCPTool::ReleaseIOOperData(IO_OPER_DATA* pdata)
{
	EnterCriticalSection(&m_FreeSection);
	m_FreeList.push_back(pdata);
	LeaveCriticalSection(&m_FreeSection);
	return 0;
}