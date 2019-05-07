
// TestUdpReceiverDlg.cpp : ʵ���ļ�
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


// CTestUdpReceiverDlg �Ի���



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

	for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
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

	for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
	{
		delete *iter;
	}
	m_TcpList.clear();

	for (auto iter = m_FileList.begin(); iter != m_FileList.end(); iter++)
	{
		delete *iter;
	}
	m_FileList.clear();

	CUdpReceiver::ReleaseAllList();
	CTcpListenerBase::ReleaseAllList();
	CTcpListener<CRtspServer>::ReleaseAllList();
	CTcpWorker::ReleaseAllList();
	CFileWriter::ReleaseAllList();
	CTcpConnecter::ReleaseAllList();

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
	DDX_Control(pDX, IDC_LIST_TCP, m_cTcpList);
	DDX_Control(pDX, IDC_LIST_TCPCLIENT, m_cClientList);
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
	ON_BN_CLICKED(IDC_BUTTON_TCPADD, &CTestUdpReceiverDlg::OnBnClickedButtonTcpadd)
	ON_BN_CLICKED(IDC_BUTTON_TCPSTOP, &CTestUdpReceiverDlg::OnBnClickedButtonTcpstop)
	ON_BN_CLICKED(IDC_BUTTON_TCPSTART, &CTestUdpReceiverDlg::OnBnClickedButtonTcpstart)
	ON_BN_CLICKED(IDC_BUTTON_TCPDEL, &CTestUdpReceiverDlg::OnBnClickedButtonTcpdel)
	ON_BN_CLICKED(IDC_BUTTON_CLIENTADD, &CTestUdpReceiverDlg::OnBnClickedButtonClientadd)
	ON_BN_CLICKED(IDC_BUTTON_STARTALLCLIENT, &CTestUdpReceiverDlg::OnBnClickedButtonStartallclient)
	ON_BN_CLICKED(IDC_BUTTON_STOPALLCLIENT, &CTestUdpReceiverDlg::OnBnClickedButtonStopallclient)
	ON_BN_CLICKED(IDC_BUTTON_DELETEALLCLIENT, &CTestUdpReceiverDlg::OnBnClickedButtonDeleteallclient)
END_MESSAGE_MAP()


// CTestUdpReceiverDlg ��Ϣ�������

BOOL CTestUdpReceiverDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetDlgItemText(IDC_EDIT_LOCALIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_SOURCEIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_MULTIIP, _T("224.224.224.224"));
	SetDlgItemText(IDC_EDIT_MULTIPORT, _T("2424"));
	SetDlgItemText(IDC_EDIT_FILEPATH, _T("e:\\temp.ts"));


	SetDlgItemText(IDC_EDIT_TCPIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_TCPPORT, _T("3333"));

	SetDlgItemText(IDC_EDIT_SERVERIP, _T("192.168.1.99"));
	SetDlgItemText(IDC_EDIT_SERVERPORT, _T("3333"));
	SetDlgItemText(IDC_EDIT_CLIENTNUM, _T("1"));


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

	m_cTcpList.SetImageList(&m_itemImageList, LVSIL_SMALL);
	m_cTcpList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_cTcpList.InsertColumn(0, _T(""), LVCFMT_CENTER, 0);
	m_cTcpList.InsertColumn(1, _T("Host"), LVCFMT_CENTER, 150);
	m_cTcpList.InsertColumn(2, _T("Link Num"), LVCFMT_CENTER, 150);
	m_cTcpList.InsertColumn(3, _T("Status"), LVCFMT_CENTER, 150);

	m_cClientList.SetImageList(&m_itemImageList, LVSIL_SMALL);
	m_cClientList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_cClientList.InsertColumn(0, _T(""), LVCFMT_CENTER, 0);
	m_cClientList.InsertColumn(1, _T("Server"), LVCFMT_CENTER, 120);
	m_cClientList.InsertColumn(2, _T("Client"), LVCFMT_CENTER, 150);
	m_cClientList.InsertColumn(3, _T("Status"), LVCFMT_CENTER, 80);
	m_cClientList.InsertColumn(4, _T("ConnectTime"), LVCFMT_CENTER, 90);
	m_cClientList.InsertColumn(5, _T("Recv"), LVCFMT_CENTER, 80);
	m_cClientList.InsertColumn(6, _T("Recved"), LVCFMT_CENTER, 80);
	m_cClientList.InsertColumn(7, _T("Write"), LVCFMT_CENTER, 80);
	m_cClientList.InsertColumn(8, _T("Writed"), LVCFMT_CENTER, 80);
	m_cClientList.InsertColumn(9, _T("error"), LVCFMT_CENTER, 80);

	m_CalcCenter.InitObj(0);

	m_nLastTickCount = 0;

	SetTimer(1,1000, NULL);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestUdpReceiverDlg::OnPaint()
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

		index = 0;
		for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
		{
			_itow_s((*iter)->GetPacketNum(), temp, 10);
			m_cTcpList.SetItemText(index, 2, temp);
			_itow_s((*iter)->GetStatus(), temp, 10);
			m_cTcpList.SetItemText(index, 3, temp);
			index++;
		}

		index = 0;
		unsigned char datatemp[1024];
		int recv, recved, write, writed, erro;
		for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
		{
			if ((*iter)->IsConnect())
				(*iter)->Write(datatemp, 50);
			{
				m_cClientList.SetItemText(index, 2, (*iter)->GetLocalAddr());
				_itow_s((*iter)->GetStatus(), temp, 10);
				m_cClientList.SetItemText(index, 3, temp);
				_itow_s((*iter)->GetConnectTime(), temp, 10);
				m_cClientList.SetItemText(index, 4, temp);
				(*iter)->GetRecvWriteNum(recv, recved, write, writed, erro);
				_itow_s(recv, temp, 10);
				m_cClientList.SetItemText(index, 5, temp);
				_itow_s(recved, temp, 10);
				m_cClientList.SetItemText(index, 6, temp);
				_itow_s(write, temp, 10);
				m_cClientList.SetItemText(index, 7, temp);
				_itow_s(writed, temp, 10);
				m_cClientList.SetItemText(index, 8, temp);
				_itow_s(erro, temp, 10);
				m_cClientList.SetItemText(index, 9, temp);
			}
			index++;
		}
		m_nLastTickCount = curtickcount;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTestUdpReceiverDlg::OnBnClickedButtonTcpadd()
{
	TCHAR localip[256], temp[10];
	int port;

	GetDlgItemText(IDC_EDIT_TCPIP, localip, 256);
	port = GetDlgItemInt(IDC_EDIT_TCPPORT);

	CTcpListener<CRtspServer> *precv = new CTcpListener<CRtspServer>;
	int result = precv->InitObject(localip, port);
	if (result < 0)
		return;

	int index = m_cTcpList.InsertItem(m_cTcpList.GetItemCount(), NULL);
	_itow_s(port, temp, 10);
	CString tempstr = localip;
	tempstr += _T(":");
	tempstr += temp;
	m_cTcpList.SetItemText(index, 1, tempstr);

	m_TcpList.push_back(precv);

	m_cTcpList.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
}

void CTestUdpReceiverDlg::OnBnClickedButtonTcpstart()
{
	POSITION pos = m_cTcpList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cTcpList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
		{
			if (index == cursel)
			{
				if ((*iter)->Start(&m_CalcCenter) < 0)
				{
					OutputDebugString(_T("tcp start fail"));
					(*iter)->Stop();
				}
				break;
			}
			index++;
		}
		break;
	}
}

void CTestUdpReceiverDlg::OnBnClickedButtonTcpstop()
{
	POSITION pos = m_cTcpList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cTcpList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
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



void CTestUdpReceiverDlg::OnBnClickedButtonTcpdel()
{
	POSITION pos = m_cTcpList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cTcpList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_TcpList.begin(); iter != m_TcpList.end(); iter++)
		{
			if (index == cursel)
			{
				(*iter)->Stop();
				(*iter)->UninitObject();
				delete *iter;
				m_TcpList.erase(iter);
				m_cTcpList.DeleteItem(cursel);
				break;
			}
			index++;
		}
	}
}

void CTestUdpReceiverDlg::OnBnClickedButtonClientadd()
{
	TCHAR localip[256], temp[10];
	int port,clientnum;

	GetDlgItemText(IDC_EDIT_SERVERIP, localip, 256);
	port = GetDlgItemInt(IDC_EDIT_SERVERPORT);
	clientnum = GetDlgItemInt(IDC_EDIT_CLIENTNUM);

	int res;
	CTcpConnecter *pcleint ;
	for (int i = 0; i < clientnum;i++)
	{
		pcleint = new CTcpConnecter;
		res = pcleint->InitObject(localip, port, NULL);
		if (res < 0)
		{
			pcleint->UninitObject();
			delete pcleint;
			continue;
		}
		m_ClientList.push_back(pcleint);
		int index = m_cClientList.InsertItem(m_cClientList.GetItemCount(), NULL);
		_itow_s(port, temp, 10);
		CString tempstr = localip;
		tempstr += _T(":");
		tempstr += temp;
		m_cClientList.SetItemText(index, 1, tempstr);
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonClientstart()
{
	POSITION pos = m_cClientList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cClientList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
		{
			if (index == cursel)
			{
				if ((*iter)->Start() < 0)
				{
					OutputDebugString(_T("client start fail"));
					(*iter)->Stop();
				}
				break;
			}
			index++;
		}
		break;
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonClientstop()
{
	POSITION pos = m_cClientList.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		int cursel = m_cClientList.GetNextSelectedItem(pos);
		int index = 0;
		for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
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


void CTestUdpReceiverDlg::OnBnClickedButtonClientdel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CTestUdpReceiverDlg::OnBnClickedButtonStartallclient()
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
	{
		if ((*iter)->Start() < 0)
		{
			OutputDebugString(_T("client start fail"));
			(*iter)->Stop();
		}
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonStopallclient()
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
	{
		(*iter)->Stop();
	}
}


void CTestUdpReceiverDlg::OnBnClickedButtonDeleteallclient()
{
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
	{
		(*iter)->Stop();
	}
	Sleep(1000);
	for (auto iter = m_ClientList.begin(); iter != m_ClientList.end(); iter++)
	{
		delete (*iter);
	}
	m_ClientList.clear();

	m_cClientList.DeleteAllItems();
}
