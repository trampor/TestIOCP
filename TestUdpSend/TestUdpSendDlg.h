
// TestUdpSendDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CTestUdpSendDlg �Ի���
class CTestUdpSendDlg : public CDialogEx
{
// ����
public:
	CTestUdpSendDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTUDPSEND_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
