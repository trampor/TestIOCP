// G-TcpServerLibDemo.h : PROJECT_unAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGTcpServerDemoApp:
// �йش����ʵ�֣������ G-TcpServerLibDemo.cpp
//

class CGTcpServerDemoApp : public CWinApp
{
public:
	CGTcpServerDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGTcpServerDemoApp theApp;