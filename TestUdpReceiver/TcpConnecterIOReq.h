#pragma once
#include "IOReq.h"
class CTcpConnecterIOReq : public CIOReq
{
public:
	CTcpConnecterIOReq();
	virtual ~CTcpConnecterIOReq();

	unsigned char* m_pDataBuf;
	int m_nBufSize;
	int m_nOperateType;
};

