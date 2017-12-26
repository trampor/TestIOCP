#pragma once

#include "TcpIOCPServer.h"
#include "TcpIOCPClientManager.h"
#include "TcpNetworkConsumer.h"

class CTcpIOCPServerImp : public CTcpIOCPServer
{
public:
	CTcpIOCPServerImp();
	~CTcpIOCPServerImp();

	int InitServer(LPCTSTR ip, unsigned short port, int threadnums, CTcpNetworkConsumer* pconsumer);
	virtual int StartService() ;
	virtual int StopService();
	virtual int CloseClient(CTcpIOCPClient *pclient);
	virtual int GetClientNum();

private:
	int InitSocket();
	static unsigned WINAPI InitAcceptThread(LPVOID pv);
	unsigned AcceptThread();
	static unsigned WINAPI InitWorkThread(LPVOID pv);
	unsigned WorkThread();

private:
	SOCKET m_ListenSocket;
	TCHAR m_sServerIp[20];
	unsigned short m_nServerPort;
	int m_nThreadNums;

	HANDLE m_hIOCP; //完成端口句柄
	HANDLE m_hEvent;//监听套接字对应事件对象

	bool m_bAcceptRunning, m_bWorkRunning;

	HANDLE *m_pThreadHandle;
	unsigned int *m_pThreadData;

	CTcpIOCPClientManager m_ClientManager;
	CTcpNetworkConsumer* m_pConsumer;

	int errclosenum, lengthclosenum;
};

