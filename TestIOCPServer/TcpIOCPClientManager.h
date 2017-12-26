#pragma once
#include "TcpIOCPClientStub.h"

class CTcpIOCPClientManager
{
public:
	CTcpIOCPClientManager();
	~CTcpIOCPClientManager();

	int InitManager(int maxconnectnums);

	CTcpIOCPClientStub* GetFreeClient();
	int CloseAllClient();
	int AddRealClient(CTcpIOCPClientStub* pclient);
	int RemoveRealClient(CTcpIOCPClientStub* pclient);
	int GetClientNum();

private:
	list<CTcpIOCPClientStub*> m_BigClientList;

	list<CTcpIOCPClientStub*> m_FreeClientList;

	map<CTcpIOCPClientStub*, int> m_RealClientList;

	CRITICAL_SECTION m_FreeListSection, m_RealListSection;
};