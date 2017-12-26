
// TestIOCPServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestIOCPServer.h"
#include "TestIOCPServerDlg.h"
#include "afxdialogex.h"
#include "MemCache.h"

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


// CTestIOCPServerDlg �Ի���



CTestIOCPServerDlg::CTestIOCPServerDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CTestIOCPServerDlg::IDD, pParent)
, m_nPort(4000)
, m_nThreadNums(0)
, m_bRunning(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestIOCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IPADDR, m_cLocalInterface);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 1, 65535);
	DDX_Text(pDX, IDC_EDIT_THREADNUM, m_nThreadNums);
	DDV_MinMaxUInt(pDX, m_nThreadNums, 0, 32);
}

BEGIN_MESSAGE_MAP(CTestIOCPServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CTRL, &CTestIOCPServerDlg::OnBnClickedButtonCtrl)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTestIOCPServerDlg ��Ϣ�������

BOOL CTestIOCPServerDlg::OnInitDialog()
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

	CMemCache::InitObj();

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

void CTestIOCPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestIOCPServerDlg::OnPaint()
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
HCURSOR CTestIOCPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CTestIOCPServerDlg::ClientStatusNotify(CTcpIOCPClient* pclient, int status_code)
{
	if (status_code == 0) //���������
	{
		unsigned char* pdata = CMemCache::GetMemBlock(4096);
		memset(pdata, 0, 4096);
		int ret = pclient->RecvData(-1,pdata, 4096);
		if (ret < 0)
		{
			m_pServer->CloseClient(pclient);
			CMemCache::ReleaseMemBlock(pdata, 4096);
		}
		SetDlgItemInt(IDC_EDIT_CONNECTS, m_pServer->GetClientNum());
	}
	else if (status_code == 1) //�ر�����
	{
		SetDlgItemInt(IDC_EDIT_CONNECTS, m_pServer->GetClientNum());
	}

	return 0;
}

int CTestIOCPServerDlg::OperateNotify(int opertype, CTcpIOCPClient* pclient, unsigned long seq_number, int ret_code, unsigned char* pdatabuf, int datalength)
{
	if (datalength <= 0)
	{
//		OutputDebugString(_T("client closed socket"));
		CMemCache::ReleaseMemBlock(pdatabuf, 4096);
		m_pServer->CloseClient(pclient);
		return 0;
	}

	if (opertype == 0) // ��֪ͨ
	{
		if (ret_code == 0) //���ճɹ�
		{
			CString tempstr;
			tempstr.Format(_T("recv client data: length=%u"), datalength);
//			OutputDebugString(tempstr);
//			OutputDebugStringA((char*)pdatabuf);

			strcat_s((char*)pdatabuf,4096, " server reply");
			int ret = pclient->SendData(-1,pdatabuf, datalength + 14);
			if (ret < 0)
			{
				m_pServer->CloseClient(pclient);
				CMemCache::ReleaseMemBlock(pdatabuf, 4096);
			}
//			OutputDebugString(_T("reply client data:"));
//			OutputDebugStringA((char*)pdatabuf);

			unsigned char* pdata = CMemCache::GetMemBlock(4096);
			memset(pdata, 0, 4096);
			ret = pclient->RecvData(-1, pdata, 4096);
			if (ret < 0)
			{
				m_pServer->CloseClient(pclient);
				CMemCache::ReleaseMemBlock(pdata, 4096);
			}
		}
		else
		{
			CString tempstr;
			tempstr.Format(_T("recv client error : errcode=%u"), ret_code);
//			OutputDebugString(tempstr);
			CMemCache::ReleaseMemBlock(pdatabuf, 4096);
			m_pServer->CloseClient(pclient);
		}
	}
	else if (opertype == 1) // ��֪ͨ
	{
		if (ret_code == 0) //���ɹ�
		{
//			OutputDebugString(_T("replay client data suc"));
		}
		else
		{
			CString tempstr;
			tempstr.Format(_T("send to client error : errcode=%u"), ret_code);
//			OutputDebugString(tempstr);
			m_pServer->CloseClient(pclient);
		}
		CMemCache::ReleaseMemBlock(pdatabuf, 4096);
	}
	else
	{
		OutputDebugString(_T("Error oper_type"));
	}
	return 0;
}

void CTestIOCPServerDlg::OnBnClickedButtonCtrl()
{
	UpdateData();
	if (!m_bRunning)
	{
		CString localip;
		m_cLocalInterface.GetWindowText(localip);
		m_pServer = CTcpIOCPServer::CreateServer(localip, m_nPort, m_nThreadNums, this);
		if (m_pServer != NULL)
		{
			m_pServer->StartService();
			m_bRunning = true;
			GetDlgItem(IDC_BUTTON_CTRL)->SetWindowText(_T("Stop"));
			SetDlgItemInt(IDC_EDIT_CONNECTS, 0);
		}
	}
	else
	{
		m_pServer->StopService();
		m_bRunning = false;
		GetDlgItem(IDC_BUTTON_CTRL)->SetWindowText(_T("Start"));
	}
}


void CTestIOCPServerDlg::OnClose()
{
	if (m_pServer != NULL)
		delete m_pServer;
	CMemCache::UninitObj();

	__super::OnClose();
}
