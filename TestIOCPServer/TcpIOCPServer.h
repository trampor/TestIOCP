#pragma once
#include "TcpIOCPClient.h"
#include "TcpNetworkConsumer.h"

class CTcpIOCPServer
{
public:
	CTcpIOCPServer();
	virtual ~CTcpIOCPServer();

	static CTcpIOCPServer* CreateServer(LPCTSTR ip, unsigned short port, int threadnums, CTcpNetworkConsumer* pconsumer);

	virtual int StartService() = 0;

	virtual int StopService() = 0;

	virtual int CloseClient(CTcpIOCPClient *pclient) = 0;

	virtual int GetClientNum() = 0;
};

