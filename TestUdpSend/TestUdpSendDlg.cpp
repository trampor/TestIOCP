
// TestUdpSendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestUdpSend.h"
#include "TestUdpSendDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestUdpSendDlg 对话框



CTestUdpSendDlg::CTestUdpSendDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestUdpSendDlg::IDD, pParent)
	, m_bBitrate(FALSE)
	, m_nBitrate(0)
	, m_sDestIp(_T(""))
	, m_nPort(0)
	, m_nNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestUdpSendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LOCALIP, m_cLocalInterface);
	DDX_Control(pDX, IDC_EDIT_FILE, m_cFilePath);
	DDX_Radio(pDX, IDC_RADIO_PCR, m_bBitrate);
	DDX_Text(pDX, IDC_EDIT_BITRATE, m_nBitrate);
	DDX_Text(pDX, IDC_EDIT_DESTIP, m_sDestIp);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_NUM, m_nNum);
}

BEGIN_MESSAGE_MAP(CTestUdpSendDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CTestUdpSendDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTestUdpSendDlg::OnBnClickedButtonAdd)
END_MESSAGE_MAP()


// CTestUdpSendDlg 消息处理程序

BOOL CTestUdpSendDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	IP_ADAPTER_INFO buffer[20];
	PIP_ADAPTER_INFO pCurAdapt;
	PIP_ADDR_STRING pCurIpAddr;
	ULONG len = sizeof(IP_ADAPTER_INFO)* 20;
	DWORD res = GetAdaptersInfo(buffer, &len);	// fill buffer by structures
	if (res == ERROR_SUCCESS) {
		TCHAR buf[200];
		pCurAdapt = &buffer[0];
		do{
			if (pCurAdapt->AdapterName[0] == '\0' && pCurAdapt->Description[0] == '\0')
				break;

			pCurIpAddr = &(pCurAdapt->IpAddressList);
			do{
				if (pCurIpAddr->IpAddress.String[0] == '\0')
					break;

				int wnum = MultiByteToWideChar(CP_ACP, 0, pCurIpAddr->IpAddress.String, -1, buf, 200);
				if (wnum > 0 && (wcscmp(buf, _T("0.0.0.0")) != 0))
					m_cLocalInterface.AddString(buf);
			} while ((pCurIpAddr = pCurIpAddr->Next) != NULL);

		} while ((pCurAdapt = pCurAdapt->Next) != NULL);
		if (m_cLocalInterface.GetCount() > 0)
			m_cLocalInterface.SetCurSel(0);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestUdpSendDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestUdpSendDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestUdpSendDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestUdpSendDlg::OnBnClickedButtonBrowse()
{
	CString str;
	str = _T("ts Files (*.ts)|*.ts|transport file (*.trp)|*.trp|All Files (*.*)|*.*||");
	CFileDialog filedlg(true, _T(".ts"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_EXPLORER, str);
	int result = filedlg.DoModal();
	if (result == IDOK)
	{
		m_cFilePath.SetWindowText(filedlg.GetPathName());
	}
}


void CTestUdpSendDlg::OnBnClickedButtonAdd()
{
	// TODO:  在此添加控件通知处理程序代码
}
