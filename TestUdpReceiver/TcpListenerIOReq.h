#pragma once
#include "IOReq.h"

class CTcpListenerIOReq : public CIOReq
{
public:
	CTcpListenerIOReq(CReqObserver *pobserver);
	virtual ~CTcpListenerIOReq();

	SOCKET m_Socket;
	unsigned char* m_pDataBuf;
	int m_nBufSize;
};

