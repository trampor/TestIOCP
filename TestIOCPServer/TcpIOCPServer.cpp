#include "stdafx.h"
#include "TcpIOCPServer.h"
#include "TcpIOCPServerImp.h"

CTcpIOCPServer::CTcpIOCPServer()
{
}


CTcpIOCPServer::~CTcpIOCPServer()
{
}

CTcpIOCPServer* CTcpIOCPServer::CreateServer(LPCTSTR ip, unsigned short port, int threadnums, CTcpNetworkConsumer* pconsumer)
{
	CTcpIOCPServerImp *pserver = new CTcpIOCPServerImp;
	int ret = pserver->InitServer(ip, port, threadnums, pconsumer);
	if (ret < 0)
		return NULL;
	else
		return pserver;
}
