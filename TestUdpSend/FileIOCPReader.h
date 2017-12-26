#pragma once
#include "FileDataConsumer.h"

class CFileIOCPReader
{
public:
	CFileIOCPReader();
	virtual ~CFileIOCPReader();

	int Init(int threadnums, CFileDataConsumer* pconsumer);

	int OpenFile(LPCTSTR assetinfo, LPCTSTR filepath, int file_block_size); //���ļ�׼����ȡ,���ݿ��С���ļ������߼��з�
	int LocateFile(LPCTSTR assetinfo,int type,int pos); //����ʱ������ֽ�λ�÷���block_index
	int ReadFileBlock(LPCTSTR assetinfo, int block_index, unsigned char* pbuf, int buf_size);
	int ReadFile(LPCTSTR assetinfo, __int64 startpos, unsigned char* pbuf, int buf_size);
	int CloseFile(LPCTSTR assetinfo);
};

