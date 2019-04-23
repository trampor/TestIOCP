#include "stdafx.h"
#include "FileWriterIOReq.h"


CFileWriterIOReq::CFileWriterIOReq(unsigned char* pdatabuf, int bufsize, int datasize, CReqObserver *pobserver)
	:CIOReq(pobserver)
{
	m_pDataBuf = pdatabuf;
	m_nBufSize = bufsize;
	m_nDataSize = datasize;
}


CFileWriterIOReq::~CFileWriterIOReq()
{
}
