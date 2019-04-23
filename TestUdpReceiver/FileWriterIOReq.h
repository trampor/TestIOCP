#pragma once
#include "IOReq.h"
#include "dataobserver.h"

class CFileWriterIOReq : public CIOReq
{
public:
	CFileWriterIOReq(unsigned char* pdatabuf, int bufsize, int datasize, CReqObserver *pobserver);
	~CFileWriterIOReq();

	unsigned char* m_pDataBuf;
	int m_nBufSize;
	int m_nDataSize;
};

