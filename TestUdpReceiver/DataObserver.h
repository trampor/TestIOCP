#pragma once
#include "bufsource.h"

class CDataObserver
{
public:
	virtual int ImportData(unsigned char* pdatabuf, int bufsize, int datasize, CBufSource* pbufsource=NULL) = 0;
};
