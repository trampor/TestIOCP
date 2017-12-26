#pragma once


#include "resource.h"
#include "..\..\G-TcpServer.h"

// GTcpSvrRunInfo 对话框

class GTcpSvrRunInfo : public CDialog
{
	DECLARE_DYNAMIC(GTcpSvrRunInfo)

public:
	GTcpSvrRunInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~GTcpSvrRunInfo();

// 对话框数据
	enum { IDD = IDD_GTCPSVR_RUNINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void SetListCtrlStyleDemo(CListCtrl* pLstCtrl);
	unsigned int m_unCPUNumber;
	unsigned int m_unPageSize;
	DWORDLONG m_unPhyMemSize;
	DWORDLONG m_unFreePhyMemSize;
	CString		 m_strPhyMemFreeRate;	

	unsigned int m_unIoDataUseMem;
	unsigned int m_unGBUFSize;
	unsigned int m_unIoDataTotal;
	unsigned int m_unIoDataUse;
	CString		 m_strIoDataUseRate;

	unsigned int m_unHndDataUseMem;
	unsigned int m_unHndDataSize;
	unsigned int m_unHndDataTotal;
	unsigned int m_unHndDataUse;
	CString		 m_strHndDataUseRate;

	unsigned int m_unServerUseMem;
	unsigned int m_unPendingAcceptCount;
	unsigned int m_bMaxConnection;
	unsigned int m_unClientCount;
	CString		 m_strConnectRate;	

	unsigned int m_unListenCount;
	unsigned int m_unThreadNumber;

	CListCtrl m_lvListen;
	CListCtrl m_lvThread;
};

void __stdcall GTcpSvr_CreateRunInfo(void);
void __stdcall GTcpSvr_DestroyRunInfo(void);
void __stdcall GTcpSvr_ShowRunInfo(void);
void __stdcall GTcpSvr_HideRunInfo(void);
void __stdcall GTcpSvrInfo_DelListen(unsigned int unListen);
void __stdcall GTcpSvrInfo_AddListen(unsigned int unListen, wchar_t* pstrIP, unsigned int unPort);
void __stdcall GTcpSvrInfo_AddThread(unsigned int unThreadContext, unsigned hThreadHandle, unsigned int nThreadID);
void __stdcall GTcpSvr_ClearRunInfo(void);