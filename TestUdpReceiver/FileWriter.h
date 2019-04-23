#pragma once
#include "FileWriterIOReq.h"
#include "dataobserver.h"
#include "NoLockBiList.h"

#define WFILEBUFSIZE 640*1024

class CFileWriter : public CReqObserver, public CDataObserver
{
public:
	CFileWriter();
	~CFileWriter();

	int InitObject(LPCTSTR pfilepath);
	int UninitObject();

	int ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp);

	int ImportData(unsigned char* pdatabuf, int bufsize, int datasize, CBufSource* pbufsource = NULL);

	HANDLE GetFileHandle();

	int GetRecvPacketNum();

	int GetWritePacketNum();

	static int ReleaseAllList();

private:
	HANDLE m_hFileDest;
	unsigned long m_nRecvPackets,m_nWritedPackets;
	unsigned long m_unWriteNumber;

	unsigned char* m_pCurBuf;
	long m_nCurSize;

	long m_nWriteNum,m_nWritedNum;

	static CNoLockBiList<unsigned char*> m_BufList;
	static CNoLockBiList<CFileWriterIOReq*> m_IOReqList;

	//sync for import data
	//SRWLOCK m_SRWLock;
};

