
// TestUdpReceiverDlg.h : 头文件
//

#pragma once
#include "IOCompletionPortCenter.h"
#include "afxcmn.h"
#include "UdpReceiver.h"
#include "Tcplistener.h"
#include "Tcpworker.h"
#include "filewriter.h"
#include <list>
using namespace std;

#include "NoLockBiList.h"

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

	CIOCompletionPortCenter m_CalcCenter;
	CImageList m_itemImageList;
	list<CUdpReceiver*> m_RecvList;
	list<CTcpListener<CTcpWorker>*> m_TcpList;
	list<CFileWriter*> m_FileList;
	unsigned long m_nLastTickCount;

public:
	CListCtrl m_cRecvList;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonStartall();
	afx_msg void OnBnClickedButtonStopall();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_cTcpList;
	afx_msg void OnBnClickedButtonTcpadd();
	afx_msg void OnBnClickedButtonTcpstop();
	afx_msg void OnBnClickedButtonTcpstart();
	afx_msg void OnBnClickedButtonTcpdel();
};
