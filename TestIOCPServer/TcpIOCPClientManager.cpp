#include "stdafx.h"
#include "TcpIOCPClientManager.h"


CTcpIOCPClientManager::CTcpIOCPClientManager()
{
	InitializeCriticalSection(&m_FreeListSection);
	InitializeCriticalSection(&m_RealListSection);
}

CTcpIOCPClientManager::~CTcpIOCPClientManager()
{
	DeleteCriticalSection(&m_FreeListSection);
	DeleteCriticalSection(&m_RealListSection);

	for (auto iter = m_BigClientList.begin(); iter != m_BigClientList.end(); iter++)
		delete[] * iter;
	m_BigClientList.clear();
}

int CTcpIOCPClientManager::InitManager(int maxconnectnums)
{
	CTcpIOCPClientStub* pbigclient = new CTcpIOCPClientStub[maxconnectnums];
	if (pbigclient != NULL)
	{
		m_BigClientList.push_back(pbigclient);
		for (int i = 0; i < maxconnectnums;i++)
			m_FreeClientList.push_back(pbigclient+i);
	}

	return 0;
}

CTcpIOCPClientStub* CTcpIOCPClientManager::GetFreeClient()
{
	CTcpIOCPClientStub *pclient = NULL;
	EnterCriticalSection(&m_FreeListSection);
	if (m_FreeClientList.size() > 0)
	{
		pclient = *(m_FreeClientList.begin());
		m_FreeClientList.pop_front();
	}
	else
	{
		pclient = new CTcpIOCPClientStub[1000];
		if (pclient != NULL)
		{
			OutputDebugString(_T("alloc 1000 client in manager"));
			m_BigClientList.push_back(pclient);
			for (int i = 1; i < 1000; i++)
				m_FreeClientList.push_back(pclient + i);
		}
	}
	LeaveCriticalSection(&m_FreeListSection);
	return pclient;
}

int CTcpIOCPClientManager::AddRealClient(CTcpIOCPClientStub* pclient)
{
	EnterCriticalSection(&m_RealListSection);
	m_RealClientList.insert(pair<CTcpIOCPClientStub*, int>(pclient,0));
	LeaveCriticalSection(&m_RealListSection);
	return 0;
}

int CTcpIOCPClientManager::RemoveRealClient(CTcpIOCPClientStub* pclient)
{
	EnterCriticalSection(&m_RealListSection);
	int num = m_RealClientList.erase(pclient);
	LeaveCriticalSection(&m_RealListSection);
	if (num < 1)
	{
//		OutputDebugString(_T("can't find client in manager"));
		return 0;
	}

	pclient->CloseClient();

	EnterCriticalSection(&m_FreeListSection);
	m_FreeClientList.push_back(pclient);
	LeaveCriticalSection(&m_FreeListSection);

	return 0;
}

int CTcpIOCPClientManager::GetClientNum()
{
	return m_RealClientList.size();
}

int CTcpIOCPClientManager::CloseAllClient() //关掉socket后退回free队列
{
	EnterCriticalSection(&m_RealListSection);
	for (auto iter = m_RealClientList.begin(); iter != m_RealClientList.end(); iter++)
	{
		(*iter).first->CloseClient();
		m_FreeClientList.push_back((*iter).first);
	}
	LeaveCriticalSection(&m_RealListSection);
	m_RealClientList.clear();

	return 0;
}
