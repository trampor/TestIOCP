#include "stdafx.h"
#include "MemCache.h"

bool CMemCache::m_bInited = false;
list<unsigned char*> CMemCache::m_BigList;
list<unsigned char*> CMemCache::m_FreeList[32];
CRITICAL_SECTION CMemCache::m_FreeSection;

int CMemCache::InitObj()
{
	if (m_bInited)
		return 0;
	m_bInited = true;
	InitializeCriticalSection(&m_FreeSection);

	return 0;
}

int CMemCache::UninitObj()
{
	for (auto iter = m_BigList.begin(); iter != m_BigList.end(); iter++)
		delete[] * iter;
	m_BigList.clear();
	DeleteCriticalSection(&m_FreeSection);
	return 0;
}

unsigned char* CMemCache::GetMemBlock(int blocksize)
{
	unsigned char *pdata = NULL;
	EnterCriticalSection(&m_FreeSection);
	for (auto i = 0; i<32; i++)
	{
		if (blocksize <= (2<<i))
		{
			if (m_FreeList[i].size() > 0)
			{
				pdata = *(m_FreeList[i].begin());
				m_FreeList[i].pop_front();
			}
			else
			{
				int num = 1000;
				if (i > 12 && i<= 16)
					num = 100;
				else if (i > 16)
					num = 5;
				pdata = new unsigned char[num*(2<<i)];
				if (pdata != NULL)
				{
					m_BigList.push_back(pdata);
					for (auto j = 1; j < num; j++)
						m_FreeList[i].push_back(pdata + j*(2<< i));
				}
			}
			break;
		}
	}
	LeaveCriticalSection(&m_FreeSection);
	return pdata;
}

int CMemCache::ReleaseMemBlock(unsigned char* pdata, int blocksize)
{
	EnterCriticalSection(&m_FreeSection);
	for (auto i = 0; i < 32; i++)
	{
		if (blocksize <= (2 << i))
		{
			m_FreeList[i].push_back(pdata);
			break;
		}
	}
	LeaveCriticalSection(&m_FreeSection);
	return 0;
}