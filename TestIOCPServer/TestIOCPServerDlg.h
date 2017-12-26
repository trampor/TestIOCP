
// TestIOCPServerDlg.h : 头文件
//

#pragma once
#include "TcpNetworkConsumer.h"
#include "TcpIOCPServer.h"
#include "afxwin.h"


// CTestIOCPServerDlg 对话框
class CTestIOCPServerDlg : public CDialogEx,public CTcpNetworkConsumer
{
// 构造
public:
	CTestIOCPServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTIOCPSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	virtual int ClientStatusNotify(CTcpIOCPClient* pclient, int status_code);

	virtual int OperateNotify(int opertype, CTcpIOCPClient* pclient, unsigned long seq_number, int ret_code, unsigned char* pdatabuf, int datalength);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CTcpIOCPServer *m_pServer;
public:
	afx_msg void OnBnClickedButtonCtrl();
	CComboBox m_cLocalInterface;
	UINT m_nPort;
	UINT m_nThreadNums;
	bool m_bRunning;
	afx_msg void OnClose();
};
