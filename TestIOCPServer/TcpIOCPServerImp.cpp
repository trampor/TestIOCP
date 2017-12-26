#include "stdafx.h"
#include "TcpIOCPServerImp.h"
#include "TcpIOCPClientStub.h"
#include "commparam.h"

CTcpIOCPServerImp::CTcpIOCPServerImp()
{
	m_ListenSocket = INVALID_SOCKET;
	m_hEvent = WSA_INVALID_EVENT;
	m_bAcceptRunning = false;
	m_bWorkRunning = false;
	m_hIOCP = NULL;
	m_pThreadHandle = NULL;
	m_pThreadData = NULL;

	errclosenum = 0;
	lengthclosenum = 0;
}

CTcpIOCPServerImp::~CTcpIOCPServerImp()
{
	StopService();

	CTcpIOCPTool::UninitObj();
}

int CTcpIOCPServerImp::InitServer(LPCTSTR plocalip, unsigned short port, int threadnums, CTcpNetworkConsumer* pconsumer)
{
	if (plocalip == NULL)
		return -1;
	if (wcslen(plocalip) > 15)
		return -2;
	if (pconsumer == NULL)
		return -3;
	memset(m_sServerIp, 0, 20 * sizeof(TCHAR));
	wcscpy_s(m_sServerIp,20, plocalip);
	m_nServerPort = port;
	m_nThreadNums = threadnums;
	if (m_nThreadNums <= 0)
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		m_nThreadNums = sysinfo.dwNumberOfProcessors;
	}
	m_pConsumer = pconsumer;

	m_ClientManager.InitManager(1000);

	CTcpIOCPTool::InitObj(1000);

	return 0;
}

int CTcpIOCPServerImp::InitSocket()
{
	WSAData wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return -1;
	}

	m_hEvent = WSACreateEvent();
	if (m_hEvent == WSA_INVALID_EVENT)
	{
		closesocket(m_ListenSocket);
		WSACleanup();
		m_ListenSocket = INVALID_SOCKET;
		return -2;
	}

	//为监听套接字设置FD_ACCEPT事件
	int ret = WSAEventSelect(m_ListenSocket, m_hEvent, FD_ACCEPT);
	if (ret == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		WSACloseEvent(m_hEvent);
		WSACleanup();
		m_ListenSocket = INVALID_SOCKET;
		m_hEvent = WSA_INVALID_EVENT;
		return -3;
	}
	
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	char localip[20];
	memset(localip, 0, 20);
	WideCharToMultiByte(CP_ACP, 0, m_sServerIp, -1, localip, 20, NULL, NULL);
	addr.sin_addr.S_un.S_addr = inet_addr(localip);
	addr.sin_port = htons(m_nServerPort);
	ret = bind(m_ListenSocket, (SOCKADDR*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		WSACloseEvent(m_hEvent);
		WSACleanup();
		m_ListenSocket = INVALID_SOCKET;
		m_hEvent = WSA_INVALID_EVENT;
		return -4;
	}
	ret = listen(m_ListenSocket, 1000);
	if (ret == SOCKET_ERROR)
	{
		closesocket(m_ListenSocket);
		WSACloseEvent(m_hEvent);
		WSACleanup();
		m_ListenSocket = INVALID_SOCKET;
		m_hEvent = WSA_INVALID_EVENT;
		return -5;
	}
	return 0;
}

int CTcpIOCPServerImp::StartService()
{
	if (m_bAcceptRunning)
		return 0;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_nThreadNums);
	if (m_hIOCP == NULL)
		return -1;

	int ret = InitSocket();
	if (ret < 0)
	{
		CloseHandle(m_hIOCP);
		return -2;
	}

	m_pThreadHandle = new HANDLE[m_nThreadNums + 1];
	m_pThreadData = new unsigned int[m_nThreadNums + 1];

	m_bAcceptRunning = true;
	m_pThreadHandle[0] = (HANDLE)_beginthreadex(NULL, 0, InitAcceptThread, this, 0, &m_pThreadData[0]);
	if (m_pThreadHandle[0] == 0)
		return -1;
	m_bWorkRunning = true;
	for (int i = 1; i<=m_nThreadNums; i++)
	{
		m_pThreadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, InitWorkThread, this, 0, &m_pThreadData[i]);
		if (m_pThreadHandle[0] == 0)
			return -1*(i+1);
	}

	return 0;
}

int CTcpIOCPServerImp::StopService()
{
	if (m_bAcceptRunning)
	{
		m_bAcceptRunning = false;
		if (m_pThreadHandle[0] != 0)
		{
			WaitForSingleObject(m_pThreadHandle[0], INFINITE);
			CloseHandle(m_pThreadHandle[0]);
		}
	}
	if (m_bWorkRunning)
	{
		for (int i = 0; i < m_nThreadNums; i++)
		{
			IO_OPER_DATA* pdata = CTcpIOCPTool::GetIOOperData();
			pdata->oper_type = 100;
			PostQueuedCompletionStatus(m_hIOCP, 0, NULL, &pdata->iooverlap);
		}
		WaitForMultipleObjects(m_nThreadNums, m_pThreadHandle + 1, true, INFINITE);
		for (int i = 0; i < m_nThreadNums; i++)
		{
			CloseHandle(m_pThreadHandle[1+i]);
		}
		m_bWorkRunning = false;
	}

	if (m_pThreadHandle != NULL)
	{
		delete m_pThreadHandle;
		m_pThreadHandle = NULL;
	}

	if (m_pThreadData != NULL)
	{
		delete m_pThreadData;
		m_pThreadData = NULL;
	}

	m_ClientManager.CloseAllClient();

	if (m_hIOCP != NULL)
	{
		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
	}

	if (m_ListenSocket != INVALID_SOCKET)
	{
		closesocket(m_ListenSocket);
		m_ListenSocket = INVALID_SOCKET;
	}
	return 0;
}

int CTcpIOCPServerImp::CloseClient(CTcpIOCPClient *pclient)
{

	m_ClientManager.RemoveRealClient((CTcpIOCPClientStub*)pclient);

	m_pConsumer->ClientStatusNotify(pclient, 1);

	return 0;
}

int CTcpIOCPServerImp::GetClientNum()
{
	return m_ClientManager.GetClientNum();
}

unsigned WINAPI CTcpIOCPServerImp::InitAcceptThread(LPVOID pv)
{
	CTcpIOCPServerImp* lpInst = (CTcpIOCPServerImp*)pv;
	return lpInst->AcceptThread();
}

unsigned CTcpIOCPServerImp::AcceptThread()
{
	int ret;
	SOCKET sAccept;
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	WSANETWORKEVENTS events;
	CTcpIOCPClientStub *pclient;
	while (m_bAcceptRunning)
	{
		ret = WSAWaitForMultipleEvents(1, &m_hEvent, false, 50, false);
		if (ret == WSA_WAIT_TIMEOUT)
			continue;
		ret = WSAEnumNetworkEvents(m_ListenSocket, m_hEvent, &events);//重置事件对象
		if (ret == SOCKET_ERROR) //需要通知上层应用
		{
			break;
		}
		if (events.lNetworkEvents&FD_ACCEPT)
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0)//发生FD_ACCEPT网络事件。接受客户端请求
			{
				sAccept = WSAAccept(m_ListenSocket, (SOCKADDR*)&addr, &addrlen, 0, NULL);
				pclient = m_ClientManager.GetFreeClient();
				if (pclient == NULL)
				{
					closesocket(sAccept);
					continue;
				}
				pclient->InitClient(sAccept, (SOCKADDR*)&addr);
				m_ClientManager.AddRealClient(pclient);
				if (CreateIoCompletionPort((HANDLE)sAccept, m_hIOCP, (ULONG_PTR)pclient, m_nThreadNums) == NULL)
				{
					m_ClientManager.RemoveRealClient(pclient);
					break;
				}
				m_pConsumer->ClientStatusNotify(pclient, 0);
			}
		}
	}
	return 0;
}

unsigned WINAPI CTcpIOCPServerImp::InitWorkThread(LPVOID pv)
{
	CTcpIOCPServerImp* lpInst = (CTcpIOCPServerImp*)pv;
	return lpInst->WorkThread();
}

unsigned CTcpIOCPServerImp::WorkThread()
{
	LPOVERLAPPED lpoverlapped;
	CTcpIOCPClientStub *pclient;
	DWORD transferred,lasterr;
	BOOL bret;
	int oper_type;
	unsigned long seq_number;
	unsigned char* pdatabuf;
	while (m_bWorkRunning)
	{
		pclient = NULL;
		lpoverlapped = NULL;
		bret = GetQueuedCompletionStatus(m_hIOCP, &transferred, (PULONG_PTR)&pclient, &lpoverlapped, 50);
		if (!bret)
		{
			lasterr = GetLastError();
			if (lasterr == WAIT_TIMEOUT)
				continue;
			else if (lasterr == ERROR_ABANDONED_WAIT_0) //iocp出错，退出线程
			{
				CString tempstr;
				tempstr.Format(_T("GetQueuedCompletionStatus return fail,exit thread,errorcode=%u"),lasterr);
				OutputDebugString(tempstr);
				break;
			}

			IO_OPER_DATA* pio = (IO_OPER_DATA*)lpoverlapped;
			if (pio != NULL) //操作失败，通知上层
			{
				oper_type = pio->oper_type;
				seq_number = pio->seq_number;
				pdatabuf = (unsigned char*)pio->dataBuf.buf;
				pclient->ReleaseOperData(pio);
				m_pConsumer->OperateNotify(oper_type, pclient, seq_number, lasterr, pdatabuf, transferred);
				errclosenum++;
			}
			else //iocp出错，退出线程
			{
				CString tempstr;
				tempstr.Format(_T("GetQueuedCompletionStatus return fail,exit thread,errorcode=%u"), lasterr);
				OutputDebugString(tempstr);
				break;
			}
		}
		else if ((lpoverlapped != NULL) && (pclient != NULL))//成功的异步IO完成。根据从lpoverlapped中得到的类型，进行操作
		{
			IO_OPER_DATA* pio = (IO_OPER_DATA*)lpoverlapped;
			switch (pio->oper_type)
			{
			case 0: //读IO结果
			case 1: //写IO结果
				oper_type = pio->oper_type;
				seq_number = pio->seq_number;
				pio->overtime = GetTickCount();
				pdatabuf = (unsigned char*)pio->dataBuf.buf;
				pclient->ReleaseOperData(pio);
				pclient->lastrecv = GetTickCount();
				m_pConsumer->OperateNotify(oper_type, pclient, seq_number, 0, pdatabuf, transferred);
				if (transferred <= 0)
					lengthclosenum++;

				break;
			case 100: //退出线程
			{
				CString tempstr;
				tempstr.Format(_T("Recv exit notify,exiting thread 0x%x......"), GetCurrentThreadId());
				OutputDebugString(tempstr);
				CTcpIOCPTool::ReleaseIOOperData(pio);
				return 0x11111111;
			}
			default:
				break;
			}
		}
		else if (lpoverlapped != NULL)//成功的异步IO完成。根据从lpoverlapped中得到的类型，进行操作
		{
			IO_OPER_DATA* pio = (IO_OPER_DATA*)lpoverlapped;
			if (pio->oper_type == 100)
			{
				CString tempstr;
				tempstr.Format(_T("Recv exit notify,exiting thread 0x%x......"), GetCurrentThreadId());
				OutputDebugString(tempstr);
				CTcpIOCPTool::ReleaseIOOperData(pio);
				return 0x11111111;
			}
			CTcpIOCPTool::ReleaseIOOperData(pio);
		}
	}
	return 0;
}