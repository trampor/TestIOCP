#include "stdafx.h"
#include "TcpListenerBase.h"


CNoLockBiList<unsigned char*> CTcpListenerBase::m_BufList;
CNoLockBiList<SOCKET> CTcpListenerBase::m_SocketList;
CNoLockBiList<CTcpListenerIOReq*> CTcpListenerBase::m_IOReqList;

CTcpListenerBase::CTcpListenerBase()
{
	m_TcpSocket = INVALID_SOCKET;

	m_nStatus = STATUSSTOP;

	m_nAcceptNum = 0;

	lpfnAcceptEx = NULL;
}


CTcpListenerBase::~CTcpListenerBase()
{
}

int CTcpListenerBase::ReleaseAllList()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	while (pDataBuf != NULL)
	{
		delete (unsigned char*)(pDataBuf);
		pDataBuf = (unsigned char*)m_BufList.PopHead();
	}

	SOCKET sock = (SOCKET)m_SocketList.PopHead();
	while (sock != NULL)
	{
		closesocket(sock);
		sock = (SOCKET)m_SocketList.PopHead();
	}

	CTcpListenerIOReq* pIOReq = (CTcpListenerIOReq*)m_IOReqList.PopHead();
	while (pIOReq != NULL)
	{
		delete pIOReq;
		pIOReq = (CTcpListenerIOReq*)m_IOReqList.PopHead();
	}
	return 0;
}

int CTcpListenerBase::InitObject(LPCTSTR plocalip, int port)
{
	if (plocalip == NULL)
	{
		OutputDebugString(_T("CTcpListener::InitObject plocalip parameter error"));
		return -1;
	}
	else
		m_sLocalIp = plocalip;

	m_nPort = port;

	unsigned char* ptempbuf;
	for (int i = 0; i < 1; i++)
	{
		ptempbuf = new unsigned char[LISTENBUFSIZE];
		m_BufList.PushTail(ptempbuf);
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}

	SOCKET sock;
	for (int i = 0; i < 1000; i++)
	{
		sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		m_SocketList.PushTail(sock);
		InterlockedExchangeAdd(&m_SocketList.m_nAllocatedEleNum, 1);
	}

	CTcpListenerIOReq* ptempioreq;
	for (int i = 0; i <1; i++)
	{
		ptempioreq = new CTcpListenerIOReq(this);
		m_IOReqList.PushTail(ptempioreq);
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	return 0;
}

int CTcpListenerBase::UninitObject()
{
	Stop();
	return 0;
}

int CTcpListenerBase::Start(CIOCompletionPortBase* piocompletionport)
{
	if (m_nStatus == STATUSRUN)
	{
		OutputDebugString(_T("CUdpReceiver::Start already run"));
		return 1;
	}

	m_TcpSocket = MakeTcpServerSocket((m_sLocalIp.length() == 0) ? NULL : m_sLocalIp.c_str(), m_nPort);
	if (m_TcpSocket == INVALID_SOCKET)
	{
		m_nStatus = STATUSERROR;
		return -1;
	}

	if (piocompletionport != NULL)
	{
		if (!piocompletionport->AssociateDevice((HANDLE)m_TcpSocket, CK_SOCKET_LISTEN))
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpListener::Start iocompletionport AssociateDevice fail"));
			return -2;
		}
	}
	if (Accept() == 0)
	{
		m_nStatus = STATUSRUN;
		return 0;
	}
	else
	{
		return -3;
	}
}

int CTcpListenerBase::Stop()
{
	m_nStatus = STATUSSTOP;

	if (m_TcpSocket != INVALID_SOCKET)
	{
		closesocket(m_TcpSocket);
		m_TcpSocket = INVALID_SOCKET;
	}

	return 0;
}

SOCKET CTcpListenerBase::MakeTcpServerSocket(LPCTSTR plocalip, int port)
{
	if (plocalip == NULL)
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket plocalip parameter error"));
		return INVALID_SOCKET;
	}

	SOCKET hTcpServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hTcpServerSocket == INVALID_SOCKET)
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket WSASocket fail"));
		return INVALID_SOCKET;
	}

	int err;
	BOOL reuse = TRUE;
	err = setsockopt(hTcpServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if (err != 0)
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket setsockopt SO_REUSEADDR fail"));
		closesocket(hTcpServerSocket);
		return INVALID_SOCKET;
	}

	SOCKADDR_IN bindAddr;
	bindAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, plocalip, (void*)&bindAddr.sin_addr);
	bindAddr.sin_port = htons(port);
	if (bind(hTcpServerSocket, (const struct sockaddr FAR*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket bind fail"));
		closesocket(hTcpServerSocket);
		return INVALID_SOCKET;
	}

	err = listen(hTcpServerSocket, 1000);
	if (err == SOCKET_ERROR)
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket listen fail"));
		closesocket(hTcpServerSocket);
		return INVALID_SOCKET;
	}

	DWORD dwbytes;
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	if (0 != WSAIoctl(hTcpServerSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwbytes, NULL, NULL))
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket WSAIoctl GET lpfnAcceptEx fail"));
		closesocket(hTcpServerSocket);
		return INVALID_SOCKET;
	}

	// 获取GetAcceptExSockAddrs函数指针，也是同理	
	GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	if (0 != WSAIoctl(hTcpServerSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs), &lpfnGetAcceptExSockaddrs, sizeof(lpfnGetAcceptExSockaddrs), &dwbytes, NULL, NULL))
	{
		OutputDebugString(_T("CTcpListener::MakeTcpServerSocket WSAIoctl GET lpfnGetAcceptExSockaddrs fail"));
		closesocket(hTcpServerSocket);
		return INVALID_SOCKET;
	}

	return hTcpServerSocket;
}

int CTcpListenerBase::GetPacketNum()
{
	return m_nAcceptNum;
}

int CTcpListenerBase::GetStatus()
{
	return m_nStatus;
}

int CTcpListenerBase::Accept()
{
	//调用AcceptEx函数，地址长度需要在原有的上面加上16个字节	
	//向服务线程投递一个接收连接的的请求
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[LISTENBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpListenerBase::Accept alloc new buf fail"));
			return -1;
		}
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	SOCKET sock = (SOCKET)m_SocketList.PopHead();
	if (sock == NULL)
	{
		sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sock == INVALID_SOCKET)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpListenerBase::Accept alloc new socket fail"));
			m_BufList.PushTail(pDataBuf);
			return -2;
		}
		InterlockedExchangeAdd(&m_SocketList.m_nAllocatedEleNum, 1);
	}
	CTcpListenerIOReq* pIOReq = (CTcpListenerIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CTcpListenerIOReq(this);
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpListenerBase::Accept alloc new ioreq fail"));
			m_BufList.PushTail(pDataBuf);
			m_SocketList.PushTail(sock);
			return -3;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	DWORD dwBytes;
	pIOReq->m_Socket = sock;
	pIOReq->m_pDataBuf = pDataBuf;
	pIOReq->m_nBufSize = LISTENBUFSIZE;
	pIOReq->m_pObserver = this;
	if (!lpfnAcceptEx(m_TcpSocket, pIOReq->m_Socket, pIOReq->m_pDataBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, pIOReq))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CTcpListenerBase::Accept acceptex fail"));
			m_BufList.PushTail(pDataBuf);
			m_SocketList.PushTail(sock);
			m_IOReqList.PushTail(pIOReq);
			return -4;
		}
	}
	return 0;
}
