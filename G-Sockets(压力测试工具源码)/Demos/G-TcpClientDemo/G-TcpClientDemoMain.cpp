/********************************************************************************
								测试工具主要模块
********************************************************************************/

#include "stdafx.h"
#include "G-TcpClientDemoMain.h"
#include "..\..\G-TcpClient.h"
#include "..\G-TcpCltRunInfo\G-TcpCltRunInfo.h"

/********************************************************************************
				以下是 G-TcpClient 事件回调函数
********************************************************************************/
void __stdcall GTcpClt_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
{
	//线程刚开始运行
	if(bIsBegin)
	{
		//给运行信息窗口增加一个线程列表
		GTcpCltInfo_AddThread(unThreadContext, unThreadHandle, unThreadID);
		//是处理线程
		if(1 == unFlag)
		{
			//处理线程工作量比较大，应该高于工作线程
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
			//或SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//只有处理线程才会调用回调函数响应连接的所有事件，所有可能需要初始化线程，
			//以使用数据库操作等其他需要线程内独立的操作
			CoInitialize(NULL);
		}else
		//是工作线程
		if(2 == unFlag)
		{
			//工作线程权限介于看守线程和处理线程之间
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}else
		//是看守线程
		if(3 == unFlag)
		{
			//看守线程没有必要设置太高权限
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}
	}else
	//线程结束
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

void __stdcall GTcpClt_OnConnected(unsigned int unPerHandle, void* _NULL, unsigned int unNULL)
{
	CConnection* pConnection = (CConnection*)GTcpClt_GetPerHandleOwner(unPerHandle);
	pConnection->GetOwner()->OnConnected(pConnection, unPerHandle);
}

void __stdcall GTcpClt_OnReceived(CConnection* pConnection, unsigned char* pBuf, unsigned int unLen)
{
	pConnection->GetOwner()->OnReceive(pConnection, pBuf, unLen);
}

void __stdcall GTcpClt_OnSended(CConnection* pConnection, unsigned int unSendID, unsigned int unLen)
{
	pConnection->GetOwner()->OnSended(pConnection, unSendID, unLen);	
}

void __stdcall GTcpClt_OnDisconnected(CConnection* pConnection, unsigned int unFlag)
{
	pConnection->GetOwner()->OnDisconnected(pConnection, unFlag);
}
/********************************************************************************
				以上是 G-TcpClient 事件回调函数
********************************************************************************/

DWORD WINAPI SendThreadProc(CServerTestTool* pServerTestTool)
{
	AfxInitThread();

	CConnection *pConnection, *pTmp;
	DWORD dwTickCount;
	unsigned int unSendFlag;
	BOOL bIsClose;

	unSendFlag = pServerTestTool->m_unSendFlag;
	bIsClose = FALSE;
	ResetEvent(pServerTestTool->m_hSendThreadEnd);
	for(;;)	
	{
		//看看有没有新的连接在未决队列
		pServerTestTool->AddConnectionFromPendingQueue();
		//如果是关闭并且已经释放完所有连接
		if(pServerTestTool->m_bIsCloseSendThread && (!pServerTestTool->m_pConnections))
			break;
		dwTickCount = GetTickCount();
		//是否关闭？
		if(!pServerTestTool->m_bIsCloseSendThread)
		{//不是关闭
			pConnection = pServerTestTool->m_pConnections;
			while(pConnection)
			{
				//已经被关闭了
				if(pConnection->m_bAllowFree)
				{
					pTmp = pConnection;
					pConnection = pServerTestTool->DeleteConnection(pConnection);
					delete pTmp;
					continue;
				}else
				if((pConnection->m_unInlineTime) && pConnection->m_unSendBytes &&
				   ((1 == pConnection->m_unInlineTime) || (dwTickCount - pConnection->m_unConnectTickCount >= pConnection->m_unInlineTime)))
				{//如果是短连接，并且成功发送的数据，连接时间达到规定的时间，则关闭之
					pConnection->m_unSendBytes = 0;
					GTcpClt_CloseConnect(pConnection->m_unPerHandle);
				}else
				if(1 == pServerTestTool->m_unSendFlag)
				{//定时发送模式
					if(HNDS_CONNECTED == GTcpClt_GetState(pConnection->m_unPerHandle))
					{
						//定时发送模式并且发送时间到？
						if(dwTickCount - pConnection->m_unSendTickCount >= pServerTestTool->m_unSendSpaceTimeOfOnce)
						{
							//定时发送模式，发送设定数量的数据包
							pConnection->SendPack(pServerTestTool->m_unSendPacketNumberOfOnce);
						}
					}
				}
				pConnection = pConnection->m_pNext;
			}
			Sleep(1);
		}else		
		{//是关闭
			if(bIsClose != pServerTestTool->m_bIsCloseSendThread)
			{//第一次获得关闭信号
				bIsClose = pServerTestTool->m_bIsCloseSendThread;
				pConnection = pServerTestTool->m_pConnections;
				//遍历并投递关闭请求
				while(pConnection)
				{
					GTcpClt_CloseConnect(pConnection->m_unPerHandle);
					pConnection = pConnection->m_pNext;
				}
			}
			pConnection = pServerTestTool->m_pConnections;
			//遍历所有，看看有没有已经完成关闭工作的，有则删除节点和释放内存
			while(pConnection)
			{
				if(pConnection->m_bAllowFree)
				{
					pTmp = pConnection;
					pConnection = pServerTestTool->DeleteConnection(pConnection);
					delete pTmp;
					continue;
				}
				pConnection = pConnection->m_pNext;
			}
		}
		//发送模式改变为非定时发送时，则遍历所有连接发送一个数据包，以触发相应条件达到设定的发送模式
		if(unSendFlag != pServerTestTool->m_unSendFlag)
		{
			unSendFlag = pServerTestTool->m_unSendFlag;
			if((2 == pServerTestTool->m_unSendFlag) || (4 == pServerTestTool->m_unSendFlag))
			{
				pConnection = pServerTestTool->m_pConnections;
				while(pConnection && (!pServerTestTool->m_bIsCloseSendThread))
				{
					if(HNDS_CONNECTED == GTcpClt_GetState(pConnection->m_unPerHandle))
						pConnection->SendPack(1);
					pConnection = pConnection->m_pNext;
				}
			}
		}
	}
	//我已经安全退出了
	SetEvent(pServerTestTool->m_hSendThreadEnd);
	return(0);
}

DWORD WINAPI ConnectThreadProc(POPEN_CONNECT_INFO pConnectInfo)
{
	AfxInitThread();

	CConnection *pHead, *pTail, *pTmp;
	unsigned int unCount, unSum;

	unSum = 0;
	for(;;)	
	{
		pHead = pTail = NULL;		
		//每次发起多少个链接
		unCount = pConnectInfo->unConnectNumberOfOnce;
		while(unCount--)
		{
			//新的连接对象
			pTmp = new CConnection(pConnectInfo->pOwner,
								   pConnectInfo->pOwner->m_unSendPacketSize, 
								   pConnectInfo->unInlineTime);
			if(!pTmp)
				break;
			pTmp->m_unInlineTime = pConnectInfo->unInlineTime;
			//投递连接请求
			pTmp->m_unPerHandle = GTcpClt_OpenConnect(pConnectInfo->szRIP,
														pConnectInfo->szRPort,
														pConnectInfo->szLIP,
														(PGFN_ON_CONNECTED)&GTcpClt_OnConnected,
														(PGFN_ON_RECEIVED)&GTcpClt_OnReceived,
														(PGFN_ON_SENDED)&GTcpClt_OnSended,
														(PGFN_ON_DISCONNECTED)&GTcpClt_OnDisconnected,
														pTmp);
			//请求失败
			if(!pTmp->m_unPerHandle)
			{
				delete pTmp;
				if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
					break;
				continue;
			}
			//加入到本次连接的小段队列里
			//（为防止发送线程遍历时，加入连接到总队列等待时间过长，因此先组成一段小队列，
			//再放在未决队列里，以便发送线程获取合并到总队列）
			pTmp->m_pNext = NULL;
			if(pTail)
			{
				pTail->m_pNext = pTmp;
				pTmp->m_pPrior = pTail;
				pTail = pTmp;
			}else
			{
				pTmp->m_pPrior = NULL;
				pHead = pTail = pTmp;
			}
			if(!pTmp->m_unInlineTime)
				InterlockedIncrement((LONG*)&pConnectInfo->pOwner->m_unLongConnectCount);
			else
				InterlockedIncrement((LONG*)&pConnectInfo->pOwner->m_unShortConnectCount);
			//如果有总数限制
			if(pConnectInfo->unConnectTotal)
			{
				unSum++;
				//如果超过连接总数
				if(unSum >= pConnectInfo->unConnectTotal)
				{
					//将本次连接的小队列放入未决队列里，以便发送线程处理
					if(pHead)
						pConnectInfo->pOwner->ConnectionEnqueue(pHead, pTail);
					goto End;
				}
			}
			//如果是关闭
			if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
				break;
		}
		if(pHead)
			//将本次连接的小队列放入未决队列里，以便发送线程处理
			pConnectInfo->pOwner->ConnectionEnqueue(pHead, pTail);
		//如果是关闭
		if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
			break;
		//休眠一段时间再发起连接
		Sleep(pConnectInfo->unConnectSpaceTimeOfOnce);
	}
End:
	//是不是最后一个退出的连接线程？
	if(0 == InterlockedDecrement((LONG*)&pConnectInfo->pOwner->m_unConnectThreadCount))
		//我安全退出了
		SetEvent(pConnectInfo->pOwner->m_hConnectThreadEnd);
	//释放内存
	delete pConnectInfo;
	return(0);
}

void CServerTestTool::OpenConnect(wchar_t* pszLocalIP,
								  wchar_t* pszRemoteIP,
								  wchar_t* pszRemotePort,					 
								  unsigned int unInlineTime,
								  unsigned int unConnectTotal,
								  unsigned int unSpaceTimeOfOnce,
								  unsigned int unConnectNumberOfOnce)
{
	POPEN_CONNECT_INFO pConnectInfo;

	pConnectInfo = new(OPEN_CONNECT_INFO);
	if(!pConnectInfo)
		return;
	pConnectInfo->pOwner = this;
	swprintf_s(pConnectInfo->szLIP, _T("%s"), pszLocalIP);
	swprintf_s(pConnectInfo->szRIP, _T("%s"), pszRemoteIP);
	swprintf_s(pConnectInfo->szRPort, _T("%s"), pszRemotePort);
	pConnectInfo->unInlineTime = unInlineTime;
	pConnectInfo->unConnectSpaceTimeOfOnce = unSpaceTimeOfOnce;
	pConnectInfo->unConnectNumberOfOnce = unConnectNumberOfOnce;
	pConnectInfo->unConnectTotal = unConnectTotal;
	//如果是第一个启动的连接线程
	if(1 == InterlockedIncrement((LONG*)&m_unConnectThreadCount))
		//设置事件状态
		ResetEvent(m_hConnectThreadEnd);

	unsigned int unID;

	//创建连接线程
	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ConnectThreadProc, pConnectInfo, 0, (LPDWORD)&unID));
}

BOOL CServerTestTool::Start(unsigned int unHeartbeatTime,
						    unsigned int unMaxNetDelayTime,
						    unsigned int unGuardThreadSleepTime,
						    unsigned int unHndDataInitNumber,
						    unsigned int unProcesserThreadNumber,
						    unsigned int unWorkerThreadNumber)
{
	m_pConnections = NULL;
	m_pPendingConnectedQueueHead = m_pPendingConnectedQueueTail = NULL;
	m_unSendBytes = 0;
	m_unRecvBytes = 0;
	m_unLongConnectCount = 0;
	m_unShortConnectCount = 0;
	m_unConnectThreadCount = 0;
	m_bIsCloseSendThread = FALSE;
	m_bIsCloseConnectThread = FALSE;

	unsigned int unID;

	CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&SendThreadProc, this, 0, (LPDWORD)&unID));
	GTcpClt_Start(unHeartbeatTime,
				  unMaxNetDelayTime,
				  unGuardThreadSleepTime,
				  &GTcpClt_OnThread,
				  unHndDataInitNumber,
				  1500,
				  unProcesserThreadNumber,
				  unWorkerThreadNumber);
	return(GTcpClt_IsActive());
}

BOOL CServerTestTool::Stop(void)
{
	m_bIsCloseConnectThread = TRUE;
	WaitForSingleObject(m_hConnectThreadEnd, INFINITE);
	SetEvent(m_hConnectThreadEnd);
	m_bIsCloseSendThread = TRUE;
	WaitForSingleObject(m_hSendThreadEnd, INFINITE);
	SetEvent(m_hSendThreadEnd);
	GTcpClt_Stop();
	return(!GTcpClt_IsActive());
}
