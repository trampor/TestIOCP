#pragma once

#include "IOReq.h"

class CUdpReceiverIOReq : public CIOReq
{
public:
	CUdpReceiverIOReq(CReqObserver *pobserver);
	~CUdpReceiverIOReq();

	unsigned char* m_pDataBuf;
	int m_nBufSize;
};

