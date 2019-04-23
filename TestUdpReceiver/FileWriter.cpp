#include "stdafx.h"
#include "FileWriter.h"

CNoLockBiList<unsigned char*> CFileWriter::m_BufList;
CNoLockBiList<CFileWriterIOReq*> CFileWriter::m_IOReqList;

CFileWriter::CFileWriter()
{
	m_hFileDest = INVALID_HANDLE_VALUE;
	m_nRecvPackets = 0;
	m_nWritedPackets = 0;

	m_pCurBuf = NULL;
	m_nCurSize = 0;

	m_nWriteNum = 0;
	m_nWritedNum = 0;;

	//InitializeSRWLock(&m_SRWLock);
}


CFileWriter::~CFileWriter()
{
}

int CFileWriter::ReleaseAllList()
{
	unsigned char* pDataBuf = (unsigned char*)m_BufList.PopHead();
	while (pDataBuf != NULL)
	{
		delete pDataBuf;
		pDataBuf = (unsigned char*)m_BufList.PopHead();
	}

	CFileWriterIOReq* pIOReq = (CFileWriterIOReq*)m_IOReqList.PopHead();
	while (pIOReq != NULL)
	{
		delete pIOReq;
		pIOReq = (CFileWriterIOReq*)m_IOReqList.PopHead();
	}
	return 0;
}

int CFileWriter::ConsumeReq(int bytenumber, int ck, unsigned long error, OVERLAPPED *povlp)
{
	m_nWritedNum++;

	if (error == 0)
		InterlockedExchangeAdd(&m_nWritedPackets, ((CFileWriterIOReq*)povlp)->m_nDataSize);
	else
		OutputDebugString(_T("CFileWriter::ConsumeReq iocomplete error"));


	if (povlp != NULL)
	{
		m_BufList.PushTail(((CFileWriterIOReq*)povlp)->m_pDataBuf);
		m_IOReqList.PushTail((CFileWriterIOReq*)povlp);
	}

	return 0;
}

int CFileWriter::InitObject(LPCTSTR pfilepath)
{
	if ((pfilepath == NULL) || (wcslen(pfilepath)==0))
		return -1;

	m_hFileDest = CreateFile(pfilepath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (m_hFileDest == INVALID_HANDLE_VALUE)
	{
		OutputDebugString(_T("CFileWriter::InitObject createfile fail"));
		return -2;
	}

	m_nRecvPackets = 0;
	m_nWritedPackets = 0;

	unsigned char* ptempbuf;
	for (int i = 0; i < 2; i++)
	{
		ptempbuf = new unsigned char[WFILEBUFSIZE];
		m_BufList.PushTail(ptempbuf);
		InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
	}

	CFileWriterIOReq* ptempioreq;
	for (int i = 0; i <1; i++)
	{
		ptempioreq = new CFileWriterIOReq(NULL,0, 0, this);
		m_IOReqList.PushTail(ptempioreq);
		InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
	}

	return 0;
}

int CFileWriter::UninitObject()
{
	if (m_hFileDest != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFileDest);
	m_hFileDest = INVALID_HANDLE_VALUE;
	m_nRecvPackets = 0;
	m_nWritedPackets = 0;
	return 0;
}

HANDLE CFileWriter::GetFileHandle()
{
	return m_hFileDest;
}

int CFileWriter::GetRecvPacketNum()
{
	return m_nRecvPackets;
}

int CFileWriter::GetWritePacketNum()
{
	return m_nWritedPackets;
}

int CFileWriter::ImportData(unsigned char* pdatabuf, int bufsize, int datasize, CBufSource* pbufsource)
{
	InterlockedExchangeAdd(&m_nRecvPackets, datasize);

	if (m_hFileDest == INVALID_HANDLE_VALUE)
	{
		if ((pbufsource != NULL) && (datasize!=0))
			pbufsource->ReturnBuf(pdatabuf);
		return -1;
	}

	//datasize==0 indicate the process is over ,flush the reserved data to file 
	if (datasize == 0 )
	{
		if (m_nCurSize > 0)
		{
			CFileWriterIOReq *pOverlap = (CFileWriterIOReq*)m_IOReqList.PopHead();
			if (pOverlap == NULL)
			{
				pOverlap = new CFileWriterIOReq(m_pCurBuf, WFILEBUFSIZE, m_nCurSize, this);
				if (pOverlap == NULL)
				{
					if (pbufsource != NULL)
						pbufsource->ReturnBuf(pdatabuf);
					OutputDebugString(_T("CFileWriter::ImportData alloc new ioreq fail"));
					//				ReleaseSRWLockExclusive(&m_SRWLock);
					return -3;
				}
				InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
			}
			else
			{
				pOverlap->m_pDataBuf = m_pCurBuf;
				pOverlap->m_nBufSize = WFILEBUFSIZE;
				pOverlap->m_nDataSize = m_nCurSize;
				pOverlap->m_pObserver = this;
			}
			pOverlap->Offset = 0xffffffff;
			pOverlap->OffsetHigh = 0xffffffff;

			BOOL bsuc = WriteFile(m_hFileDest, m_pCurBuf, m_nCurSize, NULL, pOverlap);
			if (!bsuc)
			{
				if (ERROR_IO_PENDING != GetLastError())
				{
					if (pbufsource != NULL)
						pbufsource->ReturnBuf(pdatabuf);
					m_IOReqList.PushTail(pOverlap);
					OutputDebugString(_T("CFileWriter::ImportData WriteFile Fail"));
				}
			}
			m_nWriteNum++;
		}
		m_pCurBuf = NULL;
		m_nCurSize = 0;
		return 0;
	}

//	AcquireSRWLockExclusive(&m_SRWLock);
	if (m_pCurBuf == NULL)
	{
		m_pCurBuf = (unsigned char*)m_BufList.PopHead();
		if (m_pCurBuf == NULL)
		{
			m_pCurBuf = new unsigned char[WFILEBUFSIZE];
			if (m_pCurBuf == NULL)
			{
				OutputDebugString(_T("CFileWriter::ImportData alloc new buf fail"));
				if (pbufsource != NULL)
					pbufsource->ReturnBuf(pdatabuf);
//				ReleaseSRWLockExclusive(&m_SRWLock);
				return -2;
			}
			InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
		}
		m_nCurSize = 0;
	}
	if (m_nCurSize + datasize > WFILEBUFSIZE)
	{
		CFileWriterIOReq *pOverlap = (CFileWriterIOReq*)m_IOReqList.PopHead();
		if (pOverlap == NULL)
		{
			pOverlap = new CFileWriterIOReq(m_pCurBuf, WFILEBUFSIZE, m_nCurSize, this);
			if (pOverlap == NULL)
			{
				if (pbufsource != NULL)
					pbufsource->ReturnBuf(pdatabuf);
				OutputDebugString(_T("CFileWriter::ImportData alloc new ioreq fail"));
//				ReleaseSRWLockExclusive(&m_SRWLock);
				return -3;
			}
			InterlockedExchangeAdd(&m_IOReqList.m_nAllocatedEleNum, 1);
		}
		else
		{
			pOverlap->m_pDataBuf = m_pCurBuf;
			pOverlap->m_nBufSize = WFILEBUFSIZE;
			pOverlap->m_nDataSize = m_nCurSize;
			pOverlap->m_pObserver = this;
		}
		pOverlap->Offset = 0xffffffff;
		pOverlap->OffsetHigh = 0xffffffff;

		BOOL bsuc = WriteFile(m_hFileDest, m_pCurBuf, m_nCurSize, NULL, pOverlap);
		if (!bsuc)
		{
			if (ERROR_IO_PENDING != GetLastError())
			{
				if (pbufsource != NULL)
					pbufsource->ReturnBuf(pdatabuf);
				m_IOReqList.PushTail(pOverlap);
				OutputDebugString(_T("CFileWriter::ImportData WriteFile Fail"));
			}
		}
		m_nWriteNum++;
		//get a new buf for cache
		m_pCurBuf = (unsigned char*)m_BufList.PopHead();
		if (m_pCurBuf == NULL)
		{
			m_pCurBuf = new unsigned char[WFILEBUFSIZE];
			if (m_pCurBuf == NULL)
			{
				OutputDebugString(_T("CFileWriter::ImportData alloc new buf fail"));
				if (pbufsource != NULL)
					pbufsource->ReturnBuf(pdatabuf);
//				ReleaseSRWLockExclusive(&m_SRWLock);
				return -2;
			}
			InterlockedExchangeAdd(&m_BufList.m_nAllocatedEleNum, 1);
		}
		m_nCurSize = 0;
	}
	memcpy(m_pCurBuf + m_nCurSize, pdatabuf, datasize);
	m_nCurSize += datasize;
	if (pbufsource != NULL)
		pbufsource->ReturnBuf(pdatabuf);
//	ReleaseSRWLockExclusive(&m_SRWLock);

	return 0;
}
