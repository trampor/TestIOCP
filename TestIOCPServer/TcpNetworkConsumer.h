#pragma once
#include "TcpIOCPClient.h"

class CTcpNetworkConsumer
{
public:
	CTcpNetworkConsumer();
	virtual ~CTcpNetworkConsumer();

	//0 �������� 1 �ж�����
	virtual int ClientStatusNotify(CTcpIOCPClient* pclient, int status_code) = 0;

	virtual int OperateNotify(int opertype, CTcpIOCPClient* pclient, unsigned long seq_number, int ret_code, unsigned char* pdatabuf, int datalength) = 0;
};

