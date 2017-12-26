
// TestIOCPServerDlg.h : ͷ�ļ�
//

#pragma once
#include "TcpNetworkConsumer.h"
#include "TcpIOCPServer.h"
#include "afxwin.h"


// CTestIOCPServerDlg �Ի���
class CTestIOCPServerDlg : public CDialogEx,public CTcpNetworkConsumer
{
// ����
public:
	CTestIOCPServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTIOCPSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	virtual int ClientStatusNotify(CTcpIOCPClient* pclient, int status_code);

	virtual int OperateNotify(int opertype, CTcpIOCPClient* pclient, unsigned long seq_number, int ret_code, unsigned char* pdatabuf, int datalength);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
