#pragma once
#include "WinSock2.h"
#include <list>
using namespace std;

struct IO_OPER_DATA
{
	WSAOVERLAPPED iooverlap;

	WSABUF dataBuf;

	int seq_number;

	unsigned long starttime, overtime;

	int oper_type; //0 read 1 write 100 killthread
};

class CTcpIOCPTool
{
public:
	static int InitObj(int maxconnectnums);
	static int UninitObj();
	static IO_OPER_DATA* GetIOOperData();
	static int ReleaseIOOperData(IO_OPER_DATA* pdata);

private:
	static bool m_bInited;
	static list<IO_OPER_DATA*> m_BigList;
	static list<IO_OPER_DATA*> m_FreeList;
	static CRITICAL_SECTION m_FreeSection;
};