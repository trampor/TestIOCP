
// TestUdpSendDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CTestUdpSendDlg 对话框
class CTestUdpSendDlg : public CDialogEx
{
// 构造
public:
	CTestUdpSendDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTUDPSEND_DIALOG };

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
public:
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonAdd();
	CComboBox m_cLocalInterface;
	CEdit m_cFilePath;
	BOOL m_bBitrate;
	UINT m_nBitrate;
	CString m_sDestIp;
	UINT m_nPort;
	UINT m_nNum;
};
