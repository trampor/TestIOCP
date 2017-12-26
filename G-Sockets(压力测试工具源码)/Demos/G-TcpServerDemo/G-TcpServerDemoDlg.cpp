// G-TcpServerDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "G-TcpServerDemo.h"
#include "G-TcpServerDemoDlg.h"

#include "..\..\G-TcpServer.h"
#include "..\G-TcpSvrRunInfo\G-TcpSvrRunInfo.h"

#ifdef _DEBUG
#define new DEBUG_unEW
#endif


// CGTcpServerDemoDlg 对话框

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


// CGTcpServerDemoDlg 消息处理程序
void SetListCtrlStyleDemo(CListCtrl* pLstCtrl)
{
	//获得原有风格
	DWORD dwStyle = GetWindowLong(pLstCtrl->m_hWnd, GWL_STYLE); 
	dwStyle &= ~(LVS_TYPEMASK);
	dwStyle &= ~(LVS_EDITLABELS);

	//设置新风格
    SetWindowLong(pLstCtrl->m_hWnd, GWL_STYLE, dwStyle | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS);

	//设置扩展风格
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(pLstCtrl->m_hWnd, styles, styles );
}

BOOL CGTcpServerDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_btnListen.EnableWindow(FALSE);
	m_btnBroad.EnableWindow(FALSE);

	CString str;
	wchar_t wt[256];

	this->GetWindowText(wt, 255);
	str.Format(_T("%s %s"), wt, GTcpSvr_GetVersionName());	
	this->SetWindowText(str);

	m_edtBlog.SetWindowTextW(_T("http://Blog.CSDN.Net/GuestCode   代码客   48092788@qq.com"));
	GTcpSvr_CreateRunInfo();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGTcpServerDemoDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(0, _T("open"), _T("http://Blog.CSDN.Net/GuestCode"), NULL, NULL, SW_SHOWNORMAL);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGTcpServerDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGTcpServerDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGTcpServerDemoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CGTcpServerDemoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CGTcpServerDemoDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
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
	
	// TODO:  在此添加控件通知处理程序代码
}

/********************************************************************************
				以下是 GTcpServer 事件回调函数
********************************************************************************/
void __stdcall GTcpSvr_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
//说明：线程开始和结束时在线程体内的回调函数，可以在这里设置线程权限和进行线程相关的初始化工作。
//输入：hThreadHandle：线程句柄
//		nThreadID：线程ID
//		bIsBegin：为TRUE表示线程刚开始运行，其他表示线程结束了。
//		unFlag：1：为数据处理者线程，2：为IO工作者线程，3：为看守线程，其他未定义。
//输出：无
//注意：线程权限设置可根据处理线程、工作者线程、伺服线程的角色来设置，相关线程内部需要的初始化工作，可以根据
//		unFlag为1和bIsBegin为真时进行初始化工作，比如：CoInitialize(nil)；unFlag为1和bIsBegin为假进行反初始化
//		工作，比如：CoUninitialize()。
{
	//线程刚开始运行
	if(bIsBegin)
	{
		GTcpSvrInfo_AddThread(unThreadContext, unThreadHandle, unThreadID);
		//是处理线程
		if(1 == unFlag)
		{
			//处理线程工作量比较大，应该高于工作线程
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
			//或SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//只有处理线程才会调用回调函数响应连接的所有事件，所有可能需要初始化线程，
			//以使用数据库操作等其他需要线程内独立的操作
			CoInitialize(NULL);
		}else
		//是工作线程
		if(2 == unFlag)
		{
			//工作线程权限介于看守线程和处理线程之间
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}else
		//是看守线程
		if(3 == unFlag)
		{
			//看守线程没有必要设置太高权限
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}
	}else
	{
		//GTcpSvrInfo_DelThread(gcThreadContext);
		//是处理线程
		if(1 == unFlag)
		{
			//反初始化
			CoUninitialize();
			//取消线程模式
			//或SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_END);
		}else
		//是工作线程
		if(2 == unFlag)
		{
			
		}else
		//是看守线程
		if(3 == unFlag)
		{
			
		}
	}
}

//原定义的回调函数的参数void* pOwenr强制转换为unsigned int unPerHandle，
//因为在OnConnect已经把Owner设置为unPerHandle。（下同）
void __stdcall GTcpSvr_OnReceived(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen)
{
	//以下是两种发送函数示例
	int i;
	CGTcpServerDemoDlg* pDlg = (CGTcpServerDemoDlg*)GTcpSvr_GetListenOwner(unPerHandle);

	i = pDlg->m_unLoop * 1000;
	while(i--);
	if(pDlg->m_bIsPing)
	{
		//发送1
		/*PGBUF pGBuf;

		pGBuf = GTcpSvr_AllocGBuf();
		if(pGBuf)
		{
			memcpy(pGBuf, pBuf, nLen);
			GTcpSvr_PostSendGBuf(unPerHandle, pGBuf, nLen);
		}*/
		//发送2
		GTcpSvr_PostSendBuf(unPerHandle, pBuf, unLen);
	}
}

void __stdcall GTcpSvr_OnConnected(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen)
{
	//懒得重复代码，调用接收函数了事 ：)
	GTcpSvr_OnReceived(unPerHandle, pBuf, unLen);
}

void __stdcall GTcpSvr_OnSended(unsigned int unPerHandle, unsigned int unSendID,  unsigned int unLen)
{

}

void __stdcall GTcpSvr_OnDisconnected(unsigned int unPerHandle, unsigned int unFlag)
{
	
}
/********************************************************************************
				以上是 GTcpServer 事件回调函数
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
	// TODO: 在此添加控件通知处理程序代码
	
	if(!GTcpSvr_IsActive())
	{
		GTcpSvr_Start(m_unMaxConnection,				//最大连接量，默认仅需修改最大连接量即可
						m_unHeartbeatTime,		//心跳超时的时长，必须在这个时长内收到客户端的心跳数据，否则被断开
						m_unMaxNetDelayTime,			//接受超时，客户端连接后，必须在这个时长内向服务器发送数据，否则被断开
						&GTcpSvr_OnThread,			//线程开始和结束的回调函数地址
						m_unMaxPostAccept,			//最大投递AcceptEx的数量
						m_unIoDataNumber,			//IoData的数量，系统会自适应扩充，不应低于MaxConnection * 1.5
						m_unProcesserThreadNumber,	//处理线程数，可以根据CPU数量设置，0时系统设置为CPUNumber
						m_unWorkerThreadNumber,		//工作线程数，可以根据CPU数量设置
						m_unGuardThreadSleepTime,	//看守线程休眠的时间，唤醒之后检测是否有接受超时或空闲超时
						m_unHndDataExNumber);		//HndData扩展量，HndDataCount = MaxConnection + HndDataExNumber，少于1000等于1000
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
			MessageBox(_T("G-TcpServer启动失败，更多请参阅日志文件！"));
	}
}

void CGTcpServerDemoDlg::OnDestroy()
{
	GTcpSvr_DestroyRunInfo();

	CDialog::OnDestroy();
	
	// TODO: 在此处添加消息处理程序代码
	GTcpSvr_Stop();
}

void CGTcpServerDemoDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;

	str.Format(_T("%d"), m_strListenPort);
	unsigned int unListen = GTcpSvr_OpenListen(m_strHostIP.GetBuffer(),		 //监听的主机IP地址
												str.GetBuffer(),			 //监听的主机端口
												&GTcpSvr_OnConnected,			 //连接回调函数地址，当有客户连接并收到客户的第一笔数据
																			 //之后的回调，可以给每个监听设置不同的回调函数
												(PGFN_ON_RECEIVED)&GTcpSvr_OnReceived,	//收到数据之后的回调函数地址，可以给每个监听设置
																						//不同的回调函数
												(PGFN_ON_SENDED)&GTcpSvr_OnSended,		//客户异常断开之后的回调函数地址，
																						//可以给每个监听设置不同的回调函数
												(PGFN_ON_DISCONNECTED)&GTcpSvr_OnDisconnected,  //空闲超时之后的回调函数地址，
																							//可以给每个监听设置不同的回调函数
												this);										//单个监听的宿主，可以每个监听设置不同的宿主，
																							//可以在客户事件里使用GTcpSvr_GetListenOwner获得
	if(unListen)
	{
		CString str;
		str.Format(_T("Listen(%d)"), GTcpSvr_GetListenCount());
		m_btnListen.SetWindowText(str.GetBuffer());
		GTcpSvrInfo_AddListen(unListen, m_strHostIP.GetBuffer(), m_strListenPort);
		m_strListenPort++;
		UpdateData(FALSE);
	}else
		MessageBox(_T("监听失败，请确认IP和端口是否正确或者已经被占用，更多请参阅日志文件！"));
}

void CGTcpServerDemoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	GTcpSvr_ShowRunInfo();
}
void CGTcpServerDemoDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
}
