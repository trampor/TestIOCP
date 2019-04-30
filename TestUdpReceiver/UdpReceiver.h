#pragma once

#include "UdpReceiverIOReq.h"
#include "dataobserver.h"
#include <list>
#include <string>
#include "iocompletionportbase.h"
#include "NoLockBiList.h"
using namespace std;

class CUdpReceiver : public CReqObserver, public CBufSource
{
public:
	CUdpReceiver();
	~CUdpReceiver();

	int InitObject(LPCTSTR plocalip, LPCTSTR psourceip, LPCTSTR pmultiip, int port);
	int UninitObject();

	int Start(CIOCompletionPortBase* piocompletionport);
	int Stop();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	int ReturnBuf(unsigned char* pbuf);

	int AddDataObserver(CDataObserver* pdataobserver);

	int RemoveDataObserver(CDataObserver* pdataobserver);

	HANDLE GetFileHandle();

	int GetStatus();

	int GetPacketNum();

	unsigned long GetRecvBytesNum();

	static int ReleaseAllList();

	virtual int ReadedDataProcess(unsigned char* pdatabuf,int datasize);

private:
	SOCKET MakeMultiSocket(LPCTSTR plocalip, LPCTSTR psourceip, LPCTSTR pmultiip, int port);
	int Read();

private:
	SOCKET m_UdpSocket;
	unsigned long m_unRecvedNumber,m_nFlag;
	WSABUF m_WSABUF;
	long m_nPacketNum;
	int m_nStatus;
	wstring m_sLocalIp, m_sSourceIp, m_sMulticastIp;
	int m_nMulticastPort;
	volatile unsigned long m_nRecvBytesNumOld, m_nRecvBytesNum;
	list<CDataObserver*> m_DataObserverList;

	long m_nRecvNum, m_nRecvedNum;

	static CNoLockBiList<unsigned char*> m_BufList;
	static CNoLockBiList<CUdpReceiverIOReq*> m_IOReqList;

	SRWLOCK m_ObserverLock;
};

