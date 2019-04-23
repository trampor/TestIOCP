#include "stdafx.h"
#include "IOReq.h"


CIOReq::CIOReq(CReqObserver *pobserver)
{
	Internal = InternalHigh = 0;
	Offset = OffsetHigh = 0;
	hEvent = NULL;
	m_pObserver = pobserver;
}

CIOReq::~CIOReq()
{
}

int CIOReq::ReportResult(int bytenumber, int ck, unsigned long error)
{
	m_pObserver->ConsumeReq(bytenumber,  ck,  error , this);
	return 0;
}
