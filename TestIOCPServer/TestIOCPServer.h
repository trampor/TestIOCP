
// TestIOCPServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestIOCPServerApp: 
// �йش����ʵ�֣������ TestIOCPServer.cpp
//

class CTestIOCPServerApp : public CWinApp
{
public:
	CTestIOCPServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestIOCPServerApp theApp;