/********************************************************************************
*																				*
*		G-TcpServerr：基于完成端口的Tcp服务器通讯模块（IOCP TcpServer）			*
*																				*
*				Copyright © 2009-2010  GuestCode 代码客（卢益贵）  				*
*							版权所有 侵权必究									*
*																				*
* QQ:48092788  E-Mail:48092788@qq.com 源码博客：http://blog.csdn.net/guestcode	*
*																				*
*					GSN:34674B4D-1F63-11D3-B64C-11C04F79498E					*
*																				*
********************************************************************************/

#pragma once

extern "C" {

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 类型定义 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifndef _GTYPE
#define _GTYPE
typedef unsigned char* PGBUF;
typedef void(__stdcall *PGFN_ON_CONNECTED)(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_RECEIVED)(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_SENDED)(unsigned int unPerHandle, unsigned int unSendID, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_DISCONNECTED)(unsigned int unPerHandle, unsigned int unFlag);
typedef void(__stdcall *PGFN_ON_THREAD)(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag);

/*
typedef struct _CONNECTION
{
unsigned int unPerHandle;
}CONNECTION, *PCONNECTION;
typedef void(__stdcall *PGFN_ON_CONNECTED)(unsigned int unPerHandle, unsigned char* pBuf, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_RECEIVED)(PCONNECTION pConnection, unsigned char* pBuf, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_SENDED)(PCONNECTION pConnection, unsigned int unSendID, unsigned int unLen);
typedef void(__stdcall *PGFN_ON_DISCONNECTED)(PCONNECTION pConnection, unsigned int unFlag);

void __stdcall GTcpSvr_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
{
}

void __stdcall GTcpSvr_OnConnected(unsigned int unPerHandle, void* _NULL, unsigned int unNULL)
{
}

void __stdcall GTcpSvr_OnReceived(PCONNECTION pConnection, unsigned char* pBuf, unsigned int unLen)
{
}

void __stdcall GTcpSvr_OnSended(PCONNECTION pConnection, unsigned int unSendID, unsigned int unLen)
{
}

void __stdcall GTcpSvr_OnDisconnected(PCONNECTION pConnection, unsigned int unFlag)
{
}
*/

#define _USE_UNICODE						1
#ifndef _DLL
//#define _DLL
#endif
#ifdef _DLL
#define DllExport _declspec(dllexport)
#else
#define DllExport 
#endif
#define	VER_FLAG_WIDE_CHAR					0x01
#define	VER_FLAG_BETA						0x02
#define	VER_FLAG_ZERO_READ					0x04
#define	VER_FLAG_TRIAL						0x08
#define	VER_FLAG_DEBUG						0x10
#define	VER_FLAG_PRO						0x20
#define	VER_FLAG_ULT						0x40
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 类型定义 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 版本信息 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#if(_USE_UNICODE)
DllExport wchar_t* __stdcall GTcpSvr_GetVersionName(void);
#else
DllExport char* __stdcall GTcpSvr_GetVersionName(void);
#endif
DllExport float __stdcall GTcpSvr_GetVersionNumber(void);
DllExport unsigned int __stdcall GTcpSvr_GetVersionFlag(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 版本信息 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 功能函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport DWORDLONG __stdcall GTcpSvr_GetPhyMemInfo(DWORDLONG* pdwTotal);
#if(_USE_UNICODE)
DllExport void __stdcall GTcpSvr_WriteLog(wchar_t* pstrLog, unsigned int unCode = 0);
DllExport void __stdcall GTcpSvr_GetHostIP(wchar_t* pstrIP, unsigned int unLen, BOOL bIsInternetIP = FALSE);
#else
DllExport void __stdcall GTcpSvr_WriteLog(char* pstrLog, unsigned int unCode = 0);
DllExport void __stdcall GTcpSvr_GetHostIP(char* pstrIP, unsigned int unLen, BOOL bIsInternetIP = FALSE);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 功能函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>> PerIoData函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpSvr_GetGBufSize(void);
DllExport unsigned int __stdcall GTcpSvr_GetIoDataSize(void);
DllExport unsigned int __stdcall GTcpSvr_GetIoDataUse(void);
DllExport unsigned int __stdcall GTcpSvr_GetIoDataTotal(void);
DllExport float __stdcall GTcpSvr_GetIoDataUseRate(void);
DllExport unsigned int __stdcall GTcpSvr_GetIoDataUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< PerIoData函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>> PerHndData函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpSvr_GetHndDataUse(void);
DllExport unsigned int __stdcall GTcpSvr_GetHndDataTotal(void);
DllExport unsigned int __stdcall GTcpSvr_GetHndDataSize(void);
DllExport float __stdcall GTcpSvr_GetHndDataUseRate(void);
DllExport unsigned int __stdcall GTcpSvr_GetHndDataUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< PerHndData函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 信息函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpSvr_GetMaxConnection(void);
DllExport unsigned int __stdcall GTcpSvr_GetThreadNumber(void);
DllExport unsigned int __stdcall GTcpSvr_GetPageSize(void);
DllExport unsigned int __stdcall GTcpSvr_GetBlockSize(void);
DllExport unsigned int __stdcall GTcpSvr_GetClientCount(void);
DllExport unsigned int __stdcall GTcpSvr_GetListenCount(void);
DllExport unsigned int __stdcall GTcpSvr_GetPendingAcceptCount(void);
DllExport unsigned int __stdcall GTcpSvr_GetListenClientCount(unsigned int unListenContext);
DllExport unsigned int __stdcall GTcpSvr_GetThreadRunCount(unsigned int unThreadContext);
#if(_USE_UNICODE)
DllExport wchar_t* __stdcall GTcpSvr_GetThreadName(unsigned int unThreadContext);
DllExport BOOL __stdcall GTcpSvr_GetPerHandleInfo(unsigned int unPerHandle, wchar_t* pstrIP,  unsigned int unIPLen, wchar_t* pstrPort, unsigned int unPortLen);
DllExport BOOL __stdcall GTcpSvr_GetPerHandleName(unsigned int unPerHandle, wchar_t* pstrName,  unsigned int unLen);
#else
DllExport char* __stdcall GTcpSvr_GetThreadName(unsigned int unThreadContext);
DllExport BOOL __stdcall GTcpSvr_GetPerHandleInfo(unsigned int unPerHandle, char* pstrIP, unsigned int unIPLen, char* pstrPort, unsigned int unPortLen);
DllExport BOOL __stdcall GTcpSvr_GetPerHandleName(unsigned int unPerHandle, char* pstrName, unsigned int unLen);
#endif
DllExport BOOL __stdcall GTcpSvr_GetPerHandleAddr(unsigned int unPerHandle, char* pAddr,  unsigned int unLen);
DllExport unsigned int __stdcall GTcpSvr_GetProcesserNumber(void);
DllExport BOOL __stdcall GTcpSvr_IsActive();
DllExport unsigned int __stdcall GTcpSvr_GetUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 信息函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 操作函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpSvr_GetListenContext(unsigned int unPerHandle);
DllExport void* __stdcall GTcpSvr_GetListenOwner(unsigned int unPerHandle);
DllExport BOOL __stdcall GTcpSvr_SetPerHandleOwner(unsigned int unPerHandle, void* pOwner);
DllExport void* __stdcall GTcpSvr_GetPerHandleOwner(unsigned int unPerHandle);
DllExport BOOL __stdcall GTcpSvr_CloseClient(unsigned int unPerHandle);
DllExport PGBUF __stdcall GTcpSvr_AllocGBuf(void);
DllExport BOOL __stdcall GTcpSvr_FreeGBuf(PGBUF pGBuf);
DllExport unsigned int __stdcall GTcpSvr_PostSendGBuf(unsigned int unPerHandle, PGBUF pGBuf,  unsigned int unLen);
DllExport unsigned int __stdcall GTcpSvr_PostSendBuf(unsigned int unPerHandle, unsigned char* pBuf,  unsigned int unLen);
DllExport void __stdcall GTcpSvr_PostBroadcast(char* pBuf,  unsigned int unLen);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 操作函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 主要函数 
DllExport BOOL __stdcall GTcpSvr_CloseListen(unsigned int unListenContext);
#if(_USE_UNICODE)
DllExport unsigned int __stdcall GTcpSvr_OpenListen(wchar_t* pstrLocalIP,
													wchar_t* pstrLocalPort,
													PGFN_ON_CONNECTED pfnOnConnected,
													PGFN_ON_RECEIVED pfnOnReceivedd,
													PGFN_ON_SENDED pfnOnSended,
													PGFN_ON_DISCONNECTED pfnOnDisconnecteded,													
													void* pOwner = NULL);
#else
DllExport unsigned int __stdcall GTcpSvr_OpenListen(char* pstrLocalIP,
													char* pstrLocalPort,
													PGFN_ON_CONNECTED pfnOnConnected,
													PGFN_ON_RECEIVED pfnOnReceivedd,
													PGFN_ON_SENDED pfnOnSended,
													PGFN_ON_DISCONNECTED pfnOnDisconnecteded,													
													void* pOwner = NULL);
#endif
DllExport BOOL __stdcall GTcpSvr_Start(unsigned int unMaxConnection			= 1000,
									   unsigned int unHeartbeatTime			= 60,
									   unsigned int unMaxNetDelayTime		= 6,
									   PGFN_ON_THREAD pfnOnThread			= NULL,
									   unsigned int unMaxPostAccept			= 100,
									   unsigned int unIoDataInitNumber		= 1500,
									   unsigned int unProcesserThreadNumber = 0,
									   unsigned int unWorkerThreadNumber	= 0,
									   unsigned int unGuardThreadSleepTime	= 2,
									   unsigned int unHndDataExNumber		= 1500);
DllExport void __stdcall GTcpSvr_Stop(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 主要函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

}   /* ... extern "C" */