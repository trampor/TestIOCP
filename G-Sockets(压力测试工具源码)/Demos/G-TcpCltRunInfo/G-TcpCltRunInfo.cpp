// GTcpCltRunInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "G-TcpCltRunInfo.h"


// GTcpCltRunInfo 对话框

CRITICAL_SECTION GTcpCltInfo_cs;

GTcpCltRunInfo *pGTcpCltRunInfo = NULL;

void __stdcall GTcpClt_CreateRunInfo(void)
{
	if(NULL == pGTcpCltRunInfo)
	{
		InitializeCriticalSection(&GTcpCltInfo_cs);
		pGTcpCltRunInfo = new GTcpCltRunInfo;
		pGTcpCltRunInfo->Create(IDD_GTCPCLT_RUNINFO, NULL);
	}
}

void __stdcall GTcpClt_DestroyRunInfo(void)
{
	if(pGTcpCltRunInfo)
	{
		delete pGTcpCltRunInfo;
		pGTcpCltRunInfo = NULL;
	}
}

void __stdcall GTcpClt_ShowRunInfo(void)
{
	if(pGTcpCltRunInfo)
	{
		pGTcpCltRunInfo->ShowWindow(SW_SHOW);
	}
}

void __stdcall GTcpClt_HideRunInfo(void)
{
	if(pGTcpCltRunInfo)
	{
		pGTcpCltRunInfo->ShowWindow(SW_HIDE);
	}
}

void __stdcall GTcpClt_ClearRunInfo(void)
{
	if(pGTcpCltRunInfo)
	{
		pGTcpCltRunInfo->m_lvThread.DeleteAllItems();
		pGTcpCltRunInfo->UpdateWindow();
	}
}

void __stdcall GTcpCltInfo_AddThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID)
{
	if(!pGTcpCltRunInfo)
		return;

	EnterCriticalSection(&GTcpCltInfo_cs);
	LV_ITEM lvitem;
	CString str;
	int nIndex;

	nIndex = pGTcpCltRunInfo->m_lvThread.GetItemCount();
	if(0 > nIndex)
		nIndex = 0;
	lvitem.state = 0;
	lvitem.stateMask = 0;

	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 0;
	str.Format(_T("%d"), nIndex + 1);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpCltRunInfo->m_lvThread.InsertItem(&lvitem);

	pGTcpCltRunInfo->m_lvThread.SetItemData(nIndex, (DWORD_PTR)unThreadContext);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 1;
	str.Format(_T("%s"), GTcpClt_GetThreadName(unThreadContext));
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpCltRunInfo->m_lvThread.SetItem(&lvitem);

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 2;
	str.Format(_T("%d"), 0);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpCltRunInfo->m_lvThread.SetItem(&lvitem);

	/*lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	str.Format(_T("%d"), hThread);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpCltRunInfo->m_lvThread.SetItem(&lvitem);*/

	lvitem.iItem = nIndex;
	lvitem.iSubItem = 3;
	str.Format(_T("%d"), unThreadID);
	lvitem.pszText = (LPTSTR)(LPCTSTR)str;
	pGTcpCltRunInfo->m_lvThread.SetItem(&lvitem);
	LeaveCriticalSection(&GTcpCltInfo_cs);
}

void __stdcall GTcpCltInfo_DelThread(unsigned int unThread)
{
	if(!pGTcpCltRunInfo)
		return;

	EnterCriticalSection(&GTcpCltInfo_cs);
	LV_ITEM lvitem;
	int i, j, nEnd;
	CString str;

	nEnd = pGTcpCltRunInfo->m_lvThread.GetItemCount();
	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	for(i = 0; i < nEnd; i++)
	{
		if((unsigned int)pGTcpCltRunInfo->m_lvThread.GetItemData(i) == unThread)
		{
			pGTcpCltRunInfo->m_lvThread.DeleteItem(i);
			nEnd = pGTcpCltRunInfo->m_lvThread.GetItemCount();
			for(j = i; j < nEnd; j++)
			{
				lvitem.iItem = j;
				lvitem.iSubItem = 0;
				str.Format(_T("%d"), j + 1);
				lvitem.pszText = (LPTSTR)(LPCTSTR)str;
				pGTcpCltRunInfo->m_lvThread.SetItem(&lvitem);				
			}
			LeaveCriticalSection(&GTcpCltInfo_cs);
			return;
		}
	}
	LeaveCriticalSection(&GTcpCltInfo_cs);
}

IMPLEMENT_DYNAMIC(GTcpCltRunInfo, CDialog)

GTcpCltRunInfo::GTcpCltRunInfo(CWnd* pParent /*=NULL*/)
	: CDialog(GTcpCltRunInfo::IDD, pParent)
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
	, m_unClientCount(0)

	, m_unThreadNumber(0)
{
	
}

GTcpCltRunInfo::~GTcpCltRunInfo()
{
	
}

void GTcpCltRunInfo::DoDataExchange(CDataExchange* pDX)
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
	DDX_Text(pDX, IDC_LJL, m_unClientCount);

	DDX_Text(pDX, IDC_XCSL, m_unThreadNumber);

	DDX_Control(pDX, IDC_LVXC, m_lvThread);
}


BEGIN_MESSAGE_MAP(GTcpCltRunInfo, CDialog)
	ON_BN_CLICKED(IDOK, &GTcpCltRunInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GTcpCltRunInfo::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// GTcpCltRunInfo 消息处理程序

void GTcpCltRunInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void GTcpCltRunInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void GTcpCltRunInfo::SetListCtrlStyleDemo(CListCtrl* pLstCtrl)
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

BOOL GTcpCltRunInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 在此添加控件通知处理程序代码
	SetListCtrlStyleDemo(&m_lvThread);

	LV_COLUMN col;

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Inder");
	col.cx = 50;
	m_lvThread.InsertColumn(0, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Name");
	col.cx = 100;
	m_lvThread.InsertColumn(1, &col);

	col.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.iSubItem = 0;
	col.pszText = _T("Active");
	col.cx = 100;
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
	col.cx = 100;
	m_lvThread.InsertColumn(3, &col);

	CString str;
	wchar_t wt[256];

	this->GetWindowText(wt, 255);
	//版本名带T的表示试用版，最大连接不能超过100，发送字节数不能超过128，不能设置0读投递，并有其他更能限制
	//版本名带bata表示非正式版
	//版本名带WChar表示支持WideChar
	//版本号以时间格式累加和显示，
	str.Format(_T("%s %s"), wt, GTcpClt_GetVersionName());	
	this->SetWindowText(str);

	SetTimer(10000, 1000, NULL);
	return TRUE;
}
void GTcpCltRunInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_unFreePhyMemSize = GTcpClt_GetPhyMemInfo(&m_unPhyMemSize) / (1024 * 1024);
	m_unPhyMemSize /= (1024 * 1024);
	m_strPhyMemFreeRate.Format(_T("%0.2f"), (double)m_unFreePhyMemSize / (double)m_unPhyMemSize * (double)100);
	m_unPageSize = GTcpClt_GetPageSize();
	m_unCPUNumber = GTcpClt_GetProcesserNumber();

	m_unIoDataUseMem = GTcpClt_GetIoDataUseMem() / (1024 * 1024);
	m_unGBUFSize = GTcpClt_GetGBufSize();
	m_unIoDataTotal = GTcpClt_GetIoDataTotal();
	m_unIoDataUse = GTcpClt_GetIoDataUse();
	m_strIoDataUseRate.Format(_T("%0.2f"), GTcpClt_GetIoDataUseRate());

	m_unHndDataUseMem = GTcpClt_GetHndDataUseMem() / (1024 * 1024);
	m_unHndDataSize = GTcpClt_GetHndDataSize();
	m_unHndDataTotal = GTcpClt_GetHndDataTotal();
	m_unHndDataUse = GTcpClt_GetHndDataUse();
	m_strHndDataUseRate.Format(_T("%0.2f"), GTcpClt_GetHndDataUseRate());

	m_unServerUseMem = GTcpClt_GetUseMem() / (1024 * 1024);
	m_unClientCount = GTcpClt_GetConnectCount();
	m_unThreadNumber = GTcpClt_GetThreadNumber();
	UpdateData(FALSE);

	LV_ITEM lvitem;
	int i, nEnd;
	CString str;

	lvitem.state = 0;
	lvitem.stateMask = 0;
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	nEnd = m_lvThread.GetItemCount();

	for(i = 0; i < nEnd; i++)
	{		
		lvitem.iItem = i;
		lvitem.iSubItem = 2;
		str.Format(_T("%d"), GTcpClt_GetThreadRunCount((unsigned int)m_lvThread.GetItemData(i)));
		lvitem.pszText = (LPTSTR)(LPCTSTR)str;
		m_lvThread.SetItem(&lvitem);
	}

	CDialog::OnTimer(nIDEvent);
}