#pragma once
#include "tcpworkerIOReq.h"
#include "iocompletionportbase.h"
#include "NoLockBiList.h"
#include "tcplistener.h"

class CTcpWorker : public CReqObserver
{
public:
	CTcpWorker();
	virtual ~CTcpWorker();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	virtual int InitObject(unsigned int serverip, int serverport, unsigned int clientip, int clientport, SOCKET& insock, CTcpListener<CTcpWorker>* plisten);
	virtual int UninitObject();

	virtual int Start();
	virtual int Stop();

	static int ReleaseAllList();

	virtual int ReadedDataProcess(unsigned char* pdatabuf, int datasize);
private:
	int Write(unsigned char* pdatabuf,int datsize);
	int Read();
	bool IsOver();

private:
	CTcpListener<CTcpWorker>* m_pListener;

	SOCKET m_TcpSocket;
	unsigned int m_nServerIp;
	int m_nServerPort;
	unsigned int m_nClientIp;
	int m_nClientPort;

	unsigned long m_unRecvedNumber, m_nFlag;
	WSABUF m_WSABUF;
	long m_nPacketNum;
	int m_nStatus,res;

	volatile unsigned long m_nRecvBytesNumOld, m_nRecvBytesNum;

	long m_nRecvNum, m_nRecvedNum, m_nErrorNum;
	long m_nWriteNum, m_nWritedNum;

	static CNoLockBiList<unsigned char*> m_BufList;
	static CNoLockBiList<CTcpWorkerIOReq*> m_IOReqList;
};

