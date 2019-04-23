#pragma once


class CBufSource
{
public:
	virtual int ReturnBuf(unsigned char* pbuf) = 0;
};

class CDataObserver
{
public:
	virtual int ImportData(unsigned char* pdatabuf, int bufsize, int datasize, CBufSource* pbufsource=NULL) = 0;
};
