
// TestUdpReceiverDlg.h : 头文件
//

#pragma once
#include "CalcCenter.h"
#include "afxcmn.h"
#include "UdpReceiver.h"
#include "filewriter.h"
#include <list>
using namespace std;

#include "NoLockBiList.h"

#define THREADNUM 20

// CTestUdpReceiverDlg 对话框
class CTestUdpReceiverDlg : public CDialogEx
{
// 构造
public:
	CTestUdpReceiverDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTestUdpReceiverDlg();

// 对话框数据
	enum { IDD = IDD_TESTUDPRECEIVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	CCalcCenter m_CalcCenter;
	CImageList m_itemImageList;
	list<CUdpReceiver*> m_RecvList;
	list<CFileWriter*> m_FileList;
	unsigned long m_nLastTickCount;

	CNoLockBiList<int> templist;
	bool m_brun;
	HANDLE m_hThreads[THREADNUM];
	unsigned int m_ThreadData;
	static unsigned WINAPI InitTestThread(LPVOID pv);
	unsigned TestThread();

public:
	CListCtrl m_cRecvList;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonStartall();
	afx_msg void OnBnClickedButtonStopall();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
