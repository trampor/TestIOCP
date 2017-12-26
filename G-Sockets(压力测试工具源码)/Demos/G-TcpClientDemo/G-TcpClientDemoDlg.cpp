// G-TcpClientDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "G-TcpClientDemo.h"
#include "G-TcpClientDemoDlg.h"
#include "G-TcpClientDemoMain.h"
#include "..\..\G-TcpClient.h"
#include "..\G-TcpCltRunInfo\G-TcpCltRunInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGTcpClientDemoDlg �Ի���




CGTcpClientDemoDlg::CGTcpClientDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGTcpClientDemoDlg::IDD, pParent)
	, m_unProcesserThread(0)
	, m_unWorkerThread(0)
	, m_unPerHandleNumber(1000)
	, m_unHeartbeatTime(60)
	, m_unGuardThreadSleepTime(2)
	, m_unMaxNetDelayTime(5)

	, m_bIsOnTimerSend(TRUE)
	, m_bIsOnSendedSend(FALSE)	
	, m_bIsOnHeartbeatSend(FALSE)
	, m_bIsOnResultSend(FALSE)
	, m_unSendFlag(1)
	, m_unSendSpaceTime(1000)
	, m_unSendPacketNumber(1)
	, m_unSendPackSize(28)

	, m_unInlineTime(0)
	, m_strRemotePort(_T("6800"))
	, m_unOnceConnectSpaceTime(1000)
 	, m_unOnceConnectNumber(1)
 	, m_unConnectTotal(1)

	, m_unShortConnectCount(0)
	, m_unLongConnectCount(0)	
	, m_strSendS(_T(""))
	, m_strRecvS(_T(""))
	, m_strTotalS(_T(""))
	, m_bIsPing(TRUE)
	, m_bShowInList(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_unProcesserThread = GTcpClt_GetProcesserNumber() * 2 + 2;
	m_unWorkerThread = m_unProcesserThread;

	wchar_t szIP[128];
	GTcpClt_GetHostIP(szIP, 128);
	m_strLocalIP = szIP;
	m_strRemoteIP = szIP;	
}

void CGTcpClientDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_13, m_edtBlog);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);

	DDX_Control(pDX, IDC_EDIT_PROCESSERTHREAD, m_edtProcesserThread);
	DDX_Control(pDX, IDC_EDIT_WORKERTHREAD, m_edtWorkerThread);
	DDX_Control(pDX, IDC_EDIT_HNDDATA, m_edtPerHandleNumber);
	DDX_Control(pDX, IDC_EDIT_HEARTBEATTIME, m_edtHeartbeatTime);
	DDX_Control(pDX, IDC_EDIT_GUARDTHREADSLEEPTIME, m_edtGuardThreadSleepTime);
	DDX_Control(pDX, IDC_EDIT_RECONNECTTIME, m_edtMaxNetDelayTime);

	DDX_Text(pDX, IDC_EDIT_PROCESSERTHREAD, m_unProcesserThread);
	DDX_Text(pDX, IDC_EDIT_WORKERTHREAD, m_unWorkerThread);
	DDX_Text(pDX, IDC_EDIT_HNDDATA, m_unPerHandleNumber);
	DDX_Text(pDX, IDC_EDIT_HEARTBEATTIME, m_unHeartbeatTime);
	DDX_Text(pDX, IDC_EDIT_GUARDTHREADSLEEPTIME, m_unGuardThreadSleepTime);
	DDX_Text(pDX, IDC_EDIT_RECONNECTTIME, m_unMaxNetDelayTime);

	DDX_Check(pDX, IDC_RADIO1, m_bIsOnTimerSend);
	DDX_Check(pDX, IDC_RADIO2, m_bIsOnSendedSend);
	DDX_Check(pDX, IDC_RADIO4, m_bIsOnHeartbeatSend);
	DDX_Check(pDX, IDC_RADIO3, m_bIsOnResultSend);
	DDX_Control(pDX, IDC_EDIT_SENDNUMBER, m_edtSendSpaceTime);
	DDX_Text(pDX, IDC_EDIT_SENDNUMBER, m_unSendSpaceTime);
	DDX_Control(pDX, IDC_EDIT_PACKETS, m_edtSendPacketNumber);
	DDX_Text(pDX, IDC_EDIT_PACKETS, m_unSendPacketNumber);
	DDX_Text(pDX, IDC_EDIT_BYTES, m_unSendPackSize);

	DDX_Text(pDX, IDC_EDIT_INLINETIME, m_unInlineTime);
	DDX_Text(pDX, IDC_EDIT_LOCALIP, m_strLocalIP);
	DDX_Text(pDX, IDC_EDIT_REMOTEIP, m_strRemoteIP);
	DDX_Text(pDX, IDC_EDIT_REMOTEPORT, m_strRemotePort);
	DDX_Text(pDX, IDC_EDIT_NUMBERSEC, m_unOnceConnectSpaceTime);
	DDX_Text(pDX, IDC_EDIT_CONNECTNUM, m_unOnceConnectNumber);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_unConnectTotal);
	DDX_Control(pDX, IDC_EDIT_TOTAL, m_edtConnectTotal);

	DDX_Text(pDX, IDC_EDIT_LONGCONNECTNUMBER, m_unLongConnectCount);
	DDX_Text(pDX, IDC_EDIT_SHORTCONNECTNUMBER, m_unShortConnectCount);
	DDX_Text(pDX, IDC_EDIT_NUMBERSEC2, m_strSendS);
	DDX_Text(pDX, IDC_EDIT_NUMBERSEC3, m_strRecvS);
	DDX_Text(pDX, IDC_EDIT_NUMBERSEC4, m_strTotalS);
	DDX_Control(pDX, IDC_LVXC, m_lvConnect);	
	DDX_Control(pDX, IDC_RADIO3, m_btnOnResult);
	DDX_Control(pDX, IDC_CHECK1, m_btnIsPing);
	DDX_Check(pDX, IDC_CHECK1, m_bIsPing);
	DDX_Check(pDX, IDC_CHECK2, m_bShowInList);
}

BEGIN_MESSAGE_MAP(CGTcpClientDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CGTcpClientDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGTcpClientDemoDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON6, &CGTcpClientDemoDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CGTcpClientDemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_START, &CGTcpClientDemoDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CGTcpClientDemoDlg::OnBnClickedButtonConnect)

	ON_EN_CHANGE(IDC_EDIT_PROCESSERTHREAD, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)
	ON_EN_CHANGE(IDC_EDIT_WORKERTHREAD, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)
	ON_EN_CHANGE(IDC_EDIT_HNDDATA, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)
	ON_EN_CHANGE(IDC_EDIT_HEARTBEATTIME, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)
	ON_EN_CHANGE(IDC_EDIT_GUARDTHREADSLEEPTIME, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)
	ON_EN_CHANGE(IDC_EDIT_RECONNECTTIME, &CGTcpClientDemoDlg::OnEnChangeEditProcesserthread)

	ON_BN_CLICKED(IDC_CHECK1, &CGTcpClientDemoDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_RADIO1, &CGTcpClientDemoDlg::OnRadio)
	ON_BN_CLICKED(IDC_RADIO2, &CGTcpClientDemoDlg::OnRadio)
	ON_BN_CLICKED(IDC_RADIO3, &CGTcpClientDemoDlg::OnRadio)
	ON_BN_CLICKED(IDC_RADIO4, &CGTcpClientDemoDlg::OnRadio)
	ON_EN_CHANGE(IDC_EDIT_SENDNUMBER, &CGTcpClientDemoDlg::OnEnChangeEditSendSpaceTime)
	ON_EN_CHANGE(IDC_EDIT_PACKETS, &CGTcpClientDemoDlg::OnEnChangeEditSendSpaceTime)
	ON_EN_CHANGE(IDC_EDIT_BYTES, &CGTcpClientDemoDlg::OnEnChangeEditSendSpaceTime)

	ON_EN_CHANGE(IDC_EDIT_INLINETIME, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_LOCALIP, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_REMOTEIP, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_REMOTEPORT, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_CONNECTNUM, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_CONNECT, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)
	ON_EN_CHANGE(IDC_EDIT_TOTAL, &CGTcpClientDemoDlg::OnEnChangeEditInlinetime)

	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK2, &CGTcpClientDemoDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CGTcpClientDemoDlg ��Ϣ�������

void CGTcpClientDemoDlg::SetListCtrlStyleDemo(CListCtrl* pLstCtrl)
{
	//���ԭ�з��
	DWORD dwStyle = GetWindowLong(pLstCtrl->m_hWnd, GWL_STYLE); 
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);

	//�����·��
    SetWindowLong(pLstCtrl->m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS);

	//������չ���
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(pLstCtrl->m_hWnd, styles, styles );
}

BOOL CGTcpClientDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	//CString str;
	//wchar_t wt[256];

	//this->GetWindowText(wt, 255);
	//str.Format(_T("%s %s"), wt, GTcpClt_GetVersionName());	
	//this->SetWindowText(str);

	m_edtBlog.SetWindowTextW(_T("http://Blog.CSDN.Net/GuestCode   �����   48092788@qq.com"));
	GTcpClt_CreateRunInfo();
	m_btnConnect.EnableWindow(FALSE);

	m_edtSendSpaceTime.EnableWindow(1 == m_unSendFlag);
	m_edtSendPacketNumber.EnableWindow(1 == m_unSendFlag);

	SetListCtrlStyleDemo(&m_lvConnect);

	LV_COLUMN col;

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Inder");
	col.cx = 35;
	m_lvConnect.InsertColumn(0, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("RIP");
	col.cx = 35;
	m_lvConnect.InsertColumn(1, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("RPort");
	col.cx = 45;
	m_lvConnect.InsertColumn(2, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("State");
	col.cx = 45;
	m_lvConnect.InsertColumn(3, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Time");
	col.cx = 45;
	m_lvConnect.InsertColumn(4, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Rate");
	col.cx = 60;
	m_lvConnect.InsertColumn(5, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("IErr");
	col.cx = 45;
	m_lvConnect.InsertColumn(6, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("CErr");
	col.cx = 45;
	m_lvConnect.InsertColumn(7, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Send");
	col.cx = 50;
	m_lvConnect.InsertColumn(8, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Recv");
	col.cx = 50;
	m_lvConnect.InsertColumn(9, &col);

	m_pServerTestTool = new CServerTestTool(m_hWnd);
	SetSendParam();
	SetTimer(1001, 1000, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGTcpClientDemoDlg::OnDestroy()
{
	m_lvConnect.DeleteAllItems();
	GTcpClt_DestroyRunInfo();
	delete m_pServerTestTool;

	CDialog::OnDestroy();
	
	// TODO: �ڴ˴������Ϣ����������
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGTcpClientDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGTcpClientDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGTcpClientDemoDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CGTcpClientDemoDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CGTcpClientDemoDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShellExecute(0, _T("open"), _T("http://Blog.CSDN.Net/GuestCode"), NULL, NULL, SW_SHOWNORMAL);
}

void CGTcpClientDemoDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GTcpClt_ShowRunInfo();
}

void CGTcpClientDemoDlg::OnEnChangeEditProcesserthread()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	wchar_t szText[128];

	::GetWindowText(::GetFocus(), LPWSTR(szText), 127);
	if(szText[0])
		UpdateData(TRUE);
}

void CGTcpClientDemoDlg::EnabledCtrls(BOOL bEnabled)
{
	m_edtProcesserThread.EnableWindow(bEnabled);
	m_edtWorkerThread.EnableWindow(bEnabled);
	m_edtPerHandleNumber.EnableWindow(bEnabled);
	m_edtGuardThreadSleepTime.EnableWindow(bEnabled);
	m_edtHeartbeatTime.EnableWindow(bEnabled);
	m_edtMaxNetDelayTime.EnableWindow(bEnabled);
	m_btnConnect.EnableWindow(!bEnabled);
}

void CGTcpClientDemoDlg::SetSendParam(void)
{
	//�������÷��Ͳ�������Ӱ�쵽�������ߵ�����
	m_pServerTestTool->SetSendFlag(m_unSendFlag);
	m_pServerTestTool->SetSendSpaceTimeOfOnce(m_unSendSpaceTime);
	m_pServerTestTool->SetSendPacketNumberOfOnce(m_unSendPacketNumber);
	m_pServerTestTool->SetSendPacketSize(m_unSendPackSize);
	m_pServerTestTool->SetShowInList(m_bShowInList);
}

void CGTcpClientDemoDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//����ģʽ���ã���������������Ч����Ӱ�쵽�������ߵ�����
	//Ping��ʽ�ķ��������������ݣ����OnResult����ģʽ��Ч
	UpdateData(TRUE);
	if((!m_bIsPing) && (4 == m_unSendFlag))
	{
		m_bIsOnSendedSend = TRUE;
		m_bIsOnTimerSend = m_bIsOnHeartbeatSend = m_bIsOnResultSend = FALSE;
		UpdateData(FALSE);
		m_unSendFlag = 2;
		SetSendParam();
	}
	m_btnOnResult.EnableWindow(m_bIsPing);
}

void CGTcpClientDemoDlg::OnRadio()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//����ģʽ���ã���������������Ч����Ӱ�쵽�������ߵ�����
	UpdateData(TRUE);
	if(m_bIsOnTimerSend)
		m_unSendFlag = 1;
	else
	if(m_bIsOnSendedSend)
		m_unSendFlag = 2;
	else
	if(m_bIsOnHeartbeatSend)
		m_unSendFlag = 3;
	else
	if(m_bIsOnResultSend)
		m_unSendFlag = 4;
	m_edtSendSpaceTime.EnableWindow(1 == m_unSendFlag);
	m_edtSendPacketNumber.EnableWindow(1 == m_unSendFlag);
	if((4 != m_unSendFlag) || (m_bIsPing))
		SetSendParam();
}

void CGTcpClientDemoDlg::OnEnChangeEditSendSpaceTime()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	//���Ͳ������ã���������������Ч����Ӱ�쵽�������ߵ�����
	wchar_t szText[128];

	::GetWindowText(::GetFocus(), LPWSTR(szText), 127);
	if(szText[0])
	{
		UpdateData(TRUE);		
		if(GTcpClt_GetGBufSize() < m_unSendPackSize)
		{
			m_unSendPackSize = GTcpClt_GetGBufSize();
			UpdateData(FALSE);
		}
		if(28 > m_unSendPackSize)
		{
			m_unSendPackSize = 28;
			UpdateData(FALSE);
		}
		SetSendParam();
	}
}

void CGTcpClientDemoDlg::OnEnChangeEditInlinetime()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	
	//���Ӳ������ã���Ӱ�쵽δ��ɵ������߳�
	wchar_t szText[128];

	::GetWindowText(::GetFocus(), LPWSTR(szText), 127);
	if(szText[0])
		UpdateData(TRUE);
}

void CGTcpClientDemoDlg::OnBnClickedCheck2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	SetSendParam();
}

DWORD dwTickCountSave = 0;
void CGTcpClientDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//��ʱ����ʱ��ȡ�����Ϣ
	m_unLongConnectCount = m_pServerTestTool->GetLongConnectCount();
	m_unShortConnectCount = m_pServerTestTool->GetShortConnectCount();
	if(!dwTickCountSave)
	{
		dwTickCountSave = GetTickCount();
		return;
	}
	unsigned int unSB, unRB;
	DWORD dwTickCount = GetTickCount();
	float f;

	unSB = m_pServerTestTool->GetSendBytes();
	unRB = m_pServerTestTool->GetRecvBytes();
	f = (float)unRB + unSB;
	f = f * 1000 / (dwTickCount - dwTickCountSave) / 1024;
	m_strTotalS.Format(_T("%0.2f"), f);

	f = (float)unSB * 1000 / (dwTickCount - dwTickCountSave) / 1024;
	m_strSendS.Format(_T("%0.2f"), f);

	f = (float)unRB * 1000 / (dwTickCount - dwTickCountSave) / 1024;
	m_strRecvS.Format(_T("%0.2f"), f);
	dwTickCountSave = dwTickCount;

	UpdateData(FALSE);

	LV_ITEM lvitem;
	int i, nEnd, nStart;
	CString str;

	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;

	nStart = m_lvConnect.GetTopIndex();
	nEnd = nStart + 5;
	if(nEnd > m_lvConnect.GetItemCount())
		nEnd = m_lvConnect.GetItemCount();
	CConnection* pConnect;
	for(i = nStart; i < nEnd; i++)
	{
		pConnect = (CConnection*)m_lvConnect.GetItemData(i);
		lvitem.iItem = i;
		lvitem.iSubItem = 3;
		switch(GTcpClt_GetState(pConnect->GetPerHandle()))
		{
		case HNDS_CONNECT :
			str.Format(_T("Ing"));
			break;
		case HNDS_CONNECTED :
			str.Format(_T("Conn"));
			break;
		case HNDS_DISCONNECT :
			str.Format(_T("Dis"));
			break;		
		}
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 4;
		str.Format(_T("%d"), pConnect->GetTime());
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 5;
		f = (float)pConnect->GetSpeed() / 1024;
		str.Format(_T("%0.2f"), f);
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 6;
		str.Format(_T("%d"), pConnect->GetIndexError());
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 7;
		str.Format(_T("%d"), pConnect->GetCRCError());
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 8;
		unSB = pConnect->GetSendBytes();
		if(unSB < 1024)
			str.Format(_T("%d"), unSB);
		else
		if(unSB < 1024 * 1024)
			str.Format(_T("%0.3fK"), (float)unSB / 1024);
		else
			str.Format(_T("%0.5fM"), (float)unSB / (1024 * 1024));
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);

		lvitem.iItem = i;
		lvitem.iSubItem = 9;
		unRB = pConnect->GetRecvBytes();
		if(unRB < 1024)
			str.Format(_T("%d"), unRB);
		else
		if(unRB < 1024 * 1024)
			str.Format(_T("%0.3fK"), (float)unRB / 1024);
		else
			str.Format(_T("%0.5fM"), (float)unRB / (1024 * 1024));
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvConnect.SetItem(&lvitem);
	}

	CDialog::OnTimer(nIDEvent);
}

#include <ws2tcpip.h>
void CGTcpClientDemoDlg::AddConnect(void* pConnect)
{
	//����ĳ��������
	if(!GTcpClt_IsActive())
		return;
	LV_ITEM lvitem;
	CString str;
	int nIndex;

	nIndex = m_lvConnect.GetItemCount();
	if(0 > nIndex)
		nIndex = 0;
	lvitem.state = 0;
	lvitem.stateMask = 0;

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 0;
	str.Format(_T("%d"), nIndex + 1);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	m_lvConnect.InsertItem(&lvitem);

	m_lvConnect.SetItemData(nIndex, (DWORD_PTR)pConnect);

	wchar_t	szIP[NI_MAXHOST], szPort[NI_MAXSERV];
	GTcpSock_GetPerHandleInfo(((CConnection*)pConnect)->GetPerHandle(), szIP, NI_MAXHOST, szPort, NI_MAXSERV);
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 1;
	str.Format(_T("%s"), szIP);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 2;
	str.Format(_T("%s"), szPort);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("ing");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 4;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 5;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 6;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 7;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 8;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 9;
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("0");
	m_lvConnect.SetItem(&lvitem);
}

void CGTcpClientDemoDlg::DelConnect(void* pConnect)
{
	//ɾ��ĳ��������
	LV_ITEM lvitem;
	int i, j, nEnd;
	CString str;

	nEnd = m_lvConnect.GetItemCount();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	for(i = 0; i < nEnd; i++)
	{
		if((void*)m_lvConnect.GetItemData(i) == pConnect)
		{
			m_lvConnect.DeleteItem(i);
			nEnd = m_lvConnect.GetItemCount();
			for(j = i; j < nEnd; j++)
			{
				lvitem.iItem = j;
				lvitem.iSubItem = 0;
				str.Format(_T("%d"), j + 1);
				lvitem.pszText = (LPTSTR)(LPCTSTR)str;
				m_lvConnect.SetItem(&lvitem);
			}
			((CConnection*)pConnect)->SetAllowFree();
			return;
		}
	}
	((CConnection*)pConnect)->SetAllowFree();
}

BOOL CGTcpClientDemoDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	switch(message)
	{
		case WM_USER + 101:
			AddConnect((void*)wParam);
			break;
		case WM_USER + 102:
			DelConnect((void*)wParam);
			break;
	}

	return(CDialog::OnWndMsg(message, wParam, lParam, pResult));
}

void CGTcpClientDemoDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!GTcpClt_IsActive())
	{
		//��������
		if(m_pServerTestTool->Start(m_unHeartbeatTime,
								 m_unMaxNetDelayTime,
								 m_unGuardThreadSleepTime,
								 m_unPerHandleNumber,
								 m_unProcesserThread,
								 m_unWorkerThread))
		{
			m_unShortConnectCount = 0;
			m_btnStart.SetWindowText(_T("Stop"));
			EnabledCtrls(FALSE);
		}
	}else
	{
		//�رշ���
		UpdateWindow();
		m_lvConnect.DeleteAllItems();
		GTcpClt_ClearRunInfo();
		if(m_pServerTestTool->Stop())
		{			
			m_btnStart.SetWindowText(_T("Start"));
			EnabledCtrls(TRUE);
		}
	}
}

void CGTcpClientDemoDlg::OnBnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�������Ӳ������������߳�
	m_pServerTestTool->OpenConnect(m_strLocalIP.GetBuffer(),
								   m_strRemoteIP.GetBuffer(),
								   m_strRemotePort.GetBuffer(),
								   m_unInlineTime,
								   m_unConnectTotal,
								   m_unOnceConnectSpaceTime,
								   m_unOnceConnectNumber);

}

