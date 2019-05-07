#pragma once
#include <string>
#include "IOReq.h"
#include "NoLockBiList.h"
#include "TcpConnecterIOReq.h"
#include "iocompletionportbase.h"
#include "ObjectAllocater.h"
using namespace std;

class CTcpConnecter : public CReqObserver
{
public:
	CTcpConnecter();
	virtual ~CTcpConnecter();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	virtual int InitObject(LPCTSTR pserverip, int serverport, CObjectAllocater* pallocater);
	virtual int UninitObject();

	virtual int Start();
	virtual int Stop();

	static int ReleaseAllList();

	virtual int ReadedDataProcess(unsigned char* pdatabuf, int datasize);

	bool IsConnect()
	{
		return m_bConnected;
	}
	unsigned long GetConnectTime()
	{
		return m_nConnectEnd-m_nConnectStart ;
	}
	int GetRecvWriteNum(int& nRecv, int& nRecved, int& write, int& writed, int& err)
	{
		nRecv = m_nRecvNum;
		nRecved = m_nRecvedNum;
		write = m_nWriteNum;
		writed = m_nWritedNum;
		err = m_nErrorNum;
		return 0;
	}
	LPCTSTR GetLocalAddr()
	{
		return m_sClientAddr.c_str();
	}

	int GetStatus()
	{
		return m_nStatus;
	}

	int Write(unsigned char* pdatabuf, int datsize);

private:
	SOCKET MakeTcpClientSocket(LPCTSTR pserverip, int serverport);

	int Connect();

	int Read();

	bool IsOver();

private:
	CObjectAllocater* m_pAllocater;

	bool m_bConnected;
	int m_nStatus;

	unsigned long m_nConnectStart, m_nConnectEnd;

	SOCKET m_TcpSocket;
	wstring m_sServerIp;
	int m_nServerPort;
	wstring m_sClientAddr;

	unsigned long m_unRecvedNumber, m_nFlag;
	WSABUF m_RWSABUF, m_WWSABUF;

	volatile unsigned long m_nRecvBytesNumOld, m_nRecvBytesNum;

	long m_nRecvNum, m_nRecvedNum, m_nErrorNum;
	long m_nWriteNum, m_nWritedNum, m_nPacketNum;

	static CNoLockBiList<unsigned char*> m_BufList;
	static CNoLockBiList<CTcpConnecterIOReq*> m_IOReqList;

	LPFN_CONNECTEX lpfnConnectEx ;	//ConnectEx函数指针
	LPFN_DISCONNECTEX lpfnDisConnectEx;//DisConnectEx函数指针
};

