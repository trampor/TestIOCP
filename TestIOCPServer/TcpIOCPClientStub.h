#pragma once
#include "TcpIOCPClient.h"
#include <map>
using namespace std;
#include "commparam.h"

class CTcpIOCPClientStub :public CTcpIOCPClient
{
public:
	CTcpIOCPClientStub();
	virtual ~CTcpIOCPClientStub();

	int InitClient(SOCKET sock, SOCKADDR* paddr);

	unsigned int GetID();
	int RecvData(int seq_number, unsigned char* pdatabuf, int buflength);
	int SendData(int seq_number, unsigned char* pdata, int datalength);
	unsigned long GetLastTime();

	int ReleaseOperData(IO_OPER_DATA *poperdata);
	int CloseClient();

public:
	SOCKET m_Socket;
	SOCKADDR m_PeerInfo;
	unsigned long lastrecv;

private:
	map<IO_OPER_DATA*,int> m_RecvList;
	CRITICAL_SECTION m_RecvSection;
	map<IO_OPER_DATA*,int> m_SendList;
	CRITICAL_SECTION m_SendSection;
	CRITICAL_SECTION m_CloseSocketSection;
	int m_nStatus;
	static int m_nClosednum;
};

