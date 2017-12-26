/********************************************************************************
说明：
本示例代码尚未进行优化也未进行安全性验证，
仅为主动发送模式下服务器设计方法上的介绍，
不等于实际应该或必须如此，请结合实际应用严格编写代码。
********************************************************************************/

#include "stdafx.h"
#include "ServerDemo.h"

/********************************************************************************
				以下是 GTcpServer 事件回调函数
********************************************************************************/
void __stdcall Callback_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
//说明：线程开始和结束时在线程体内的回调函数，可以在这里设置线程权限和进行线程相关的初始化工作。
//输入：hThreadHandle：线程句柄
//		nThreadID：线程ID
//		bIsBegin：为TRUE表示线程刚开始运行，其他表示线程结束了。
//		unFlag：1：为数据处理者线程，2：为IO工作者线程，3：为看守线程，其他未定义。
//输出：无
//注意：线程权限设置可根据处理线程、工作者线程、伺服线程的角色来设置，相关线程内部需要的初始化工作，可以根据
//		unFlag为1和bIsBegin为真时进行初始化工作，比如：CoInitialize(nil)；unFlag为1和bIsBegin为假进行反初始化
//		工作，比如：CoUninitialize()。
{
	//线程刚开始运行
	if(bIsBegin)
	{
		//是处理线程
		if(1 == unFlag)
		{
			//处理线程工作量比较大，应该高于工作线程
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
			//或SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//只有处理线程才会调用回调函数响应连接的所有事件，所有可能需要初始化线程，
			//以使用数据库操作等其他需要线程内独立的操作
			CoInitialize(NULL);
		}else
		//是工作线程
		if(2 == unFlag)
		{
			//工作线程权限介于看守线程和处理线程之间
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
		}else
		//是看守线程
		if(3 == unFlag)
		{
			//看守线程没有必要设置太高权限
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
		}
	}else
	{
		//是处理线程
		if(1 == unFlag)
		{
			//反初始化
			CoUninitialize();
			//取消线程模式
			//或SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_END);
		}else
		//是工作线程
		if(2 == unFlag)
		{
			
		}else
		//是看守线程
		if(3 == unFlag)
		{
			
		}
	}
}

void __stdcall Callback_OnConnected(unsigned int unPerHandle, char* pBuf, unsigned int unLen)
//说明：客户连接后的回调函数
//输入：unPerHandle：每个连接客户上下文，每个连接是唯一
//		pBuf：接收的数据缓冲区地址指针，
//		nLen：接收数据缓冲区的长度，即接收数据的大小。
//输出：无
//注意：不能往pBuf里面写入任何数据，nLen最大不超过Callback_GetGBufSize返回值，不能用任
//		何方式释放pBuf。本回调函数只在处理者线程体内被调用，因此请在处理者线程开始和结
//		束时进行相关初始化和反初始化操作。unPerHandle的生命在OnDisconnect或
//		OnOvertime到来之后将结束，此后对unPerHandle的操作将是无效的
{
	((CHall*)GTcpSvr_GetListenOwner(unPerHandle))->OnConnected(unPerHandle, pBuf, unLen);
}

void __stdcall Callback_OnReceived(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo)
//说明：客户连接响应OnConnect后，收到客户数据的回调函数
//		pUserInfo：CUserInfo类指针
//		pBuf：接收的数据缓冲区地址指针，
//		nLen：接收数据缓冲区的长度，即接收数据的大小。
//输出：无
//注意：不能往pBuf里面写入任何数据，nLen最大不超过Callback_GetGBufSize返回值，不能用任
//		何方式释放pBuf。本回调函数只在处理者线程体内被调用，因此请在处理者线程开始和结
//		束时进行相关初始化和反初始化操作。
{
	((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->DataPackProcesser(unPerHandle, pBuf, unLen, pUserInfo);
}

void __stdcall Callback_OnSended(unsigned int unPerHandle, unsigned int unSendID, unsigned int unLen, CUserInfo* pUserInfo)
//说明：每笔发送投递已决并发送到对方链路层之后回调
//		pUserInfo：CUserInfo类指针
//		unSendID：与PostSendBuf或PostSendGBuf返回值一致
//		unLen：已经发生完毕的字节数
//输出：无
{
	((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->OnSended(unPerHandle, unSendID, unLen, pUserInfo);
}

void __stdcall Callback_OnDisconnected(unsigned int unPerHandle, unsigned int unFlag, CUserInfo* pUserInfo)
//说明：客户断开后的回调函数
//		pUserInfo：CUserInfo类指针
//		bIsClose：真为被服务器强制关闭（调用CloseClient），假为客户端口断开或网络故障导致断开
//输出：无
//注意：使用Callback_CloseClient不会回调此函数
{
	if(pUserInfo)
		((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->OnDisconnect(unPerHandle, unFlag, pUserInfo);
}
/********************************************************************************
				以上是 GTcpServer 事件回调函数
********************************************************************************/