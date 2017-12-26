#include "stdafx.h"
#include "TcpIOCPClientStub.h"

int CTcpIOCPClientStub::m_nClosednum = 0;
CTcpIOCPClientStub::CTcpIOCPClientStub()
{
	m_Socket = INVALID_SOCKET;
	m_nStatus = -1;

	InitializeCriticalSection(&m_RecvSection);
	InitializeCriticalSection(&m_SendSection);
	InitializeCriticalSection(&m_CloseSocketSection);
}


CTcpIOCPClientStub::~CTcpIOCPClientStub()
{
	DeleteCriticalSection(&m_RecvSection);
	DeleteCriticalSection(&m_SendSection);
	DeleteCriticalSection(&m_CloseSocketSection);
}

int CTcpIOCPClientStub::InitClient(SOCKET sock, SOCKADDR* paddr)
{
	m_Socket = sock;
	m_PeerInfo = *paddr;
	m_nStatus = 1;

	return 0;
}

unsigned int CTcpIOCPClientStub::GetID()
{
	return m_Socket;
}

int CTcpIOCPClientStub::RecvData(int seq_number, unsigned char* pdatabuf, int buflength)
{
	IO_OPER_DATA *pdata = CTcpIOCPTool::GetIOOperData();
	if (pdata == NULL)
		return -1;
	memset((void*)pdata, 0, sizeof(IO_OPER_DATA));
	pdata->oper_type = 0;
	pdata->starttime = GetTickCount();
	pdata->overtime = -1;
	pdata->dataBuf.buf = (char*)pdatabuf;
	pdata->dataBuf.len = buflength;
    DWORD flag = 0;
	int ret = WSARecv(m_Socket, &pdata->dataBuf, 1, NULL, &flag, &pdata->iooverlap, NULL);
    if (ret == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING)
        {
			CTcpIOCPTool::ReleaseIOOperData(pdata);
			return -1 * err;
        }
    }
	while (!TryEnterCriticalSection(&m_RecvSection))
		Sleep(1);
	m_RecvList.insert(pair<IO_OPER_DATA*, int>(pdata,1));
	LeaveCriticalSection(&m_RecvSection);
	return 0;
}

int CTcpIOCPClientStub::SendData(int seq_number, unsigned char* pdatabuf, int datalength)
{
	IO_OPER_DATA *pdata = CTcpIOCPTool::GetIOOperData();
	if (pdata == NULL)
		return -1;
	memset((void*)pdata, 0, sizeof(IO_OPER_DATA));
	pdata->oper_type = 1;
	pdata->starttime = GetTickCount();
	pdata->overtime = -1;
	pdata->dataBuf.buf = (char*)pdatabuf;
	pdata->dataBuf.len = datalength;
	DWORD flag = 0;
	int ret = WSASend(m_Socket, &pdata->dataBuf, 1, NULL, flag, &pdata->iooverlap, NULL);
	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			CTcpIOCPTool::ReleaseIOOperData(pdata);
			return -1 * err;
		}
	}
	while (!TryEnterCriticalSection(&m_SendSection))
		Sleep(1);
	m_SendList.insert(pair<IO_OPER_DATA*, int>(pdata, 1));
	LeaveCriticalSection(&m_SendSection);
	return 0;
}

int CTcpIOCPClientStub::ReleaseOperData(IO_OPER_DATA *poperdata)
{
	if (m_nStatus == -1)
		OutputDebugString(_T("Error client status"));
	if (poperdata->oper_type == 0)
	{
		poperdata->oper_type = -1;
		while (!TryEnterCriticalSection(&m_RecvSection))
			Sleep(1);
		int num = m_RecvList.erase(poperdata);
		LeaveCriticalSection(&m_RecvSection);
		if (num > 0)
			CTcpIOCPTool::ReleaseIOOperData(poperdata);
	}
	else if (poperdata->oper_type == 1)
	{
		poperdata->oper_type = -1;
		while (!TryEnterCriticalSection(&m_SendSection))
			Sleep(1);
		int num = m_SendList.erase(poperdata);
		LeaveCriticalSection(&m_SendSection);
		if (num > 0)
			CTcpIOCPTool::ReleaseIOOperData(poperdata);
	} 
	else
	{
		while (!TryEnterCriticalSection(&m_RecvSection))
			Sleep(1);
		auto iter = m_RecvList.find(poperdata);
		if (iter != m_RecvList.end())
		{
			m_RecvList.erase(iter);
			poperdata->oper_type = -1;
			CTcpIOCPTool::ReleaseIOOperData(poperdata);
			LeaveCriticalSection(&m_RecvSection);
			return 0;
		}
		LeaveCriticalSection(&m_RecvSection);

		while (!TryEnterCriticalSection(&m_SendSection))
			Sleep(1);
		auto iter1 = m_SendList.find(poperdata);
		if (iter1 != m_SendList.end())
		{
			m_SendList.erase(iter1);
			poperdata->oper_type = -1;
			CTcpIOCPTool::ReleaseIOOperData(poperdata);
			LeaveCriticalSection(&m_SendSection);
			return 0;
		}
		LeaveCriticalSection(&m_SendSection);

		ASSERT(1);

		OutputDebugString(_T("CTcpIOCPClientStub::ReleaseOperData Error oper_type"));
	}
	return 0;
}

int CTcpIOCPClientStub::CloseClient()
{
	while (!TryEnterCriticalSection(&m_CloseSocketSection))
		Sleep(1);
	if (m_Socket != INVALID_SOCKET)
	{
		//CancelIoEx((HANDLE)m_Socket,NULL);
		//shutdown(m_Socket,SD_BOTH);
		closesocket(m_Socket);
		m_nClosednum++;
		m_Socket = INVALID_SOCKET;
	}
	else
	{
		OutputDebugString(_T("Error"));
	}
	LeaveCriticalSection(&m_CloseSocketSection);

	while (!TryEnterCriticalSection(&m_RecvSection))
		Sleep(1);
	for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
	{
		(*iter).first->oper_type = -1;
		CTcpIOCPTool::ReleaseIOOperData((*iter).first);
	}
	m_RecvList.clear();
	LeaveCriticalSection(&m_RecvSection);

	while (!TryEnterCriticalSection(&m_SendSection))
		Sleep(1);
	for (auto iter = m_SendList.begin(); iter != m_SendList.end(); iter++)
	{
		(*iter).first->oper_type = -1;
		CTcpIOCPTool::ReleaseIOOperData((*iter).first);
	}
	m_SendList.clear();
	LeaveCriticalSection(&m_SendSection);

	m_nStatus = -1;

	return 0;
}

unsigned long CTcpIOCPClientStub::GetLastTime()
{
	return lastrecv;
}
