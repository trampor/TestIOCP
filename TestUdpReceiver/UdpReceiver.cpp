#include "stdafx.h"
#include "UdpReceiver.h"

CNoLockBiList<unsigned char*> CUdpReceiver::m_BufList;
CNoLockBiList<CUdpReceiverIOReq*> CUdpReceiver::m_IOReqList;

CUdpReceiver::CUdpReceiver()
{
	m_nPacketNum = 0;
	m_nRecvBytesNum = 0;
	m_nStatus = STATUSSTOP;

	m_nRecvNum = 0;
	m_nRecvedNum = 0;

	m_UdpSocket = INVALID_SOCKET;
	
	InitializeSRWLock(&m_ObserverLock);
}

CUdpReceiver::~CUdpReceiver()
{
	//because all receiver object use the two list,so can't release buf in one receiver's deconstruct
}

int CUdpReceiver::ReleaseAllList()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	while (pDataBuf != NULL)
	{
		delete (unsigned char*)(pDataBuf - 4);
		pDataBuf = (unsigned char*)m_BufList.PopHead();
	}

	CUdpReceiverIOReq* pIOReq = (CUdpReceiverIOReq*)m_IOReqList.PopHead();
	while (pIOReq != NULL)
	{
		delete pIOReq;
		pIOReq = (CUdpReceiverIOReq*)m_IOReqList.PopHead();
	}
	return 0;
}

int CUdpReceiver::ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp)
{
	m_nRecvedNum++;
	if (error == 0)
	{
		InterlockedExchangeAdd(&m_nRecvBytesNum, bytenumber);
		InterlockedExchangeAdd(&m_nPacketNum, bytenumber);

		ReadedDataProcess(((CUdpReceiverIOReq*)povlp)->m_pDataBuf, bytenumber);
		m_IOReqList.PushTail((CUdpReceiverIOReq*)povlp);
	}
	else
	{
		if (povlp != NULL)
		{
			if (((CUdpReceiverIOReq*)povlp)->m_pDataBuf != NULL)
				m_BufList.PushTail(((CUdpReceiverIOReq*)povlp)->m_pDataBuf);

			m_IOReqList.PushTail((CUdpReceiverIOReq*)povlp);
		}
		m_nStatus = STATUSERROR;
		OutputDebugString(_T("CUdpReceiver::ConsumeReq iocomplete error"));
		return -1;
	}

	//再次接收
	Read();

	return 0;
}

int CUdpReceiver::InitObject(LPCTSTR plocalip, LPCTSTR psourceip, LPCTSTR pmultiip, int port)
{
	if (pmultiip == NULL)
	{
		OutputDebugString(_T("CUdpReceiver::InitObject pmultiip parameter error"));
		return -1;
	}
	else
		m_sMulticastIp = pmultiip;

	if (plocalip != NULL)
		m_sLocalIp = plocalip;

	if (psourceip != NULL)
		m_sSourceIp = psourceip;

	m_nMulticastPort = port;

	unsigned char* ptempbuf;
	for (int i = 0; i < 1; i++)
	{
		ptempbuf = new unsigned char[RECVBUFSIZE];
		m_BufList.PushTail(ptempbuf+4);
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}

	CUdpReceiverIOReq* ptempioreq;
	for (int i = 0; i <1; i++)
	{
		ptempioreq = new CUdpReceiverIOReq(this);
		m_IOReqList.PushTail(ptempioreq);
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	return 0;
}

int CUdpReceiver::UninitObject()
{
	Stop();

	return 0;
}

int CUdpReceiver::Start(CIOCompletionPortBase* piocompletionport)
{
	if (m_nStatus == STATUSRUN)
	{
		OutputDebugString(_T("CUdpReceiver::Start already run"));
		return 1;
	}

	m_UdpSocket = MakeMultiSocket((m_sLocalIp.length() == 0) ? NULL : m_sLocalIp.c_str(), (m_sSourceIp.length() == 0) ? NULL : m_sSourceIp.c_str(), (m_sMulticastIp.length() == 0) ? NULL : m_sMulticastIp.c_str(), m_nMulticastPort);
	if (m_UdpSocket == INVALID_SOCKET)
		return -1;

	if (piocompletionport != NULL)
	{
		if (!piocompletionport->AssociateDevice((HANDLE)m_UdpSocket, CK_SOCKET_READ))
		{
			OutputDebugString(_T("CUdpReceiver::Start iocompletionport AssociateDevice fail"));
			return -4;
		}
	}
	
	m_nRecvNum = 0;
	m_nRecvedNum = 0;
	m_nRecvBytesNum = 0;

	Read();
	return 0;
}

int CUdpReceiver::Read()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	if (pDataBuf == NULL)
	{
		pDataBuf = new unsigned char[RECVBUFSIZE];
		if (pDataBuf == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CUdpReceiver::Start alloc new buf fail"));
			return -1;
		}
		pDataBuf += 4;
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}
	//it's very important,reserved 4 bytes for buf reference count,
	//before use it,init it to 0,
	//modify it when dispatch buf to observer or return from observer;
	*(long*)(pDataBuf - 4) = 0;

	CUdpReceiverIOReq* pIOReq = (CUdpReceiverIOReq*)m_IOReqList.PopHead();
	if (pIOReq == NULL)
	{
		pIOReq = new CUdpReceiverIOReq(this);
		if (pIOReq == NULL)
		{
			m_nStatus = STATUSERROR;
			OutputDebugString(_T("CUdpReceiver::Start alloc new ioreq fail"));
			return -2;
		}
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}
	else
	{
		pIOReq->m_pObserver = this;
	}
	pIOReq->m_pDataBuf = pDataBuf;
	pIOReq->m_nBufSize = RECVBUFSIZE;

	m_nFlag = 0;
	m_unRecvedNumber = 0;
	m_WSABUF.buf = (char*)pDataBuf;
	m_WSABUF.len = RECVBUFSIZE;
	int result = WSARecv(m_UdpSocket, &m_WSABUF, 1, &m_unRecvedNumber, &m_nFlag, pIOReq, NULL);
	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			m_nStatus = STATUSERROR;
			m_BufList.PushTail(pDataBuf);
			m_IOReqList.PushTail(pIOReq);
			OutputDebugString(_T("CUdpReceiver::Start WSARecv Fail"));
			return -4;
		}
		else
			m_nStatus = STATUSRUN;
	}
	else
	{
		m_nStatus = STATUSRUN;
		OutputDebugString(_T("CUdpReceiver::Start WSARecv suc directly"));
	}
	m_nRecvNum++;

	return 0;
}
int CUdpReceiver::Stop()
{
	m_nStatus = STATUSSTOP;

	if (m_UdpSocket != INVALID_SOCKET)
	{
		closesocket(m_UdpSocket);
		m_UdpSocket = INVALID_SOCKET;
	}

	for (auto iter = m_DataObserverList.begin(); iter != m_DataObserverList.end(); iter++)
	{
		(*iter)->ImportData(NULL, 0, 0, this);
	}

	m_nRecvBytesNum = 0;

	return 0;
}

int CUdpReceiver::AddDataObserver(CDataObserver* pdataobserver)
{
	AcquireSRWLockExclusive(&m_ObserverLock);
	m_DataObserverList.push_back(pdataobserver);
	ReleaseSRWLockExclusive(&m_ObserverLock);
	return 0;
}

int CUdpReceiver::RemoveDataObserver(CDataObserver* pdataobserver)
{
	AcquireSRWLockExclusive(&m_ObserverLock);
	for (auto iter = m_DataObserverList.begin(); iter != m_DataObserverList.end(); iter++)
	{
		if (*iter == pdataobserver)
		{
			m_DataObserverList.erase(iter);
			break;
		}
	}
	ReleaseSRWLockExclusive(&m_ObserverLock);

	return 0;
}

int CUdpReceiver::GetStatus()
{
	return m_nStatus;
}

HANDLE CUdpReceiver::GetFileHandle()
{
	return (HANDLE)m_UdpSocket;
}

int CUdpReceiver::GetPacketNum()
{
	return m_nPacketNum;
}

unsigned long CUdpReceiver::GetRecvBytesNum()
{
	m_nRecvBytesNumOld = m_nRecvBytesNum;
	m_nRecvBytesNum = 0;
	return m_nRecvBytesNumOld;
}

SOCKET CUdpReceiver::MakeMultiSocket( LPCTSTR plocalip, LPCTSTR psourceip, LPCTSTR pmultiip, int port)
{
	if (pmultiip == NULL)
	{
		OutputDebugString(_T("CUdpReceiver::MakeMultiSocket multiip parameter error"));
		return INVALID_SOCKET;
	}

	SOCKET hMulticastSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF | WSA_FLAG_OVERLAPPED);
	if (hMulticastSocket == INVALID_SOCKET)
	{
		OutputDebugString(_T("CUdpReceiver::MakeMultiSocket WSASocket fail"));
		return INVALID_SOCKET;
	}

	int err;
	BOOL reuse = TRUE;
	err = setsockopt(hMulticastSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if (err != 0)
	{
		OutputDebugString(_T("CUdpReceiver::MakeMultiSocket setsockopt SO_REUSEADDR fail"));
		closesocket(hMulticastSocket);
		return INVALID_SOCKET;
	}

	SOCKADDR_IN bindAddr;
	bindAddr.sin_family = AF_INET;
	if (plocalip != NULL)
	{
		InetPtonW(AF_INET, plocalip, (void*)&bindAddr.sin_addr);
	}
	else
	{
		bindAddr.sin_addr.s_addr = INADDR_ANY;
	}
	bindAddr.sin_port = htons(port);
	if (bind(hMulticastSocket, (const struct sockaddr FAR*)&bindAddr, sizeof(bindAddr)) == SOCKET_ERROR)
	{
		OutputDebugString(_T("CUdpReceiver::MakeMultiSocket bind fail"));
		closesocket(hMulticastSocket);
		return INVALID_SOCKET;
	}

	if (psourceip != NULL)
	{
		ip_mreq_source imr;
		InetPtonW(AF_INET, pmultiip, (void*)&imr.imr_multiaddr);
		InetPtonW(AF_INET, psourceip, (void*)&imr.imr_sourceaddr);
		if (plocalip != NULL)
		{
			InetPtonW(AF_INET, plocalip, (void*)&imr.imr_interface);
		}
		else
		{
			imr.imr_interface.s_addr = INADDR_ANY;
		}
		err = setsockopt(hMulticastSocket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (char*)&imr, sizeof(imr));
		if (err != 0)
		{
			CString tempstr;
			tempstr.Format(_T("imr_multiaddr=0x%X imr_sourceaddr=0x%X imr_interface=0x%X error=%d"),
				imr.imr_multiaddr.s_addr, imr.imr_sourceaddr.s_addr, imr.imr_interface.s_addr, WSAGetLastError());
			OutputDebugString(tempstr);
			OutputDebugString(_T("CUdpReceiver::MakeMultiSocket setsockopt IP_ADD_SOURCE_MEMBERSHIP fail"));
			closesocket(hMulticastSocket);
			return INVALID_SOCKET;
		}
		else
			OutputDebugString(_T("CUdpReceiver::MakeMultiSocket setsockopt IP_ADD_SOURCE_MEMBERSHIP suc"));
	}
	else
	{
		SOCKADDR_IN DestAddr;
		DestAddr.sin_family = AF_INET;
		InetPtonW(AF_INET, pmultiip, (void*)&DestAddr.sin_addr);
		DestAddr.sin_port = htons(port);
		if (WSAJoinLeaf(hMulticastSocket, (const struct sockaddr FAR*)&DestAddr, sizeof(DestAddr), NULL, NULL, NULL, NULL, JL_RECEIVER_ONLY) == INVALID_SOCKET)
		{
			OutputDebugString(_T("CUdpReceiver::MakeMultiSocket WSAJoinLeaf fail"));
			closesocket(hMulticastSocket);
			return INVALID_SOCKET;
		}
		else
			OutputDebugString(_T("CUdpReceiver::MakeMultiSocket WSAJoinLeaf suc"));
	}

	int optVal = 512 * 1024;
	err = setsockopt(hMulticastSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, sizeof(int));
	if (err != 0)
	{
		OutputDebugString(_T("CUdpReceiver::MakeMultiSocket setsockopt SO_RCVBUF fail"));
	}

	return hMulticastSocket;
}

int CUdpReceiver::ReturnBuf(unsigned char* pbuf)
{
	if (pbuf == NULL)
	{
		OutputDebugString(_T("CUdpReceiver::ReturnBuf pbuf parameter error"));
		return -1;
	}

	long val = InterlockedExchangeAdd((long*)(pbuf - 4), -1);
	if (val == 1) //返回的是操作前的值，所以判断是否为1，表示引用数是否为0
		m_BufList.PushTail(pbuf);

	return 0;
}

int CUdpReceiver::ReadedDataProcess(unsigned char* pdatabuf, int datasize)
{
	if (m_DataObserverList.empty())
	{
		m_BufList.PushTail(pdatabuf);
	}
	else
	{
		AcquireSRWLockExclusive(&m_ObserverLock);
		for (auto iter = m_DataObserverList.begin(); iter != m_DataObserverList.end(); iter++)
		{
			InterlockedExchangeAdd((long*)(pdatabuf - 4), 1);
			(*iter)->ImportData(pdatabuf, datasize, datasize, this);
		}
		ReleaseSRWLockExclusive(&m_ObserverLock);
	}

	return 0;
}
