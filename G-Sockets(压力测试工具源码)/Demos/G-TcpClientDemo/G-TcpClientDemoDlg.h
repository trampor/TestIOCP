// G-TcpClientDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "G-TcpClientDemoMain.h"


// CGTcpClientDemoDlg 对话框
class CGTcpClientDemoDlg : public CDialog
{
// 构造
public:
	CGTcpClientDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GTCPCLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult);
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_edtBlog;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	
	CButton m_btnStart;
	CButton m_btnConnect;

	CEdit m_edtProcesserThread;
	CEdit m_edtWorkerThread;
	CEdit m_edtPerHandleNumber;
	CEdit m_edtHeartbeatTime;
	CEdit m_edtGuardThreadSleepTime;
	CEdit m_edtMaxNetDelayTime;
	unsigned int m_unProcesserThread;
	unsigned int m_unWorkerThread;
	unsigned int m_unPerHandleNumber;
	unsigned int m_unHeartbeatTime;
	unsigned int m_unGuardThreadSleepTime;
	unsigned int m_unMaxNetDelayTime;
	afx_msg void OnBnClickedStart();
	afx_msg void OnEnChangeEditProcesserthread();
	
	BOOL m_bIsOnTimerSend;
	BOOL m_bIsOnSendedSend;	
	BOOL m_bIsOnHeartbeatSend;
	BOOL m_bIsOnResultSend;
	unsigned int m_unSendFlag;
	afx_msg void OnRadio();
	unsigned int m_unSendSpaceTime;
	unsigned int m_unSendPacketNumber;
	unsigned int m_unSendPackSize;
	CEdit m_edtSendSpaceTime;
	CEdit m_edtSendPacketNumber;
	afx_msg void OnEnChangeEditSendSpaceTime();

	unsigned int m_unInlineTime;
	CString m_strLocalIP;
	CString m_strRemoteIP;
	CString m_strRemotePort;	
	unsigned int m_unOnceConnectSpaceTime;
	unsigned int m_unOnceConnectNumber;
	unsigned int m_unConnectTotal;
	CEdit m_edtConnectTotal;
	afx_msg void OnBnClickedButtonConnect();

	unsigned int m_unLongConnectCount;
	unsigned int m_unShortConnectCount;
	CString m_strSendS;
	CString m_strRecvS;
	CString m_strTotalS;
	CListCtrl m_lvConnect;

	void AddConnect(void* pConnect);
	void DelConnect(void* pConnect);
	void SetSendParam(void);
	void SetListCtrlStyleDemo(CListCtrl* pLstCtrl);
	void EnabledCtrls(BOOL bEnabled);
	afx_msg void OnDestroy();	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CServerTestTool* m_pServerTestTool;
	afx_msg void OnEnChangeEditInlinetime();
	CButton m_btnOnResult;
	CButton m_btnIsPing;
	BOOL m_bIsPing;
	afx_msg void OnBnClickedCheck1();
	BOOL m_bShowInList;
	afx_msg void OnBnClickedCheck2();
};
