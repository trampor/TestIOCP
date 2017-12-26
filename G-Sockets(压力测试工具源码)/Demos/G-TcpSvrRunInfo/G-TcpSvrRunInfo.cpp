// GTcpSvrRunInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "G-TcpSvrRunInfo.h"


// GTcpSvrRunInfo 对话框

CRITICAL_SECTION GTcpSvrInfo_cs;

GTcpSvrRunInfo *pGTcpSvrRunInfo = NULL;

void __stdcall GTcpSvr_CreateRunInfo(void)
{
	if(NULL == pGTcpSvrRunInfo)
	{
		InitializeCriticalSection(&GTcpSvrInfo_cs);
		pGTcpSvrRunInfo = new GTcpSvrRunInfo;
		pGTcpSvrRunInfo->Create(IDD_GTCPSVR_RUNINFO, NULL);
	}
}

void __stdcall GTcpSvr_DestroyRunInfo(void)
{
	if(pGTcpSvrRunInfo)
	{
		delete pGTcpSvrRunInfo;
		pGTcpSvrRunInfo = NULL;
	}
}

void __stdcall GTcpSvr_ShowRunInfo(void)
{
	if(pGTcpSvrRunInfo)
	{
		pGTcpSvrRunInfo->ShowWindow(SW_SHOW);
	}
}

void __stdcall GTcpSvr_HideRunInfo(void)
{
	if(pGTcpSvrRunInfo)
	{
		pGTcpSvrRunInfo->ShowWindow(SW_HIDE);
	}
}

void __stdcall GTcpSvr_ClearRunInfo(void)
{
	if(pGTcpSvrRunInfo)
	{
		pGTcpSvrRunInfo->m_lvListen.DeleteAllItems();
		pGTcpSvrRunInfo->m_lvThread.DeleteAllItems();
		pGTcpSvrRunInfo->UpdateWindow();
	}
}

void __stdcall GTcpSvrInfo_AddListen(unsigned int unListen, wchar_t* pstrIP, unsigned int unPort)
{
	if(!pGTcpSvrRunInfo)
		return;

	LV_ITEM lvitem;
	CString str;
	int nIndex;

	nIndex = pGTcpSvrRunInfo->m_lvListen.GetItemCount();
	if(0 > nIndex)
		nIndex = 0;
	lvitem.state = 0;
	lvitem.stateMask = 0;

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 0;
	str.Format(_T("%d"), nIndex + 1);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvListen.InsertItem(&lvitem);

	pGTcpSvrRunInfo->m_lvListen.SetItemData(nIndex, (DWORD_PTR)unListen);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 1;
	str.Format(_T("%s"), pstrIP);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvListen.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 2;
	str.Format(_T("%d"), unPort);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvListen.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	str.Format(_T("%d"), 0);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvListen.SetItem(&lvitem);
}

void __stdcall GTcpSvrInfo_DelListen(unsigned int unListen)
{
	if(!pGTcpSvrRunInfo)
		return;

	LV_ITEM lvitem;
	int i, j, nEnd;
	CString str;

	nEnd = pGTcpSvrRunInfo->m_lvListen.GetItemCount();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	for(i = 0; i < nEnd; i++)
	{
		if((unsigned int)pGTcpSvrRunInfo->m_lvListen.GetItemData(i) == unListen)
		{
			pGTcpSvrRunInfo->m_lvListen.DeleteItem(i);
			nEnd = pGTcpSvrRunInfo->m_lvListen.GetItemCount();
			for(j = i; j < nEnd; j++)
			{
				lvitem.iItem = j;
				lvitem.iSubItem = 0;
				str.Format(_T("%d"), j + 1);
				lvitem.pszText = (LPTSTR)(LPCTSTR)str;
				pGTcpSvrRunInfo->m_lvListen.SetItem(&lvitem);
				return;
			}
		}
	}
}

void __stdcall GTcpSvrInfo_AddThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID)
{
	if(!pGTcpSvrRunInfo)
		return;

	EnterCriticalSection(&GTcpSvrInfo_cs);
	LV_ITEM lvitem;
	CString str;
	int nIndex;

	nIndex = pGTcpSvrRunInfo->m_lvThread.GetItemCount();
	if(0 > nIndex)
		nIndex = 0;
	lvitem.state = 0;
	lvitem.stateMask = 0;

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 0;
	str.Format(_T("%d"), nIndex + 1);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvThread.InsertItem(&lvitem);

	pGTcpSvrRunInfo->m_lvThread.SetItemData(nIndex, (DWORD_PTR)unThreadContext);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 1;
	str.Format(_T("%s"), GTcpSvr_GetThreadName(unThreadContext));
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvThread.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 2;
	str.Format(_T("%d"), 0);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvThread.SetItem(&lvitem);

	/*lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	str.Format(_T("%d"), hThread);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvThread.SetItem(&lvitem);*/

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	str.Format(_T("%d"), unThreadID);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpSvrRunInfo->m_lvThread.SetItem(&lvitem);
	LeaveCriticalSection(&GTcpSvrInfo_cs);
}

void __stdcall GTcpSvrInfo_DelThread(unsigned int unThread)
{
	if(!pGTcpSvrRunInfo)
		return;

	EnterCriticalSection(&GTcpSvrInfo_cs);
	LV_ITEM lvitem;
	int i, j, nEnd;
	CString str;

	nEnd = pGTcpSvrRunInfo->m_lvThread.GetItemCount();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	for(i = 0; i < nEnd; i++)
	{
		if((unsigned int)pGTcpSvrRunInfo->m_lvThread.GetItemData(i) == unThread)
		{
			pGTcpSvrRunInfo->m_lvThread.DeleteItem(i);
			nEnd = pGTcpSvrRunInfo->m_lvThread.GetItemCount();
			for(j = i; j < nEnd; j++)
			{
				lvitem.iItem = j;
				lvitem.iSubItem = 0;
				str.Format(_T("%d"), j + 1);
				lvitem.pszText = (LPTSTR)(LPCTSTR)str;
				pGTcpSvrRunInfo->m_lvThread.SetItem(&lvitem);				
			}
			LeaveCriticalSection(&GTcpSvrInfo_cs);
			return;
		}
	}
	LeaveCriticalSection(&GTcpSvrInfo_cs);
}

IMPLEMENT_DYNAMIC(GTcpSvrRunInfo, CDialog)

GTcpSvrRunInfo::GTcpSvrRunInfo(CWnd* pParent /*=NULL*/)
	: CDialog(GTcpSvrRunInfo::IDD, pParent)
	, m_unCPUNumber(0)
	, m_unPageSize(0)
	, m_unPhyMemSize(0)
	, m_unFreePhyMemSize(0)
	, m_strPhyMemFreeRate(_T("0.00"))

	, m_unIoDataUseMem(0)
	, m_unGBUFSize(0)
	, m_unIoDataTotal(0)
	, m_unIoDataUse(0)
	, m_strIoDataUseRate(_T("0.00"))

	, m_unHndDataUseMem(0)
	, m_unHndDataSize(0)
	, m_unHndDataTotal(0)
	, m_unHndDataUse(0)
	, m_strHndDataUseRate(_T("0.00"))

	, m_unServerUseMem(0)
	, m_unPendingAcceptCount(0)
	, m_bMaxConnection(0)
	, m_unClientCount(0)
	, m_strConnectRate(_T("0.00"))

	, m_unListenCount(0)
	, m_unThreadNumber(0)
{
	
}

GTcpSvrRunInfo::~GTcpSvrRunInfo()
{
	
}

void GTcpSvrRunInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_CPUSL, m_unCPUNumber);
	DDX_Text(pDX, IDC_YDX, m_unPageSize);
	DDX_Text(pDX, IDC_WLNC, m_unPhyMemSize);
	DDX_Text(pDX, IDC_KXNC, m_unFreePhyMemSize);
	DDX_Text(pDX, IDC_KXL, m_strPhyMemFreeRate);	

	DDX_Text(pDX, IDC_IDNC, m_unIoDataUseMem);
	DDX_Text(pDX, IDC_GBUFDX, m_unGBUFSize);
	DDX_Text(pDX, IDC_IDZL, m_unIoDataTotal);
	DDX_Text(pDX, IDC_IDSY, m_unIoDataUse);
	DDX_Text(pDX, IDC_IDSYL, m_strIoDataUseRate);

	DDX_Text(pDX, IDC_HDNC, m_unHndDataUseMem);
	DDX_Text(pDX, IDC_HDDX, m_unHndDataSize);
	DDX_Text(pDX, IDC_HDZL, m_unHndDataTotal);
	DDX_Text(pDX, IDC_HDSY, m_unHndDataUse);
	DDX_Text(pDX, IDC_HDSYL, m_strHndDataUseRate);

	DDX_Text(pDX, IDC_FWNC, m_unServerUseMem);
	DDX_Text(pDX, IDC_WJJS, m_unPendingAcceptCount);
	DDX_Text(pDX, IDC_ZDLJ, m_bMaxConnection);
	DDX_Text(pDX, IDC_LJL, m_unClientCount);
	DDX_Text(pDX, IDC_LJL_, m_strConnectRate);	

	DDX_Text(pDX, IDC_JTSL, m_unListenCount);
	DDX_Text(pDX, IDC_XCSL, m_unThreadNumber);

	DDX_Control(pDX, IDC_LVJT, m_lvListen);
	DDX_Control(pDX, IDC_LVXC, m_lvThread);
}


BEGIN_MESSAGE_MAP(GTcpSvrRunInfo, CDialog)
	ON_BN_CLICKED(IDOK, &GTcpSvrRunInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GTcpSvrRunInfo::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// GTcpSvrRunInfo 消息处理程序

void GTcpSvrRunInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void GTcpSvrRunInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void GTcpSvrRunInfo::SetListCtrlStyleDemo(CListCtrl* pLstCtrl)
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

BOOL GTcpSvrRunInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 在此添加控件通知处理程序代码
	SetListCtrlStyleDemo(&m_lvListen);
	SetListCtrlStyleDemo(&m_lvThread);

	LV_COLUMN col;

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Inder");
	col.cx = 25;
	m_lvListen.InsertColumn(0, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Host IP");
	col.cx = 95;
	m_lvListen.InsertColumn(1, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Port");
	col.cx = 40;
	m_lvListen.InsertColumn(2, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Client");
	col.cx = 45;
	m_lvListen.InsertColumn(3, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Inder");
	col.cx = 25;
	m_lvThread.InsertColumn(0, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Name");
	col.cx = 60;
	m_lvThread.InsertColumn(1, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Active");
	col.cx = 60;
	m_lvThread.InsertColumn(2, &col);

	/*col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("线程句柄");
	col.cx = 60;
	m_lvThread.InsertColumn(3, &col);*/

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("ID");
	col.cx = 50;
	m_lvThread.InsertColumn(3, &col);

	CString str;
	wchar_t wt[256];

	this->GetWindowText(wt, 255);
	//版本名带T的表示试用版，最大连接不能超过100，发送字节数不能超过128，不能设置0读投递，并有其他更能限制
	//版本名带bata表示非正式版
	//版本名带WChar表示支持WideChar
	//版本号以时间格式累加和显示，
	str.Format(_T("%s %s"), wt, GTcpSvr_GetVersionName());	
	this->SetWindowText(str);

	SetTimer(10000, 1000, NULL);
	return TRUE;
}
void GTcpSvrRunInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_unFreePhyMemSize = GTcpSvr_GetPhyMemInfo(&m_unPhyMemSize) / (1024 * 1024);
	m_unPhyMemSize /= (1024 * 1024);
	m_strPhyMemFreeRate.Format(_T("%0.2f"), (double)m_unFreePhyMemSize / (double)m_unPhyMemSize * (double)100);
	m_unPageSize = GTcpSvr_GetPageSize();
	m_unCPUNumber = GTcpSvr_GetProcesserNumber();

	m_unIoDataUseMem = GTcpSvr_GetIoDataUseMem() / (1024 * 1024);
	m_unGBUFSize = GTcpSvr_GetGBufSize();
	m_unIoDataTotal = GTcpSvr_GetIoDataTotal();
	m_unIoDataUse = GTcpSvr_GetIoDataUse();
	m_strIoDataUseRate.Format(_T("%0.2f"), GTcpSvr_GetIoDataUseRate());

	m_unHndDataUseMem = GTcpSvr_GetHndDataUseMem() / (1024 * 1024);
	m_unHndDataSize = GTcpSvr_GetHndDataSize();
	m_unHndDataTotal = GTcpSvr_GetHndDataTotal();
	m_unHndDataUse = GTcpSvr_GetHndDataUse();
	m_strHndDataUseRate.Format(_T("%0.2f"), GTcpSvr_GetHndDataUseRate());

	m_unServerUseMem = GTcpSvr_GetUseMem() / (1024 * 1024);
	m_bMaxConnection = GTcpSvr_GetMaxConnection();
	m_unClientCount = GTcpSvr_GetClientCount();
	if(m_bMaxConnection)
		m_strConnectRate.Format(_T("%0.2f"), (float)m_unClientCount * 100 / m_bMaxConnection);
	else
		m_strConnectRate.Format(_T("0.00"));
	m_unPendingAcceptCount = GTcpSvr_GetPendingAcceptCount();

	m_unListenCount = GTcpSvr_GetListenCount();
	m_unThreadNumber = GTcpSvr_GetThreadNumber();
	UpdateData(FALSE);

	LV_ITEM lvitem;
	int i, nEnd;
	CString str;

	nEnd = m_lvListen.GetItemCount();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	for(i = 0; i < nEnd; i++)
	{
		lvitem.iItem = i;
		lvitem.iSubItem = 3;
		unsigned int unListetnContext = (unsigned int)m_lvListen.GetItemData(i);
		str.Format(_T("%d"), GTcpSvr_GetListenClientCount(unListetnContext));
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvListen.SetItem(&lvitem);
	}

	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	nEnd = m_lvThread.GetItemCount();

	for(i = 0; i < nEnd; i++)
	{		
		lvitem.iItem = i;
		lvitem.iSubItem = 2;
		str.Format(_T("%d"), GTcpSvr_GetThreadRunCount((unsigned int)m_lvThread.GetItemData(i)));
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvThread.SetItem(&lvitem);
	}

	CDialog::OnTimer(nIDEvent);
}