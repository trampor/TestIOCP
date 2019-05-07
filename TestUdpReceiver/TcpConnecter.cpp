#include "stdafx.h"
#include "TcpConnecter.h"

CNoLockBiList<unsigned char*> CTcpConnecter::m_BufList;
CNoLockBiList<CTcpConnecterIOReq*> CTcpConnecter::m_IOReqList;

CTcpConnecter::CTcpConnecter()
{
	m_TcpSocket = INVALID_SOCKET;
	m_nStatus = STATUSSTOP;
	m_nRecvNum = 0;
	m_nRecvedNum = 0;
	m_nErrorNum = 0;
	m_nWriteNum = 0;
	m_nWritedNum = 0;
	m_bConnected = false;

	lpfnConnectEx = NULL;
	lpfnDisConnectEx = NULL; 

	m_pAllocater = NULL;
}


CTcpConnecter::~CTcpConnecter()
{
}

int CTcpConnecter::ReleaseAllList()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	while (pDataBuf != NULL)
	{
		delete (unsigned char*)(pDataBuf - 4);
		pDataBuf = (unsigned char*)m_BufList.PopHead();
	}

	CTcpConnecterIOReq* pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
	while (pIOReq != NULL)
	{
		delete pIOReq;
		pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
	}
	return 0;
}

int CTcpConnecter::ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp)
{
	if (error == 0)
	{
		if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_CONNECT)
		{
			InterlockedExchangeAdd(&m_nRecvedNum, 1);
			m_bConnected = true;
			m_nConnectEnd = GetTickCount();
			m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
			//OutputDebugString(_T("CTcpConnecter::ConsumeReq connect suc"));

			SOCKADDR_IN serveraddr;
			ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
			int namelen = sizeof(SOCKADDR_IN);
			getsockname(m_TcpSocket, (sockaddr*)&serveraddr, &namelen);

			TCHAR addrstr[50];
			InetNtopW(serveraddr.sin_family, &serveraddr.sin_addr, addrstr, 50);
			int port = ntohs(serveraddr.sin_port);
			CString tempstr;
			tempstr.Format(_T("%s:%u"), addrstr, port);
			m_sClientAddr = tempstr;

			if (Read() < 0)
			{
				UninitObject();
				m_nStatus = STATUSERROR;
				if (IsOver())
				{
					CString tempstr;
					tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over1"),m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
					OutputDebugString(tempstr);
					if (m_pAllocater != NULL)
						m_pAllocater->ReleaseObject(this);
				}
				return -1;
			}
			return 0;
		}
		else if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_DISCONNECT)
		{
			m_bConnected = false;
			m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
			//OutputDebugString(_T("CTcpConnecter::ConsumeReq disconnect suc"));

			InterlockedExchangeAdd(&m_nRecvedNum, 1);
			if (IsOver())
			{
				CString tempstr;
				tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over2"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
				OutputDebugString(tempstr);
				if(m_pAllocater != NULL)
					m_pAllocater->ReleaseObject(this);
			}

			return 0;
		}
		if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_READ)
		{
			//OutputDebugString(_T("CTcpConnecter::ConsumeReq read suc"));
			InterlockedExchangeAdd(&m_nRecvedNum, 1);
			InterlockedExchangeAdd(&m_nRecvBytesNum, bytenumber);
			InterlockedExchangeAdd(&m_nPacketNum, bytenumber);

			if (bytenumber == 0)//peer socket closed
			{
				//OutputDebugString(_T("CTcpConnecter::ConsumeReq peer socket is closed"));
				m_BufList.PushTail(((CTcpConnecterIOReq*)povlp)->m_pDataBuf);
				m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
				UninitObject();
				if (IsOver())
				{
					CString tempstr;
					tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over3"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
					OutputDebugString(tempstr);
					if (m_pAllocater != NULL)
						m_pAllocater->ReleaseObject(this);
				}
				return -1;
			}
			else
			{
				int res = ReadedDataProcess(((CTcpConnecterIOReq*)povlp)->m_pDataBuf, bytenumber);
				m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
				if (res < 0)
				{
					UninitObject();
					if (IsOver())
					{
						CString tempstr;
						tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over4"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
						OutputDebugString(tempstr);
						if (m_pAllocater != NULL)
							m_pAllocater->ReleaseObject(this);
					}
					return -2;
				}
			}
			if (Read() < 0)
			{
				UninitObject();
				m_nStatus = STATUSERROR;
				if (IsOver())
				{
					CString tempstr;
					tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over5"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
					OutputDebugString(tempstr);
					if (m_pAllocater != NULL)
						m_pAllocater->ReleaseObject(this);
				}
				return -1;
			}
		}
		else if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_WRITE)
		{
			InterlockedExchangeAdd(&m_nWritedNum, 1);
			m_BufList.PushTail(((CTcpConnecterIOReq*)povlp)->m_pDataBuf);
			m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
			//OutputDebugString(_T("CTcpConnecter::ConsumeReq write suc"));
			return 0;
		}
		else
		{
			InterlockedExchangeAdd(&m_nErrorNum, 1);
			OutputDebugString(_T("CTcpConnecter::ConsumeReq unknown type"));
			m_BufList.PushTail(((CTcpConnecterIOReq*)povlp)->m_pDataBuf);
			m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
			UninitObject();
			if (IsOver())
			{
				CString tempstr;
				tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over6"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
				OutputDebugString(tempstr);
				if (m_pAllocater != NULL)
					m_pAllocater->ReleaseObject(this);
			}
			return -2;
		}
	}
	else
	{
		if (povlp != NULL)
		{
			if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_READ)
			{
				OutputDebugString(_T("CTcpConnecter::ConsumeReq iocomplete read error"));
			}
			else if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_CONNECT)
			{
				OutputDebugString(_T("CTcpConnecter::ConsumeReq iocomplete connect error"));
			}
			else if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_DISCONNECT)
			{
				OutputDebugString(_T("CTcpConnecter::ConsumeReq iocomplete disconnect error"));
			}
			else if (((CTcpConnecterIOReq*)povlp)->m_nOperateType == CK_SOCKET_WRITE)
			{
				OutputDebugString(_T("CTcpConnecter::ConsumeReq iocomplete write error"));
			}
			else
			{
				OutputDebugString(_T("CTcpConnecter::ConsumeReq iocomplete unkown type error"));
			}

			if (((CTcpConnecterIOReq*)povlp)->m_pDataBuf != NULL)
				m_BufList.PushTail(((CTcpConnecterIOReq*)povlp)->m_pDataBuf);

			m_IOReqList.PushTail((CTcpConnecterIOReq*)povlp);
		}
		UninitObject();
		m_nStatus = STATUSERROR;
		InterlockedExchangeAdd(&m_nErrorNum, 1);
		if(IsOver())
		{
			CString tempstr;
			tempstr.Format(_T("CTcpConnecter::ConsumeReq %s-%s:%u session is over7"), m_sClientAddr.c_str(), m_sServerIp.c_str(), m_nServerPort);
			OutputDebugString(tempstr);
			if (m_pAllocater != NULL)
				m_pAllocater->ReleaseObject(this);
		}
		return -1;
	}

	return 0;
}

int CTcpConnecter::InitObject(LPCTSTR pserverip, int serverport, CObjectAllocater* pallocater)
{
	if (pserverip == NULL)
	{
		OutputDebugString(_T("CTcpConnecter::InitObject pserverip parameter error"));
		return -1;
	}
	else
		m_sServerIp = pserverip;

	m_nServerPort = serverport;

	m_pAllocater = pallocater;

	unsigned char* ptempbuf;
	for (int i = 0; i < 1; i++)
	{
		ptempbuf = new unsigned char[RECVBUFSIZE];
		m_BufList.PushTail(ptempbuf + 4);
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}

	CTcpConnecterIOReq* ptempioreq;
	for (int i = 0; i <1; i++)
	{
		ptempioreq = new CTcpConnecterIOReq;
		m_IOReqList.PushTail(ptempioreq);
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	m_TcpSocket = MakeTcpClientSocket((m_sServerIp.length() == 0) ? NULL : m_sServerIp.c_str(), m_nServerPort);
	if (m_TcpSocket == INVALID_SOCKET)
		return -1;

	return 0;
}

int CTcpConnecter::UninitObject()
{
	Stop();

	return 0;
}

int CTcpConnecter::Start()
{
	if (m_nStatus == STATUSRUN)
	{
		OutputDebugString(_T("CTcpConnecter::Start already run"));
		return 1;
	}

	if (!CIOCompletionPortBase::AssociateDevice((HANDLE)m_TcpSocket, CK_SOCKET_READ))
	{
		OutputDebugString(_T("CTcpConnecter::Start AssociateDevice fail"));
		return -4;
	}

	m_nRecvNum = 0;
	m_nRecvedNum = 0;
	m_nErrorNum = 0;
	m_nWriteNum = 0;
	m_nWritedNum = 0;
	m_nRecvBytesNum = 0;

	m_bConnected = false;

	if (Connect() == 0)
	{
		m_nStatus = STATUSRUN;
		return 0;
	}
	else
	{
		return -3;
	}
}

int CTcpConnecter::Connect()
{
	//调用ConnectEx函数	
	//向服务投递一个连接的的请求
	CTcpConnecterIOReq* pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpConnecterIOReq();
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Connect alloc new ioreq fail"));
			return -3;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	pIOReq->m_pDataBuf = NULL;
	pIOReq->m_nBufSize = 0;
	pIOReq->m_pObserver = this;
	pIOReq->m_nOperateType = CK_SOCKET_CONNECT;

	m_nConnectStart = GetTickCount();
	DWORD bytessend=0;
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(m_nServerPort);
	InetPtonW(AF_INET, m_sServerIp.c_str(), (void*)&serveraddr.sin_addr);
	DWORD error;
	if (!lpfnConnectEx(m_TcpSocket, (const struct sockaddr FAR*)&serveraddr, sizeof(SOCKADDR_IN), NULL, 0, &bytessend, pIOReq))
	{
		if ((error=WSAGetLastError()) != ERROR_IO_PENDING)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Connect connectex fail"));
			m_IOReqList.PushTail(pIOReq);
			return -4;
		}
	}
	InterlockedExchangeAdd(&m_nRecvNum, 1);
	int iResult = setsockopt(m_TcpSocket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);

	return 0;
}

int CTcpConnecter::Stop()
{
	m_nStatus = STATUSSTOP;

	if (m_TcpSocket != INVALID_SOCKET)
	{
		if (m_bConnected)
		{
			CTcpConnecterIOReq* pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
			if (pIOReq == NULL)
			{
				pIOReq = new CTcpConnecterIOReq();
				if (pIOReq == NULL)
				{
					m_nStatus = STATUSERROR;
					OutputDebugString(_T("CTcpConnecter::Connect alloc new ioreq fail"));
				}
				else
					InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
			}
			m_nFlag = 0;
			if (pIOReq != NULL)
			{
				int error;
				pIOReq->m_pDataBuf = NULL;
				pIOReq->m_nBufSize = 0;
				pIOReq->m_pObserver = this;
				pIOReq->m_nOperateType = CK_SOCKET_DISCONNECT;
				if (!lpfnDisConnectEx(m_TcpSocket, pIOReq, m_nFlag, 0))
				{
					if ((error = WSAGetLastError()) != ERROR_IO_PENDING)
					{
						m_nStatus = STATUSERROR;
						OutputDebugString(_T("CTcpConnecter::Stop disconnectex fail"));
						m_IOReqList.PushTail(pIOReq);
					}
					else
					{
						InterlockedExchangeAdd(&m_nRecvNum, 1);
//						OutputDebugString(_T("CTcpConnecter::Stop disconnect"));
					}
				}
				else
				{
					InterlockedExchangeAdd(&m_nRecvNum, 1);
//					OutputDebugString(_T("CTcpConnecter::Stop disconnect suc directly"));
				}
			}
		}
		closesocket(m_TcpSocket);
		m_TcpSocket = INVALID_SOCKET;
	}

	m_nRecvBytesNum = 0;

	return 0;
}

SOCKET CTcpConnecter::MakeTcpClientSocket(LPCTSTR pserverip, int serverport)
{
	if (pserverip == NULL)
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket pserverip parameter error"));
		return INVALID_SOCKET;
	}

	SOCKET hTcpSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hTcpSocket == INVALID_SOCKET)
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket WSASocket fail"));
		return INVALID_SOCKET;
	}

	int err;
	BOOL reuse = TRUE;
	err = setsockopt(hTcpSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if (err != 0)
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket setsockopt SO_REUSEADDR fail"));
		closesocket(hTcpSocket);
		return INVALID_SOCKET;
	}

	sockaddr_in local_addr;
	ZeroMemory(&local_addr, sizeof(sockaddr_in));
	local_addr.sin_family = AF_INET;
	if (bind(hTcpSocket, (const struct sockaddr FAR*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket bind fail"));
		closesocket(hTcpSocket);
		return INVALID_SOCKET;
	}

	DWORD dwbytes;
	GUID guidConnectEx = WSAID_CONNECTEX;
	if (0 != WSAIoctl(hTcpSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwbytes, NULL, NULL))
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket WSAIoctl GET lpfnConnectEx fail"));
		closesocket(hTcpSocket);
		return INVALID_SOCKET;
	}

	GUID guidDisConnectEx = WSAID_DISCONNECTEX;
	if (0 != WSAIoctl(hTcpSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisConnectEx, sizeof(guidDisConnectEx), &lpfnDisConnectEx, sizeof(lpfnDisConnectEx), &dwbytes, NULL, NULL))
	{
		OutputDebugString(_T("CTcpConnecter::MakeTcpClientSocket WSAIoctl GET lpfnDisConnectEx fail"));
		closesocket(hTcpSocket);
		return INVALID_SOCKET;
	}

	return hTcpSocket;
}

int CTcpConnecter::ReadedDataProcess(unsigned char* pdatabuf, int datasize)
{
	m_BufList.PushTail(pdatabuf);
	return 0;
}

bool CTcpConnecter::IsOver()
{
	if (m_nWriteNum + m_nRecvNum <= m_nRecvedNum + m_nWritedNum + m_nErrorNum)
		return true;

	return false;
}

int CTcpConnecter::Read()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[RECVBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Read alloc new buf fail"));
			return -1;
		}
		pDataBuf += 4;
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	*(long*)(pDataBuf - 4) = 0;

	CTcpConnecterIOReq* pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpConnecterIOReq();
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Read alloc new ioreq fail"));
			return -2;
		}
		pIOReq->m_pObserver = this;
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	else
	{
		pIOReq->m_pObserver = this;
	}
	pIOReq->m_nOperateType = CK_SOCKET_READ;
	pIOReq->m_pDataBuf = pDataBuf;
	pIOReq->m_nBufSize = RECVBUFSIZE - 4;

	m_nFlag = 0;
	m_unRecvedNumber = 0;
	m_RWSABUF.buf = (char*)pDataBuf;
	m_RWSABUF.len = RECVBUFSIZE - 4;
	int result = WSARecv(m_TcpSocket, &m_RWSABUF, 1, &m_unRecvedNumber, &m_nFlag, pIOReq, NULL);
	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			m_nStatus = STATUSERROR;
			m_BufList.PushTail(pDataBuf);
			m_IOReqList.PushTail(pIOReq);
			OutputDebugString(_T("CTcpConnecter::Read WSARecv Fail"));
			return -4;
		}
		else
		{
//			OutputDebugString(_T("CTcpConnecter::Read"));
		}
	}
	else
	{
//		OutputDebugString(_T("CTcpConnecter::Read WSARecv suc directly"));
	}
	InterlockedExchangeAdd(&m_nRecvNum, 1);

	return 0;
}

int CTcpConnecter::Write(unsigned char* pdatabuf, int datsize)
{
	if ((m_nStatus != STATUSRUN) || !m_bConnected)
	{
		OutputDebugString(_T("CTcpConnecter::Write object status error"));
		return -1;
	}

	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[RECVBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Write alloc new buf fail"));
			return -2;
		}
		pDataBuf += 4;
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	CTcpConnecterIOReq* pIOReq = (CTcpConnecterIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpConnecterIOReq();
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpConnecter::Write alloc new ioreq fail"));
			return -3;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	pIOReq->m_pObserver = this;
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
			OutputDebugString(_T("CTcpConnecter::Write WSASend Fail"));
			return -4;
		}
		else
		{
//			OutputDebugString(_T("CTcpConnecter::Write"));
		}
	}
	else
	{
//		OutputDebugString(_T("CTcpConnecter::Write WSASend suc directly"));
	}
	InterlockedExchangeAdd(&m_nWriteNum, 1);

	return 0;
}