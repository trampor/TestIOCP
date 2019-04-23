#include "stdafx.h"
#include "UdpRecvIOReq.h"


CUdpRecvIOReq::CUdpRecvIOReq()
	:CIOReq(NULL)
{
	m_pDataBuf = NULL;
	m_nBufSize = 0;
}

CUdpRecvIOReq::CUdpRecvIOReq(unsigned char* pdatabuf, int datasize, CReqObserver *pobserver)
	:CIOReq(pobserver)
{
	m_pDataBuf = pdatabuf;
	m_nBufSize = datasize;
}


CUdpRecvIOReq::~CUdpRecvIOReq()
{
}
