#pragma once
#include "FileDataConsumer.h"

class CFileIOCPReader
{
public:
	CFileIOCPReader();
	virtual ~CFileIOCPReader();

	int Init(int threadnums, CFileDataConsumer* pconsumer);

	int OpenFile(LPCTSTR assetinfo, LPCTSTR filepath, int file_block_size); //打开文件准备读取,根据块大小对文件进行逻辑切分
	int LocateFile(LPCTSTR assetinfo,int type,int pos); //根据时间或者字节位置返回block_index
	int ReadFileBlock(LPCTSTR assetinfo, int block_index, unsigned char* pbuf, int buf_size);
	int ReadFile(LPCTSTR assetinfo, __int64 startpos, unsigned char* pbuf, int buf_size);
	int CloseFile(LPCTSTR assetinfo);
};

