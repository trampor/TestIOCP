
// TestUdpReceiver.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestUdpReceiverApp: 
// �йش����ʵ�֣������ TestUdpReceiver.cpp
//

class CTestUdpReceiverApp : public CWinApp
{
public:
	CTestUdpReceiverApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestUdpReceiverApp theApp;