
// TestUdpSendDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestUdpSend.h"
#include "TestUdpSendDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTestUdpSendDlg �Ի���



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


// CTestUdpSendDlg ��Ϣ�������

BOOL CTestUdpSendDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestUdpSendDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
