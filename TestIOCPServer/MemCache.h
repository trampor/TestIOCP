#pragma once
#include <list>
using namespace std;

class CMemCache
{
public:
	static int InitObj();
	static int UninitObj();
	static unsigned char* GetMemBlock(int blocksize);
	static int ReleaseMemBlock(unsigned char* pdata, int blocksize);

private:
	static bool m_bInited;
	static list<unsigned char*> m_BigList;
	static list<unsigned char*> m_FreeList[32];
	static CRITICAL_SECTION m_FreeSection;
};

