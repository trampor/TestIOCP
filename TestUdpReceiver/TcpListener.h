#pragma once

#include "TcpListenerBase.h"

template <class WorkerType>
class CTcpListener :  public CTcpListenerBase
{
public:
	CTcpListener();
	~CTcpListener();

	virtual int InitObject(LPCTSTR plocalip, int port);
	virtual int UninitObject();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	static int ReleaseAllList();

	int ReleaseWorker(WorkerType* pworker);

private:
	static CNoLockBiList<WorkerType*> m_WorkerList;
};

template <class WorkerType>
CNoLockBiList<WorkerType*> CTcpListener<WorkerType>::m_WorkerList;

template <class WorkerType>
CTcpListener<WorkerType>::CTcpListener()
{
}

template <class WorkerType>
CTcpListener<WorkerType>::~CTcpListener()
{
}

template <class WorkerType>
int CTcpListener<WorkerType>::ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp)
{
	if (ck == CK_SOCKET_LISTEN)
	{
		if (error == 0)
		{
			//setsockopt(((CTcpListenerIOReq*)povlp)->m_Socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_TcpSocket, sizeof(m_TcpSocket));
			SOCKADDR_IN *addrClient = NULL, *addrLocal = NULL;
			int nClientLen = sizeof(SOCKADDR_IN), nLocalLen = sizeof(SOCKADDR_IN);
			lpfnGetAcceptExSockaddrs(((CTcpListenerIOReq*)povlp)->m_pDataBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&addrLocal, &nLocalLen, (LPSOCKADDR*)&addrClient, &nClientLen);

			WorkerType* pworker = (WorkerType*)m_WorkerList.PopHead();
			if (pworker == NULL)
			{
				pworker = new WorkerType;
				if (pworker != NULL)
					InterlockedExchangeAdd(&m_WorkerList.m_nAllocatedEleNum, 1);
			}
			if (pworker == NULL)
			{
				closesocket(((CTcpListenerIOReq*)povlp)->m_Socket);
				m_BufList.PushTail(((CTcpListenerIOReq*)povlp)->m_pDataBuf);
				m_IOReqList.PushTail((CTcpListenerIOReq*)povlp);

				OutputDebugString(_T("CTcpListener<WorkerType>::ConsumeReq alloc new worker fail"));
				return -1;
			}
			else
			{
				pworker->InitObject(addrLocal->sin_addr.S_un.S_addr, addrLocal->sin_port, addrClient->sin_addr.S_un.S_addr, addrClient->sin_port, ((CTcpListenerIOReq*)povlp)->m_Socket,this);
				m_BufList.PushTail(((CTcpListenerIOReq*)povlp)->m_pDataBuf);
				m_IOReqList.PushTail((CTcpListenerIOReq*)povlp);
				int res = pworker->Start();
				if (res == 0)
					InterlockedExchangeAdd(&m_nAcceptNum, 1);
				else
					m_WorkerList.PushTail(pworker);
			}
		}
		else
		{
			if (povlp != NULL)
			{
				if (((CTcpListenerIOReq*)povlp)->m_Socket != INVALID_SOCKET)
				{
					closesocket(((CTcpListenerIOReq*)povlp)->m_Socket);
					((CTcpListenerIOReq*)povlp)->m_Socket = INVALID_SOCKET;
				}
				if (((CTcpListenerIOReq*)povlp)->m_pDataBuf != NULL)
					m_BufList.PushTail(((CTcpListenerIOReq*)povlp)->m_pDataBuf);
				m_IOReqList.PushTail((CTcpListenerIOReq*)povlp);
			}
			OutputDebugString(_T("CTcpListener::ConsumeReq return error"));
			return -1;
		}

		Accept();
	}

	return 0;
}

template <class WorkerType>
int CTcpListener<WorkerType>::InitObject(LPCTSTR plocalip, int port)
{
	CTcpListenerBase::InitObject( plocalip,  port);

	WorkerType* pworker;
	for (int i = 0; i <1000; i++)
	{
		pworker = new WorkerType;
		m_WorkerList.PushTail(pworker);
		InterlockedExchangeAdd(&m_WorkerList.m_nAllocatedEleNum, 1);
	}
	return 0;
}

template <class WorkerType>
int CTcpListener<WorkerType>::UninitObject()
{
	CTcpListenerBase::UninitObject();
	return 0;
}

template <class WorkerType>
int CTcpListener<WorkerType>::ReleaseAllList()
{
	int i = 0;
	WorkerType* pworker = (WorkerType*)m_WorkerList.PopHead();
	while (pworker != NULL)
	{
		i++;
		pworker->UninitObject();
		delete pworker;
		pworker = (WorkerType*)m_WorkerList.PopHead();
	}
	return 0;
}

template <class WorkerType>
int  CTcpListener<WorkerType>::ReleaseWorker(WorkerType* pworker)
{
	m_WorkerList.PushTail(pworker);
	InterlockedExchangeAdd(&m_nAcceptNum, -1);
	return 0;
}
