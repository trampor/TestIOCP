// G-TcpClientDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGTcpClientDemoApp:
// �йش����ʵ�֣������ G-TcpClientDemo.cpp
//

class CGTcpClientDemoApp : public CWinApp
{
public:
	CGTcpClientDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGTcpClientDemoApp theApp;