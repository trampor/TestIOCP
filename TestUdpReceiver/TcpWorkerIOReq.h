#pragma once
#include "IOReq.h"

class CTcpWorkerIOReq : public CIOReq
{
public:
	CTcpWorkerIOReq(CReqObserver *pobserver);
	~CTcpWorkerIOReq();

	unsigned char* m_pDataBuf;
	int m_nBufSize;
	int m_nOperateType;
};

