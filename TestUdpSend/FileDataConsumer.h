#pragma once

class CFileDataConsumer
{
public:
	CFileDataConsumer();
	virtual ~CFileDataConsumer();

	virtual int OperateNotify(int opertype, int ret_code, unsigned char* pdatabuf, int datalength) = 0;
};

