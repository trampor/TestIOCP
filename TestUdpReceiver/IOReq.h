#pragma once

class CReqObserver
{
public:
	virtual int ConsumeReq(int bytenumber, int ck,unsigned long error,OVERLAPPED *povlp) = 0;
};

class CIOReq : public OVERLAPPED
{
public:
	CIOReq(CReqObserver *pobserver);
	~CIOReq();

	int ReportResult(int bytenumber, int ck, unsigned long error=0);

	CReqObserver *m_pObserver;
};

