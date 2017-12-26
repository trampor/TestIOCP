#pragma once

class CTcpIOCPClient
{
public:
	CTcpIOCPClient();
	virtual ~CTcpIOCPClient();

	virtual unsigned int GetID() = 0;
	virtual unsigned long GetLastTime() = 0;
	virtual int RecvData(int seq_number,unsigned char* pdatabuf, int buflength) = 0;
	virtual int SendData(int seq_number, unsigned char* pdatabuf, int datalength) = 0;

};

