#pragma once

#include "IOReq.h"

class CUdpRecvIOReq : public CIOReq
{
public:
	CUdpRecvIOReq();
	CUdpRecvIOReq(unsigned char* pdatabuf, int datasize, CReqObserver *pobserver);
	~CUdpRecvIOReq();

	unsigned char* m_pDataBuf;
	int m_nBufSize;
};

