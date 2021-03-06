/********************************************************************************
*																				*
*		G-TcpClient：基于完成端口的Tcp客户端通讯模块（IOCP TcpClient）			*
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

void __stdcall GTcpClt_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
{
}

void __stdcall GTcpClt_OnConnected(unsigned int unPerHandle, void* _NULL, unsigned int unNULL)
{
}

void __stdcall GTcpClt_OnReceived(PCONNECTION pConnection, unsigned char* pBuf, unsigned int unLen)
{
}

void __stdcall GTcpClt_OnSended(PCONNECTION pConnection, unsigned int unSendID, unsigned int unLen)
{
}

void __stdcall GTcpClt_OnDisconnected(PCONNECTION pConnection, unsigned int unFlag)
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

#define HNDS_CONNECT						1
#define HNDS_CONNECTED						2
#define HNDS_DISCONNECT						3

#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 类型定义 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 版本信息 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#if(_USE_UNICODE)
DllExport wchar_t* __stdcall GTcpClt_GetVersionName(void);
#else
DllExport char* __stdcall GTcpClt_GetVersionName(void);
#endif
DllExport float __stdcall GTcpClt_GetVersionNumber(void);
DllExport unsigned int __stdcall GTcpClt_GetVersionFlag(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 版本信息 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 功能函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport DWORDLONG __stdcall GTcpClt_GetPhyMemInfo(DWORDLONG* pdwTotal);
#if(_USE_UNICODE)
DllExport void __stdcall GTcpClt_WriteLog(wchar_t* pstrLog, unsigned int unCode = 0);
DllExport void __stdcall GTcpClt_GetHostIP(wchar_t* pstrIP, unsigned int unLen, BOOL bIsInternetIP = FALSE);
#else
DllExport void __stdcall GTcpClt_WriteLog(char* pstrLog, unsigned int unCode = 0);
DllExport void __stdcall GTcpClt_GetHostIP(char* pstrIP, unsigned int unLen, BOOL bIsInternetIP = FALSE);
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 功能函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>> PerIoData函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpClt_GetGBufSize(void);
DllExport unsigned int __stdcall GTcpClt_GetIoDataSize(void);
DllExport unsigned int __stdcall GTcpClt_GetIoDataUse(void);
DllExport unsigned int __stdcall GTcpClt_GetIoDataTotal(void);
DllExport float __stdcall GTcpClt_GetIoDataUseRate(void);
DllExport unsigned int __stdcall GTcpClt_GetIoDataUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< PerIoData函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>> PerHndData函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpClt_GetHndDataUse(void);
DllExport unsigned int __stdcall GTcpClt_GetHndDataTotal(void);
DllExport unsigned int __stdcall GTcpClt_GetHndDataSize(void);
DllExport float __stdcall GTcpClt_GetHndDataUseRate(void);
DllExport unsigned int __stdcall GTcpClt_GetHndDataUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<< PerHndData函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 信息函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport unsigned int __stdcall GTcpClt_GetThreadNumber(void);
DllExport unsigned int __stdcall GTcpClt_GetPageSize(void);
DllExport unsigned int __stdcall GTcpClt_GetBlockSize(void);
DllExport unsigned int __stdcall GTcpClt_GetConnectCount(void);
DllExport unsigned int __stdcall GTcpClt_GetThreadRunCount(unsigned int unThreadContext);
DllExport unsigned int GTcpClt_GetState(unsigned int unPerHandle);
#if(_USE_UNICODE)
DllExport wchar_t* __stdcall GTcpClt_GetThreadName(unsigned int unThreadContext);
DllExport BOOL __stdcall GTcpSock_GetPerHandleInfo(unsigned int unPerHandle, wchar_t* pstrIP,  unsigned int unIPLen, wchar_t* pstrPort, unsigned int unPortLen);
DllExport BOOL __stdcall GTcpSock_GetPerHandleName(unsigned int unPerHandle, wchar_t* pstrName,  unsigned int unLen);
#else
DllExport char* __stdcall GTcpClt_GetThreadName(unsigned int unThreadContext);
DllExport BOOL __stdcall GTcpSock_GetPerHandleInfo(unsigned int unPerHandle, char* pstrIP, unsigned int unIPLen, char* pstrPort, unsigned int unPortLen);
DllExport BOOL __stdcall GTcpSock_GetPerHandleName(unsigned int unPerHandle, char* pstrName, unsigned int unLen);
#endif
DllExport unsigned int __stdcall GTcpClt_GetProcesserNumber(void);
DllExport BOOL __stdcall GTcpClt_IsActive();
DllExport unsigned int __stdcall GTcpClt_GetUseMem(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 信息函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 操作函数 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
DllExport void* __stdcall GTcpClt_GetPerHandleOwner(unsigned int unPerHandle);
DllExport BOOL __stdcall GTcpClt_SetPerHandleOwner(unsigned int unPerHandle, void* pOwner);
DllExport PGBUF __stdcall GTcpClt_AllocGBuf(void);
DllExport BOOL __stdcall GTcpClt_FreeGBuf(PGBUF pGBuf);
DllExport unsigned int __stdcall GTcpClt_PostSendGBuf(unsigned int unPerHandle, PGBUF pGBuf,  unsigned int unLen);
DllExport unsigned int __stdcall GTcpClt_PostSendBuf(unsigned int unPerHandle, unsigned char* pBuf,  unsigned int unLen);
DllExport void __stdcall GTcpClt_PostBroadcast(unsigned char* pBuf,  unsigned int unLen);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 操作函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 主要函数
DllExport BOOL __stdcall GTcpClt_CloseConnect(unsigned int unPerHandle);
#if(_USE_UNICODE)
DllExport unsigned int __stdcall GTcpClt_OpenConnect(	wchar_t* pstrRemoteIP,
														wchar_t* pstrRemotePort,
														wchar_t* pstrLocalIP,
														PGFN_ON_CONNECTED pfnOnConnected,
														PGFN_ON_RECEIVED pfnOnReceived,
														PGFN_ON_SENDED pfnOnSended,
														PGFN_ON_DISCONNECTED pfnOnDisconnected,
														void* pOwner = NULL);
#else
DllExport unsigned int __stdcall GTcpClt_OpenConnect(	char* pstrRemoteIP,
														char* pstrRemotePort,
														char* pstrLocalIP,
														PGFN_ON_CONNECTED pfnOnConnected,
														PGFN_ON_RECEIVED pfnOnReceived,
														PGFN_ON_SENDED pfnOnSended,
														PGFN_ON_DISCONNECTED pfnOnDisconnected,													
														void* pOwner = NULL);
#endif
DllExport BOOL __stdcall GTcpClt_Start(unsigned int unHeartbeatTime			= 60,
									   unsigned int unMaxNetDelayTime		= 5,
									   unsigned int unGuardThreadSleepTime	= 2,
									   PGFN_ON_THREAD pfnOnThread			= NULL,
									   unsigned int unHndDataInitNumber		= 1000,
									   unsigned int unIoDataInitNumber		= 1500,
									   unsigned int unProcesserThreadNumber = 0,
									   unsigned int unWorkerThreadNumber	= 0);
DllExport void __stdcall GTcpClt_Stop(void);
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 主要函数 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

}   /* ... extern "C" */