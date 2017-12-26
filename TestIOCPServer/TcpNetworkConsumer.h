#pragma once
#include "TcpIOCPClient.h"

class CTcpNetworkConsumer
{
public:
	CTcpNetworkConsumer();
	virtual ~CTcpNetworkConsumer();

	//0 建立连接 1 中断连接
	virtual int ClientStatusNotify(CTcpIOCPClient* pclient, int status_code) = 0;

	virtual int OperateNotify(int opertype, CTcpIOCPClient* pclient, unsigned long seq_number, int ret_code, unsigned char* pdatabuf, int datalength) = 0;
};

