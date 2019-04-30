#include "stdafx.h"
#include "UdpReceiverIOReq.h"


CUdpReceiverIOReq::CUdpReceiverIOReq(CReqObserver *pobserver)
	:CIOReq(pobserver)
{
	m_pDataBuf = NULL;
	m_nBufSize = 0;
}

CUdpReceiverIOReq::~CUdpReceiverIOReq()
{
}
