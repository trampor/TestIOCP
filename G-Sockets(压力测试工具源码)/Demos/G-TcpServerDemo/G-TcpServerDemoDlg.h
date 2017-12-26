// G-TcpServerLibDemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CGTcpServerDemoDlg 对话框
class CGTcpServerDemoDlg : public CDialog
{
// 构造
public:
	CGTcpServerDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GTCPSERVERDEMO_DIALOG };

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
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	unsigned int m_unMaxConnection;
	unsigned int m_unMaxPostAccept;
	unsigned int m_unMaxNetDelayTime;

	unsigned int m_unProcesserThreadNumber;
	unsigned int m_unWorkerThreadNumber;
	unsigned int m_unGuardThreadSleepTime;

	unsigned int m_unIoDataNumber;
	unsigned int m_unHndDataExNumber;
	unsigned int m_unHeartbeatTime;

	CButton m_btnStart;
	unsigned int m_unListenCount;
	CString m_strHostIP;
	unsigned int m_strListenPort;
	CButton m_btnListen;
	CString m_strBroad;
	CButton m_btnBroad;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton2();
	CEdit m_edt1;
	CEdit m_edt2;
	CEdit m_edt3;
	CEdit m_edt4;
	CEdit m_edt5;
	CEdit m_edt6;
	CEdit m_edt7;
	CEdit m_edt8;
	CEdit m_edt10;
	CEdit m_edtLoop;
	unsigned int m_unLoop;

	void EnableCtrls(BOOL bEnabled);
	afx_msg void OnBnClickedButton3();
	CEdit m_edtBlog;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	BOOL m_bIsPing;
	afx_msg void OnBnClickedCheck1();
};
