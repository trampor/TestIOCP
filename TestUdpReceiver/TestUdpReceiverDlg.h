
// TestUdpReceiverDlg.h : ͷ�ļ�
//

#pragma once
#include "IOCompletionPortCenter.h"
#include "afxcmn.h"
#include "UdpReceiver.h"
#include "Tcplistener.h"
#include "Tcpworker.h"
#include "rtspserver.h"
#include "filewriter.h"
#include <list>
#include "tcpconnecter.h"
using namespace std;

#include "NoLockBiList.h"

// CTestUdpReceiverDlg �Ի���
class CTestUdpReceiverDlg : public CDialogEx
{
// ����
public:
	CTestUdpReceiverDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTestUdpReceiverDlg();

// �Ի�������
	enum { IDD = IDD_TESTUDPRECEIVER_DIALOG };

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

	CIOCompletionPortCenter m_CalcCenter;
	CImageList m_itemImageList;
	list<CUdpReceiver*> m_RecvList;
	list<CTcpConnecter*> m_ClientList;
	list<CTcpListener<CRtspServer>*> m_TcpList;
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
	afx_msg void OnBnClickedButtonClientadd();
	afx_msg void OnBnClickedButtonClientstart();
	afx_msg void OnBnClickedButtonClientstop();
	afx_msg void OnBnClickedButtonClientdel();
	afx_msg void OnBnClickedButtonStartallclient();
	afx_msg void OnBnClickedButtonStopallclient();
	CListCtrl m_cClientList;
	afx_msg void OnBnClickedButtonDeleteallclient();
};
