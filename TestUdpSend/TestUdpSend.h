
// TestUdpSend.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestUdpSendApp: 
// �йش����ʵ�֣������ TestUdpSend.cpp
//

class CTestUdpSendApp : public CWinApp
{
public:
	CTestUdpSendApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestUdpSendApp theApp;