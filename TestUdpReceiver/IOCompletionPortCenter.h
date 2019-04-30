#pragma once
#include "iocompletionportbase.h"

#ifdef _DEBUG
#define DEBUGOUTPUT(format,...) printf("File: "__FILE__", Line: %05d: "format"/n", __LINE__, ##__VA_ARGS__)  
#else  
#define DEBUGOUTPUT(format,...)  
#endif  

class CIOCompletionPortCenter :public CIOCompletionPortBase
{
public:
	CIOCompletionPortCenter();
	~CIOCompletionPortCenter();

	int InitObj(int nthreadnumber = 0);
	int UninitObject();

private:
	static unsigned WINAPI InitProcessThread(LPVOID pv);
	unsigned ProcessThread();

private:
	int m_nThreadNum;
	bool m_bProcessing;
	HANDLE *m_pThreadHandles;
	unsigned int *m_pThreadData;
};

