
// TestUdpReceiverDlg.h : ͷ�ļ�
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
