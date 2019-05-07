#pragma once
#include "NoLockBiList.h"
#include "TcpListenerIOReq.h"
#include "iocompletionportbase.h"
#include <string>
#include "ObjectAllocater.h"
using namespace std;

class CTcpListenerBase :public CReqObserver
{
public:
	CTcpListenerBase();

	virtual ~CTcpListenerBase();

	virtual int InitObject(LPCTSTR plocalip, int port);
	virtual int UninitObject();

	int Start(CIOCompletionPortBase* piocompletionport);
	int Stop();

	static int ReleaseAllList();

	int GetPacketNum();

	int GetStatus();

private:
	SOCKET MakeTcpServerSocket(LPCTSTR plocalip, int port);

protected:
	int Accept();

protected:
	static CNoLockBiList<unsigned char*> m_BufList;
	static CNoLockBiList<SOCKET> m_SocketList;
	static CNoLockBiList<CTcpListenerIOReq*> m_IOReqList;
	long m_nAcceptNum;
	SOCKET m_TcpSocket;
	LPFN_ACCEPTEX lpfnAcceptEx ;	//AcceptEx函数指针
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;  //加载GetAcceptExSockaddrs函数指针

private:
	wstring m_sLocalIp;
	int m_nPort;
	int m_nStatus;

};

