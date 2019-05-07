#pragma once
#include "tcpworkerIOReq.h"
#include "iocompletionportbase.h"
#include "NoLockBiList.h"
#include "ObjectAllocater.h"

class CTcpWorker : public CReqObserver
{
public:
	CTcpWorker();
	virtual ~CTcpWorker();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	virtual int InitObject(unsigned int serverip, int serverport, unsigned int clientip, int clientport, SOCKET& insock, CObjectAllocater* pallocater);
	virtual int UninitObject();

	virtual int Start();
	virtual int Stop();

	static int ReleaseAllList();

protected:
	virtual int ReadedDataProcess(unsigned char* pdatabuf, int datasize);
	int Write(unsigned char* pdatabuf,int datsize);

private:
	int Read();
	bool IsOver();

private:
	CObjectAllocater* m_pListener;

	SOCKET m_TcpSocket;
	unsigned int m_nServerIp;
	int m_nServerPort;
	unsigned int m_nClientIp;
	int m_nClientPort;

	unsigned long m_unRecvedNumber, m_nFlag;
	WSABUF m_RWSABUF, m_WWSABUF;
	long m_nPacketNum;
	int m_nStatus,res;

	volatile unsigned long m_nRecvBytesNumOld, m_nRecvBytesNum;

	long m_nRecvNum, m_nRecvedNum, m_nErrorNum;
	long m_nWriteNum, m_nWritedNum;

	static CNoLockBiList<CTcpWorkerIOReq*> m_IOReqList;

protected:
	static CNoLockBiList<unsigned char*> m_BufList;
};

