
// TestUdpReceiverDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestUdpReceiver.h"
#include "TestUdpReceiverDlg.h"
#include "afxdialogex.h"
#include <map>
using namespace std;


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


// CTestUdpReceiverDlg 对话框



CTestUdpReceiverDlg::CTestUdpReceiverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestUdpReceiverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_itemImageList.Create(2, 18, ILC_COLOR4, 0, 1);
}

CTestUdpReceiverDlg::~CTestUdpReceiverDlg()
{
	for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
	{
		(*iter)->UninitObject();
	}

	for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++)
	{
		(*iter)->UninitObject();
	}

	m_CalcCenter.UninitObject();

	for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
	{
		delete *iter;
	}
	m_RecvList.clear();

	for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++)
	{
		delete *iter;
	}
	m_FileList.clear();

	CUdpReceiver::ReleaseAllList();
	CFileWriter::ReleaseAllList();

	//m_brun = false;
	//WaitForMultipleObjects(THREADNUM, m_hThreads, true, INFINITE);
	//for (int i = 0; i < THREADNUM; i++)
	//{
	//	CloseHandle(m_hThreads[i]);
	//}

	//map<int, int> elemap;
	//int count = 0,*pval;
	//CNoLockBiListNode<int> *ptempnode;
	//for (int i = 0; i < 10000; i++)
	//{
	//	pval = templist.PopHead();
	//	if (pval != NULL)
	//	{
	//		elemap[(int)pval] = 1;
	//		count++;
	//	}
	//}
	//CString tempstr;
	//tempstr.Format(_T("recv count %d"), elemap.size());
	//OutputDebugString(tempstr);
}

void CTestUdpReceiverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECVLIST, m_cRecvList);
}

BEGIN_MESSAGE_MAP(CTestUdpReceiverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CTestUdpReceiverDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CTestUdpReceiverDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTestUdpReceiverDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestUdpReceiverDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_STARTALL, &CTestUdpReceiverDlg::OnBnClickedButtonStartall)
	ON_BN_CLICKED(IDC_BUTTON_STOPALL, &CTestUdpReceiverDlg::OnBnClickedButtonStopall)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTestUdpReceiverDlg 消息处理程序

BOOL CTestUdpReceiverDlg::OnInitDialog()
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

	//CNoLockBiListNode<int> *ptempnode;
	//for (int i = 1; i < 10001; i++)
	//{
	//	templist.PushTail(i);
	//}

	//m_brun = true;
	//for (int i = 0; i < THREADNUM; i++)
	//{
	//	m_hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, InitTestThread, this, 0, &m_ThreadData);
	//}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	SetDlgItemText(IDC_EDIT_LOCALIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_SOURCEIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_MULTIIP, _T("224.224.224.224"));
	SetDlgItemText(IDC_EDIT_MULTIPORT, _T("2424"));
	SetDlgItemText(IDC_EDIT_FILEPATH, _T("e:\\temp.ts"));

	m_cRecvList.SetImageList(&m_itemImageList, LVSIL_SMALL);
	m_cRecvList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_cRecvList.InsertColumn(0, _T(""), LVCFMT_CENTER, 0);
	m_cRecvList.InsertColumn(1, _T("Source"), LVCFMT_CENTER, 110);
	m_cRecvList.InsertColumn(2, _T("Multicast Group"), LVCFMT_CENTER, 150);
	m_cRecvList.InsertColumn(3, _T("NIC"), LVCFMT_CENTER, 120);
	m_cRecvList.InsertColumn(4, _T("Packets"), LVCFMT_CENTER, 75);
	m_cRecvList.InsertColumn(5, _T("Bitrate"), LVCFMT_CENTER, 100);
	m_cRecvList.InsertColumn(6, _T("Status"), LVCFMT_CENTER, 60);
	m_cRecvList.InsertColumn(7, _T("File"), LVCFMT_CENTER, 120);
	m_cRecvList.InsertColumn(8, _T("FileRecv"), LVCFMT_CENTER, 80);
	m_cRecvList.InsertColumn(9, _T("FileWrite"), LVCFMT_CENTER, 80);

	m_CalcCenter.InitObj(0);

	m_nLastTickCount = 0;

	SetTimer(1,1000, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
unsigned WINAPI CTestUdpReceiverDlg::InitTestThread(LPVOID pv)
{
	CTestUdpReceiverDlg* lpInst = (CTestUdpReceiverDlg*)pv;
	return lpInst->TestThread();
}

unsigned CTestUdpReceiverDlg::TestThread()
{
	int count = 0,*pval;
	CNoLockBiListNode<int> *ptempnode;
	for (; m_brun; )
	{
		pval = templist.PopHead();
		if (pval != NULL)
		{
			templist.PushTail((int)pval);
			count++;
		}
		else
			OutputDebugString(_T("read null node\n"));
		Sleep(1);
	}

	CString tempstr;
	tempstr.Format(_T("getnodenum %d\n"),count);
	OutputDebugString(tempstr);

	return 0;
}


void CTestUdpReceiverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestUdpReceiverDlg::OnPaint()
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
HCURSOR CTestUdpReceiverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestUdpReceiverDlg::OnBnClickedButtonAdd()
{
	TCHAR localip[256], sourceip[256], multiip[256], filepath[256], temp[20];
	int port;

	GetDlgItemText(IDC_EDIT_LOCALIP, localip, 256);
	GetDlgItemText(IDC_EDIT_SOURCEIP, sourceip, 256);
	GetDlgItemText(IDC_EDIT_MULTIIP, multiip, 256);
	GetDlgItemText(IDC_EDIT_FILEPATH, filepath, 256);
	port = GetDlgItemInt(IDC_EDIT_MULTIPORT);

	CUdpReceiver *precv = new CUdpReceiver;
	int result = precv->InitObject(localip, sourceip, multiip, port);
	if (result < 0)
		return;

	CFileWriter *pfilewrier = new CFileWriter;
	result = pfilewrier->InitObject(filepath);

	int index = m_cRecvList.InsertItem(m_cRecvList.GetItemCount(), NULL);
	m_cRecvList.SetItemText(index, 1, sourceip);
	_itow_s(port, temp, 10);
	CString tempstr = multiip;
	tempstr += _T(":");
	tempstr += temp;
	m_cRecvList.SetItemText(index, 2, tempstr);

	m_cRecvList.SetItemText(index, 3, localip);
	_itow_s(precv->GetPacketNum(), temp, 10);
	m_cRecvList.SetItemText(index, 4, temp);
	_itow_s(precv->GetStatus(), temp, 10);
	m_cRecvList.SetItemText(index, 5, temp);
	_itow_s(precv->GetStatus(), temp, 10);
	m_cRecvList.SetItemText(index, 6, temp);
	m_cRecvList.SetItemText(index, 7, filepath);
	m_cRecvList.SetItemText(index, 8, temp);
	m_cRecvList.SetItemText(index, 9, temp);

	m_FileList.push_back(pfilewrier);
	m_RecvList.push_back(precv);

	precv->AddDataObserver(pfilewrier);
	if (pfilewrier->GetFileHandle() != INVALID_HANDLE_VALUE)
	{
		BOOL bsuc = m_CalcCenter.AssociateDevice(pfilewrier->GetFileHandle(), CK_FILE_WRITE);
	}

	m_cRecvList.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}


void CTestUdpReceiverDlg::OnBnClickedButtonDel()
{
	POSITION pos = m_cRecvList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cRecvList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
		{
			if (index == cursel)
			{
				(*iter)->Stop();
				(*iter)->UninitObject();
				delete *iter;
				m_RecvList.erase(iter);
				m_cRecvList.DeleteItem(cursel);
				break;
			}
			index++;
		}
		index = 0;
		for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++)
		{
			if (index == cursel)
			{
				(*iter)->UninitObject();
				delete *iter;
				m_FileList.erase(iter);
				break;
			}
			index++;
		}
		break;
	}
}

void CTestUdpReceiverDlg::OnBnClickedButtonStart()
{
	POSITION pos = m_cRecvList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cRecvList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
		{
			if (index == cursel)
			{
				if ((*iter)->Start(&m_CalcCenter) < 0)
				{
					OutputDebugString(_T("receiver start fail"));
					(*iter)->Stop();
				}
				break;
			}
			index++;
		}
		break;
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonStop()
{
	POSITION pos = m_cRecvList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cRecvList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
		{
			if (index == cursel)
			{
				(*iter)->Stop();
				break;
			}
			index++;
		}
		break;
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonStartall()
{
	for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
	{
		if ((*iter)->Start(&m_CalcCenter) < 0)
		{
			OutputDebugString(_T("receiver start fail"));
			(*iter)->Stop();
		}
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonStopall()
{
	for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
	{
		(*iter)->Stop();
	}
}


void CTestUdpReceiverDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		unsigned long curtickcount = GetTickCount();

		TCHAR temp[20];
		int index = 0;
		for (auto iter = m_RecvList.begin(); iter != m_RecvList.end(); iter++)
		{
			_itow_s((*iter)->GetPacketNum(), temp, 10);
			m_cRecvList.SetItemText(index, 4, temp);
			_itow_s((*iter)->GetRecvBytesNum()*8*(double)1000 / (curtickcount - m_nLastTickCount), temp, 10);
			m_cRecvList.SetItemText(index, 5, temp);
			_itow_s((*iter)->GetStatus(), temp, 10);
			m_cRecvList.SetItemText(index, 6, temp);
			index++;
		}

		index = 0;
		for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++)
		{
			_itow_s((*iter)->GetRecvPacketNum(), temp, 10);
			m_cRecvList.SetItemText(index, 8, temp);
			_itow_s((*iter)->GetWritePacketNum(), temp, 10);
			m_cRecvList.SetItemText(index, 9, temp);
			index++;
		}

		m_nLastTickCount = curtickcount;
	}

	CDialogEx::OnTimer(nIDEvent);
}
