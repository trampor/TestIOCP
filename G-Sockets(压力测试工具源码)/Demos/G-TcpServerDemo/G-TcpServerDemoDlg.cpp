// G-TcpServerDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "G-TcpServerDemo.h"
#include "G-TcpServerDemoDlg.h"

#include "..\..\G-TcpServer.h"
#include "..\G-TcpSvrRunInfo\G-TcpSvrRunInfo.h"

#ifdef _DEBUG
#define new DEBUG_unEW
#endif


// CGTcpServerDemoDlg �Ի���

CGTcpServerDemoDlg::CGTcpServerDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGTcpServerDemoDlg::IDD, pParent)
	, m_unLoop(0)

	, m_unMaxConnection(10000)
	, m_unMaxPostAccept(1000)
	, m_unMaxNetDelayTime(5)

	, m_unProcesserThreadNumber(0)
	, m_unWorkerThreadNumber(0)
	, m_unGuardThreadSleepTime(2)

	, m_unIoDataNumber(15000)
	, m_unHndDataExNumber(15000)
	, m_unHeartbeatTime(60)

	, m_strHostIP(_T(""))
	, m_strListenPort(6800)
	, m_strBroad(_T(""))
	, m_bIsPing(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_unProcesserThreadNumber = GTcpSvr_GetProcesserNumber() * 2 + 2;
	m_unWorkerThreadNumber = m_unProcesserThreadNumber;
	wchar_t szIP[20];
	GTcpSvr_GetHostIP(szIP, 20);
	m_strHostIP = szIP;
	m_strBroad = _T("This is Server Message!");
}

void CGTcpServerDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GEDIT_HDDT_TOTAL2, m_unMaxConnection);
	DDX_Text(pDX, IDC_GEDIT_HDDT_USED3, m_unMaxPostAccept);
	DDX_Text(pDX, IDC_GEDIT_HDDT_BYTES3, m_unMaxNetDelayTime);

	DDX_Text(pDX, IDC_GEDIT_HDDT_USED_R2, m_unProcesserThreadNumber);
	DDX_Text(pDX, IDC_GEDIT_HDDT_USED4, m_unWorkerThreadNumber);
	DDX_Text(pDX, IDC_GEDIT_HDDT_BYTES4, m_unGuardThreadSleepTime);

	DDX_Text(pDX, IDC_GEDIT_IODT_TOTAL2, m_unIoDataNumber);
	DDX_Text(pDX, IDC_GEDIT_HDDT_USED5, m_unHndDataExNumber);
	DDX_Text(pDX, IDC_GEDIT_HDDT_BYTES5, m_unHeartbeatTime);

	DDX_Control(pDX, IDC_BUTTON1, m_btnStart);
	DDX_Text(pDX, IDC_GEDIT_IODT_TOTAL3, m_strHostIP);
	DDX_Control(pDX, IDC_BUTTON2, m_btnListen);
	DDX_Text(pDX, IDC_GEDIT_IODT_TOTAL4, m_strBroad);
	DDX_Control(pDX, IDC_BUTTON3, m_btnBroad);
	DDX_Text(pDX, IDC_EDIT_11, m_strListenPort);
	DDX_Control(pDX, IDC_EDIT_1, m_edt1);
	DDX_Control(pDX, IDC_EDIT_2, m_edt2);
	DDX_Control(pDX, IDC_EDIT_3, m_edt3);
	DDX_Control(pDX, IDC_EDIT_4, m_edt4);
	DDX_Control(pDX, IDC_EDIT_5, m_edt5);
	DDX_Control(pDX, IDC_EDIT_6, m_edt6);
	DDX_Control(pDX, IDC_EDIT_7, m_edt7);
	DDX_Control(pDX, IDC_EDIT_8, m_edt8);
	DDX_Control(pDX, IDC_EDIT_9, m_edt10);
	DDX_Control(pDX, IDC_EDIT_13, m_edtBlog);
	DDX_Text(pDX, IDC_LOOP, m_unLoop);
	DDX_Control(pDX, IDC_LOOP, m_edtLoop);
	DDX_Check(pDX, IDC_CHECK1, m_bIsPing);
}

BEGIN_MESSAGE_MAP(CGTcpServerDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CGTcpServerDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGTcpServerDemoDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_LOOP, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_SEND_unUMBER, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_1, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_2, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_3, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_4, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_5, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_6, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_7, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_8, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_9, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_10, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_11, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT_12, &CGTcpServerDemoDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CGTcpServerDemoDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CGTcpServerDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CGTcpServerDemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CGTcpServerDemoDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CGTcpServerDemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_CHECK1, &CGTcpServerDemoDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CGTcpServerDemoDlg ��Ϣ�������
void SetListCtrlStyleDemo(CListCtrl* pLstCtrl)
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

BOOL CGTcpServerDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_btnListen.EnableWindow(FALSE);
	m_btnBroad.EnableWindow(FALSE);

	CString str;
	wchar_t wt[256];

	this->GetWindowText(wt, 255);
	str.Format(_T("%s %s"), wt, GTcpSvr_GetVersionName());	
	this->SetWindowText(str);

	m_edtBlog.SetWindowTextW(_T("http://Blog.CSDN.Net/GuestCode   �����   48092788@qq.com"));
	GTcpSvr_CreateRunInfo();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CGTcpServerDemoDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ShellExecute(0, _T("open"), _T("http://Blog.CSDN.Net/GuestCode"), NULL, NULL, SW_SHOWNORMAL);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGTcpServerDemoDlg::OnPaint()
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
HCURSOR CGTcpServerDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGTcpServerDemoDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CGTcpServerDemoDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CGTcpServerDemoDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	wchar_t* szText[128];

	::GetWindowText(::GetFocus(), LPWSTR(szText), 127);
	if(szText[0])
	{
		UpdateData(TRUE);
		if(!GTcpSvr_IsActive())
		{
			DWORD n1 = (DWORD)GTcpSvr_GetPageSize() * m_unIoDataNumber + (DWORD)(m_unMaxConnection + m_unHndDataExNumber) * GTcpSvr_GetHndDataSize(),
				  n2 = (DWORD)GTcpSvr_GetPhyMemInfo(NULL) - (DWORD)100 * 1024 * 1024;
			if(n1 < n2)
			{
				m_btnStart.EnableWindow(TRUE);
				m_btnStart.SetWindowText(_T("Start"));
			}else
			{
				m_btnStart.EnableWindow(FALSE);
				m_btnStart.SetWindowText(_T("No free mem"));
			}
		}
	}
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

/********************************************************************************
				������ GTcpServer �¼��ص�����
********************************************************************************/
void __stdcall GTcpSvr_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
//˵�����߳̿�ʼ�ͽ���ʱ���߳����ڵĻص����������������������߳�Ȩ�޺ͽ����߳���صĳ�ʼ��������
//���룺hThreadHandle���߳̾��
//		nThreadID���߳�ID
//		bIsBegin��ΪTRUE��ʾ�̸߳տ�ʼ���У�������ʾ�߳̽����ˡ�
//		unFlag��1��Ϊ���ݴ������̣߳�2��ΪIO�������̣߳�3��Ϊ�����̣߳�����δ���塣
//�������
//ע�⣺�߳�Ȩ�����ÿɸ��ݴ����̡߳��������̡߳��ŷ��̵߳Ľ�ɫ�����ã�����߳��ڲ���Ҫ�ĳ�ʼ�����������Ը���
//		unFlagΪ1��bIsBeginΪ��ʱ���г�ʼ�����������磺CoInitialize(nil)��unFlagΪ1��bIsBeginΪ�ٽ��з���ʼ��
//		���������磺CoUninitialize()��
{
	//�̸߳տ�ʼ����
	if(bIsBegin)
	{
		GTcpSvrInfo_AddThread(unThreadContext, unThreadHandle, unThreadID);
		//�Ǵ����߳�
		if(1 == unFlag)
		{
			//�����̹߳������Ƚϴ�Ӧ�ø��ڹ����߳�
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
			//��SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//ֻ�д����̲߳Ż���ûص�������Ӧ���ӵ������¼������п�����Ҫ��ʼ���̣߳�
			//��ʹ�����ݿ������������Ҫ�߳��ڶ����Ĳ���
			CoInitialize(NULL);
		}else
		//�ǹ����߳�
		if(2 == unFlag)
		{
			//�����߳�Ȩ�޽��ڿ����̺߳ʹ����߳�֮��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}else
		//�ǿ����߳�
		if(3 == unFlag)
		{
			//�����߳�û�б�Ҫ����̫��Ȩ��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}
	}else
	{
		//GTcpSvrInfo_DelThread(gcThreadContext);
		//�Ǵ����߳�
		if(1 == unFlag)
		{
			//����ʼ��
			CoUninitialize();
			//ȡ���߳�ģʽ
			//��SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_END);
		}else
		//�ǹ����߳�
		if(2 == unFlag)
		{
			
		}else
		//�ǿ����߳�
		if(3 == unFlag)
		{
			
		}
	}
}

//ԭ����Ļص������Ĳ���void* pOwenrǿ��ת��Ϊunsigned int unPerHandle��
//��Ϊ��OnConnect�Ѿ���Owner����ΪunPerHandle������ͬ��
void __stdcall GTcpSvr_OnReceived(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen)
{
	//���������ַ��ͺ���ʾ��
	int i;
	CGTcpServerDemoDlg* pDlg = (CGTcpServerDemoDlg*)GTcpSvr_GetListenOwner(unPerHandle);

	i = pDlg->m_unLoop * 1000;
	while(i--);
	if(pDlg->m_bIsPing)
	{
		//����1
		/*PGBUF pGBuf;

		pGBuf = GTcpSvr_AllocGBuf();
		if(pGBuf)
		{
			memcpy(pGBuf, pBuf, nLen);
			GTcpSvr_PostSendGBuf(unPerHandle, pGBuf, nLen);
		}*/
		//����2
		GTcpSvr_PostSendBuf(unPerHandle, pBuf, unLen);
	}
}

void __stdcall GTcpSvr_OnConnected(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen)
{
	//�����ظ����룬���ý��պ������� ��)
	GTcpSvr_OnReceived(unPerHandle, pBuf, unLen);
}

void __stdcall GTcpSvr_OnSended(unsigned int unPerHandle, unsigned int unSendID,  unsigned int unLen)
{

}

void __stdcall GTcpSvr_OnDisconnected(unsigned int unPerHandle, unsigned int unFlag)
{
	
}
/********************************************************************************
				������ GTcpServer �¼��ص�����
********************************************************************************/

void CGTcpServerDemoDlg::EnableCtrls(BOOL bEnabled)
{
	m_edt1.EnableWindow(bEnabled);
	m_edt2.EnableWindow(bEnabled);
	m_edt3.EnableWindow(bEnabled);
	m_edt4.EnableWindow(bEnabled);
	m_edt5.EnableWindow(bEnabled);
	m_edt6.EnableWindow(bEnabled);
	m_edt7.EnableWindow(bEnabled);
	m_edt8.EnableWindow(bEnabled);
	m_edt10.EnableWindow(bEnabled);
}

void CGTcpServerDemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	if(!GTcpSvr_IsActive())
	{
		GTcpSvr_Start(m_unMaxConnection,				//�����������Ĭ�Ͻ����޸��������������
						m_unHeartbeatTime,		//������ʱ��ʱ�������������ʱ�����յ��ͻ��˵��������ݣ����򱻶Ͽ�
						m_unMaxNetDelayTime,			//���ܳ�ʱ���ͻ������Ӻ󣬱��������ʱ������������������ݣ����򱻶Ͽ�
						&GTcpSvr_OnThread,			//�߳̿�ʼ�ͽ����Ļص�������ַ
						m_unMaxPostAccept,			//���Ͷ��AcceptEx������
						m_unIoDataNumber,			//IoData��������ϵͳ������Ӧ���䣬��Ӧ����MaxConnection * 1.5
						m_unProcesserThreadNumber,	//�����߳��������Ը���CPU�������ã�0ʱϵͳ����ΪCPUNumber
						m_unWorkerThreadNumber,		//�����߳��������Ը���CPU��������
						m_unGuardThreadSleepTime,	//�����߳����ߵ�ʱ�䣬����֮�����Ƿ��н��ܳ�ʱ����г�ʱ
						m_unHndDataExNumber);		//HndData��չ����HndDataCount = MaxConnection + HndDataExNumber������1000����1000
		if(GTcpSvr_IsActive())
		{
			m_btnStart.SetWindowText(_T("Stop"));
			m_btnListen.EnableWindow(TRUE);
			m_btnBroad.EnableWindow(TRUE);
			EnableCtrls(FALSE);
		}
	}else
	{
		GTcpSvr_ClearRunInfo();
		GTcpSvr_Stop();
		if(!GTcpSvr_IsActive())
		{
			CString str(_T("Listen"));
			m_btnListen.SetWindowText(str.GetBuffer());
			m_btnStart.SetWindowText(_T("Start"));
			m_btnListen.EnableWindow(FALSE);
			m_btnBroad.EnableWindow(FALSE);
			m_strListenPort = 6800;
			UpdateData(FALSE);
			EnableCtrls(TRUE);
		}else
			MessageBox(_T("G-TcpServer����ʧ�ܣ������������־�ļ���"));
	}
}

void CGTcpServerDemoDlg::OnDestroy()
{
	GTcpSvr_DestroyRunInfo();

	CDialog::OnDestroy();
	
	// TODO: �ڴ˴������Ϣ����������
	GTcpSvr_Stop();
}

void CGTcpServerDemoDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;

	str.Format(_T("%d"), m_strListenPort);
	unsigned int unListen = GTcpSvr_OpenListen(m_strHostIP.GetBuffer(),		 //����������IP��ַ
												str.GetBuffer(),			 //�����������˿�
												&GTcpSvr_OnConnected,			 //���ӻص�������ַ�����пͻ����Ӳ��յ��ͻ��ĵ�һ������
																			 //֮��Ļص������Ը�ÿ���������ò�ͬ�Ļص�����
												(PGFN_ON_RECEIVED)&GTcpSvr_OnReceived,	//�յ�����֮��Ļص�������ַ�����Ը�ÿ����������
																						//��ͬ�Ļص�����
												(PGFN_ON_SENDED)&GTcpSvr_OnSended,		//�ͻ��쳣�Ͽ�֮��Ļص�������ַ��
																						//���Ը�ÿ���������ò�ͬ�Ļص�����
												(PGFN_ON_DISCONNECTED)&GTcpSvr_OnDisconnected,  //���г�ʱ֮��Ļص�������ַ��
																							//���Ը�ÿ���������ò�ͬ�Ļص�����
												this);										//��������������������ÿ���������ò�ͬ��������
																							//�����ڿͻ��¼���ʹ��GTcpSvr_GetListenOwner���
	if(unListen)
	{
		CString str;
		str.Format(_T("Listen(%d)"), GTcpSvr_GetListenCount());
		m_btnListen.SetWindowText(str.GetBuffer());
		GTcpSvrInfo_AddListen(unListen, m_strHostIP.GetBuffer(), m_strListenPort);
		m_strListenPort++;
		UpdateData(FALSE);
	}else
		MessageBox(_T("����ʧ�ܣ���ȷ��IP�Ͷ˿��Ƿ���ȷ�����Ѿ���ռ�ã������������־�ļ���"));
}

void CGTcpServerDemoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	#ifdef _UNICODE
	char pBuf[1024];
	WideCharToMultiByte(CP_ACP, 0, m_strBroad.GetBuffer(), -1, pBuf, 1024, NULL, NULL);
	GTcpSvr_PostBroadcast(pBuf, strlen(pBuf) + 1);
	#else
	GTcpSvr_PostBroadcast(m_strBroad.GetBuffer(), m_strBroad.GetLength() + 1);
	#endif
}

void CGTcpServerDemoDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GTcpSvr_ShowRunInfo();
}
void CGTcpServerDemoDlg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}
