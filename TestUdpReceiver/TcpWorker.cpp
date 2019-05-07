#include "stdafx.h"
#include "Tcpworker.h"
#include "TcpListener.h"

CNoLockBiList<unsigned char*> CTcpWorker::m_BufList;
CNoLockBiList<CTcpWorkerIOReq*> CTcpWorker::m_IOReqList;

CTcpWorker::CTcpWorker()
{
	m_TcpSocket = INVALID_SOCKET;

	m_nRecvNum = 0;
	m_nRecvedNum = 0;

	m_nWriteNum = 0;
	m_nWritedNum = 0;
}


CTcpWorker::~CTcpWorker()
{
}

int CTcpWorker::ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp)
{
	if (error == 0)
	{
		if (((CTcpWorkerIOReq*)povlp)->m_nOperateType == CK_SOCKET_READ)
		{
			//OutputDebugString(_T("CTcpWorker::ConsumeReq read suc"));
			InterlockedExchangeAdd(&m_nRecvedNum,1);
			InterlockedExchangeAdd(&m_nRecvBytesNum, bytenumber);
			InterlockedExchangeAdd(&m_nPacketNum, bytenumber);

			if (bytenumber == 0)//peer socket closed
			{
				//OutputDebugString(_T("CTcpWorker::ConsumeReq peer socket is closed"));
				m_BufList.PushTail(((CTcpWorkerIOReq*)povlp)->m_pDataBuf);
				m_IOReqList.PushTail((CTcpWorkerIOReq*)povlp);
				UninitObject();
				if (IsOver())
				{
					CString tempstr;
					tempstr.Format(_T("CTcpWorker::ConsumeReq %X:%u - %X:%u session is over1"), ntohl(m_nServerIp), ntohs(m_nServerPort), ntohl(m_nClientIp), ntohs(m_nClientPort));
					OutputDebugString(tempstr);
					m_pListener->ReleaseObject(this);
				}
				return -1;
			}
			else
			{
				res = ReadedDataProcess(((CTcpWorkerIOReq*)povlp)->m_pDataBuf,bytenumber);
				m_IOReqList.PushTail((CTcpWorkerIOReq*)povlp);
				if (res < 0)
				{
					UninitObject();
					if (IsOver())
					{
						CString tempstr;
						tempstr.Format(_T("CTcpWorker::ConsumeReq %X:%u - %X:%u session is over2"), ntohl(m_nServerIp), ntohs(m_nServerPort), ntohl(m_nClientIp), ntohs(m_nClientPort));
						OutputDebugString(tempstr);
						m_pListener->ReleaseObject(this);
					}
					return -2;
				}
			}
		}
		else if (((CTcpWorkerIOReq*)povlp)->m_nOperateType == CK_SOCKET_WRITE)
		{
			InterlockedExchangeAdd(&m_nWritedNum,1);
			m_BufList.PushTail(((CTcpWorkerIOReq*)povlp)->m_pDataBuf);
			m_IOReqList.PushTail((CTcpWorkerIOReq*)povlp);
			//OutputDebugString(_T("CTcpWorker::ConsumeReq write suc"));
			return 0;
		}
		else
		{
			OutputDebugString(_T("CTcpWorker::ConsumeReq unknown type"));
			m_BufList.PushTail(((CTcpWorkerIOReq*)povlp)->m_pDataBuf);
			m_IOReqList.PushTail((CTcpWorkerIOReq*)povlp);
			UninitObject();
			InterlockedExchangeAdd(&m_nErrorNum,1);
			if (IsOver())
			{
				CString tempstr;
				tempstr.Format(_T("CTcpWorker::ConsumeReq %X:%u - %X:%u session is over3"), ntohl(m_nServerIp), ntohs(m_nServerPort), ntohl(m_nClientIp), ntohs(m_nClientPort));
				OutputDebugString(tempstr);
				m_pListener->ReleaseObject(this);
			}
			return -3;
		}
	}
	else
	{
		if (povlp != NULL)
		{
			if (((CTcpWorkerIOReq*)povlp)->m_nOperateType == CK_SOCKET_READ)
			{
				OutputDebugString(_T("CTcpWorker::ConsumeReq iocomplete read error"));
			}
			else if (((CTcpWorkerIOReq*)povlp)->m_nOperateType == CK_SOCKET_WRITE)
			{
				OutputDebugString(_T("CTcpWorker::ConsumeReq iocomplete write error"));
			}
			else 
			{
				OutputDebugString(_T("CTcpWorker::ConsumeReq iocomplete unkown type error"));
			}

			if (((CTcpWorkerIOReq*)povlp)->m_pDataBuf != NULL)
				m_BufList.PushTail(((CTcpWorkerIOReq*)povlp)->m_pDataBuf);

			m_IOReqList.PushTail((CTcpWorkerIOReq*)povlp);
		}
		m_nStatus = STATUSERROR;
		UninitObject();
		InterlockedExchangeAdd(&m_nErrorNum,1);
		if (IsOver())
		{
			CString tempstr;
			tempstr.Format(_T("CTcpWorker::ConsumeReq %X:%u - %X:%u session is over4"), ntohl(m_nServerIp), ntohs(m_nServerPort), ntohl(m_nClientIp), ntohs(m_nClientPort));
			OutputDebugString(tempstr);
			m_pListener->ReleaseObject(this);
		}
		return -4;
	}

	//再次接收
	res = Read();
	if (res < 0)
	{
		UninitObject();
		if (IsOver())
		{
			CString tempstr;
			tempstr.Format(_T("CTcpWorker::ConsumeReq %X:%u - %X:%u session is over"), ntohl(m_nServerIp), ntohs(m_nServerPort), ntohl(m_nClientIp), ntohs(m_nClientPort));
			OutputDebugString(tempstr);
			m_pListener->ReleaseObject(this);
		}
	}

	return 0;
}

int CTcpWorker::InitObject(unsigned int serverip, int serverport, unsigned int clientip, int clientport, SOCKET& insock, CObjectAllocater* pallocater)
{
	if (insock == INVALID_SOCKET)
		return -1;

	m_TcpSocket = insock;
	m_nServerIp = serverip;
	m_nServerPort = serverport;
	m_nClientIp = clientip;
	m_nClientPort = clientport;

	m_pListener = pallocater;

	unsigned char* ptempbuf;
	for (int i = 0; i < 1; i++)
	{
		ptempbuf = new unsigned char[RECVBUFSIZE];
		m_BufList.PushTail(ptempbuf+4);
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}

	CTcpWorkerIOReq* ptempioreq;
	for (int i = 0; i <1; i++)
	{
		ptempioreq = new CTcpWorkerIOReq(this);
		m_IOReqList.PushTail(ptempioreq);
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	m_nStatus = STATUSSTOP;
	return 0;
}

int CTcpWorker::UninitObject()
{
	Stop();
	return 0;
}

int CTcpWorker::Start()
{
	if (m_nStatus == STATUSRUN)
	{
		OutputDebugString(_T("CTcpWorker::Start already run"));
		return 1;
	}

	if (!CIOCompletionPortBase::AssociateDevice((HANDLE)m_TcpSocket, CK_SOCKET_READ))
	{
		OutputDebugString(_T("CTcpWorker::Start iocompletionport AssociateDevice fail"));
		return -1;
	}

	m_nRecvNum = 0;
	m_nRecvedNum = 0;

	m_nWriteNum = 0;
	m_nWritedNum = 0;

	m_nRecvBytesNum = 0;

	if (0 == Read())
	{
		m_nStatus = STATUSRUN;
		return 0;
	}
	else
		return -2;
}

int CTcpWorker::Stop()
{
	m_nStatus = STATUSSTOP;

	if (m_TcpSocket != INVALID_SOCKET)
	{
		closesocket(m_TcpSocket);
		m_TcpSocket = INVALID_SOCKET;
	}

	return 0;
}

int CTcpWorker::Read()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[RECVBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpWorker::Read alloc new buf fail"));
			return -1;
		}
		pDataBuf += 4;
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	*(long*)(pDataBuf - 4) = 0;

	CTcpWorkerIOReq* pIOReq = (CTcpWorkerIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpWorkerIOReq(this);
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpWorker::Read alloc new ioreq fail"));
			return -2;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	else
	{
		pIOReq->m_pObserver = this;
	}
	pIOReq->m_nOperateType = CK_SOCKET_READ;
	pIOReq->m_pDataBuf = pDataBuf;
	pIOReq->m_nBufSize = RECVBUFSIZE-4;

	m_nFlag = 0;
	m_unRecvedNumber = 0;
	m_RWSABUF.buf = (char*)pDataBuf;
	m_RWSABUF.len = RECVBUFSIZE-4;
	int result = WSARecv(m_TcpSocket, &m_RWSABUF, 1, &m_unRecvedNumber, &m_nFlag, pIOReq, NULL);
	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			m_nStatus = STATUSERROR;
			m_BufList.PushTail(pDataBuf);
			m_IOReqList.PushTail(pIOReq);
			OutputDebugString(_T("CTcpWorker::Read WSARecv Fail"));
			return -4;
		}
		else
		{
			//OutputDebugString(_T("CTcpWorker::Read"));
		}
	}
	else
	{
		//OutputDebugString(_T("CTcpWorker::Read WSARecv suc directly"));
	}
	InterlockedExchangeAdd(&m_nRecvNum, 1);

	return 0;
}

int CTcpWorker::Write(unsigned char* pdatabuf, int datsize)
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[RECVBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpWorker::Write alloc new buf fail"));
			return -1;
		}
		pDataBuf += 4;
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	CTcpWorkerIOReq* pIOReq = (CTcpWorkerIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpWorkerIOReq(this);
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpWorker::Write alloc new ioreq fail"));
			return -2;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	else
	{
		pIOReq->m_pObserver = this;
	}
	pIOReq->m_nOperateType = CK_SOCKET_WRITE;
	pIOReq->m_pDataBuf = pDataBuf;
	memcpy(pIOReq->m_pDataBuf, pdatabuf, datsize);
	pIOReq->m_nBufSize = datsize;

	m_nFlag = 0;
	m_WWSABUF.buf = (char*)pDataBuf;
	m_WWSABUF.len = datsize;
	int result = WSASend(m_TcpSocket, &m_WWSABUF, 1, NULL, m_nFlag, pIOReq, NULL);
	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			m_nStatus = STATUSERROR;
			m_BufList.PushTail(pDataBuf);
			m_IOReqList.PushTail(pIOReq);
			OutputDebugString(_T("CTcpWorker::Write WSASend Fail"));
			return -4;
		}
		else
		{
			//OutputDebugString(_T("CTcpWorker::Write"));
		}
	}
	else
	{
		//OutputDebugString(_T("CTcpWorker::Write WSASend suc directly"));
	}
	InterlockedExchangeAdd(&m_nWriteNum, 1);

	return 0;
}

bool CTcpWorker::IsOver()
{
	if (m_nWriteNum + m_nRecvNum <= m_nRecvedNum + m_nWritedNum + m_nErrorNum)
		return true;

	return false;
}

int CTcpWorker::ReleaseAllList()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	while (pDataBuf != NULL)
	{
		delete (unsigned char*)(pDataBuf-4);
		pDataBuf = (unsigned char*)m_BufList.PopHead();
	}

	CTcpWorkerIOReq* pIOReq = (CTcpWorkerIOReq*)m_IOReqList.PopHead();
	while (pIOReq != NULL)
	{
		delete pIOReq;
		pIOReq = (CTcpWorkerIOReq*)m_IOReqList.PopHead();
	}
	return 0;
}

int CTcpWorker::ReadedDataProcess(unsigned char* pdatabuf, int datasize)
{
	int res = Write(pdatabuf, datasize);
	m_BufList.PushTail(pdatabuf);
	return res;
}
