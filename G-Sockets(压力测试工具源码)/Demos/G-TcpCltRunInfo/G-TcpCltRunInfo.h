#pragma once


#include "resource.h"
#include "..\..\G-TcpClient.h"

// GTcpCltRunInfo 对话框

class GTcpCltRunInfo : public CDialog
{
	DECLARE_DYNAMIC(GTcpCltRunInfo)

public:
	GTcpCltRunInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~GTcpCltRunInfo();

// 对话框数据
	enum { IDD = IDD_GTCPCLT_RUNINFO };

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
	unsigned int m_unClientCount;

	unsigned int m_unThreadNumber;

	CListCtrl m_lvThread;
};

void __stdcall GTcpClt_CreateRunInfo(void);
void __stdcall GTcpClt_DestroyRunInfo(void);
void __stdcall GTcpClt_ShowRunInfo(void);
void __stdcall GTcpClt_HideRunInfo(void);
void __stdcall GTcpCltInfo_AddThread(unsigned int unThreadContext, unsigned hThreadHandle, unsigned int nThreadID);
void __stdcall GTcpClt_ClearRunInfo(void);