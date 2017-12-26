/********************************************************************************
						测试工具主要模块
********************************************************************************/


#pragma once
#include "..\..\G-TcpClient.h"

#ifndef DEMO_TYPE
#define DEMO_TYPE
#define HEAD_FLAG		0x1FFFFFF1
typedef struct _PACK_HEAD
{
unsigned int unHeadFlag;		//数据包头标志
unsigned int unLen;				//包长度
unsigned int unCRC;				//效验标志
unsigned int unIndex;			//序号
void*		 pOwner;			//发送者
unsigned int unTickCount;		//发送时的系统节拍
}PACK_HEAD, *PPACK_HEAD;

class CServerTestTool;
typedef struct _OPEN_CONNECT_INFO
{
CServerTestTool*	pOwner;						//所有者
wchar_t				szLIP[64];					//本地IP
wchar_t				szRIP[64];					//远程IP
wchar_t				szRPort[12];				//远程端口
unsigned int		unInlineTime;				//在线时长，0为永久在线，单位：MS
unsigned int		unConnectTotal;				//连接总数
unsigned int		unConnectSpaceTimeOfOnce;	//每次发起连接的间隔时间（MS）
unsigned int		unConnectNumberOfOnce;		//每次发起连接的数量
}OPEN_CONNECT_INFO, *POPEN_CONNECT_INFO;
#endif

__forceinline unsigned int GetCRC(unsigned char* pBuf, unsigned int unLen)
{
	//CRC太耗时，使用简单的异或和
	unsigned int Result = 0;

	while(unLen--)
	{
		Result = (Result ^ *pBuf) + *pBuf;
		pBuf++;
	}
	return(Result);
}

class CConnection;
class CServerTestTool
{
friend class CConnection;
friend DWORD WINAPI SendThreadProc(CServerTestTool* pServerTestTool);
friend DWORD WINAPI ConnectThreadProc(POPEN_CONNECT_INFO pConnectInfo);
private:
	HWND				m_hWnd;
	HANDLE				m_hHeap;
	CConnection*		m_pConnections;					//所有连接的总队列
	CRITICAL_SECTION	m_csLock;						//同步锁
	CConnection*		m_pPendingConnectedQueueHead;	//未入列队列头
	CConnection*		m_pPendingConnectedQueueTail;	//未入列队列头尾
	unsigned int		m_unSendBytes;					//调用GetSendBytes前的发生字节数
	unsigned int		m_unRecvBytes;					//调用GetRecvBytes前的发生字节数
	unsigned int		m_unLongConnectCount;			//长连接的总数
	unsigned int		m_unShortConnectCount;			//总共发起了多少短连接
	unsigned int		m_unConnectThreadCount;			//正在工作的连接线程的数量
	BOOL				m_bIsCloseSendThread;			//关闭发送线程标志
	BOOL				m_bIsCloseConnectThread;		//关闭连接线程标志
	HANDLE				m_hSendThreadEnd;				//发送线程结束标志
	HANDLE				m_hConnectThreadEnd;			//连接线程结束标志
	unsigned int		m_unSendFlag;					//发送模式标志，1：OnTimer定时发送数据包，2：OnSended每个发送投递完成后再接着发送下一个，
														//3：OnHeartbeat心跳时间到时发送数据包，4：OnResult发送的数据等服务器返回来以后再接着发送下一个
	unsigned int		m_unSendSpaceTimeOfOnce;		//定时发送的时长
	unsigned int		m_unSendPacketNumberOfOnce;		//定时发送多少个包
	unsigned int		m_unSendPacketSize;				//所有模式发送时的包长度

	BOOL				m_bShowInList;

	__forceinline void ConnectionEnqueue(CConnection* pHead, CConnection* pTail);
	__forceinline void AddConnectionFromPendingQueue(void);
	__forceinline void Lock(void){EnterCriticalSection(&m_csLock);};
	__forceinline void Unlock(void){LeaveCriticalSection(&m_csLock);};
	__forceinline CConnection* DeleteConnection(CConnection* pConnection);
public:
	CServerTestTool(HWND hWnd)
	{
		m_hWnd = hWnd;
		m_hHeap = GetProcessHeap();
		m_pConnections = NULL;
		InitializeCriticalSection(&m_csLock);
		m_pPendingConnectedQueueHead = m_pPendingConnectedQueueTail = NULL;
		m_unSendBytes = 0;
		m_unRecvBytes = 0;
		m_unLongConnectCount = 0;
		m_unShortConnectCount = 0;
		m_unConnectThreadCount = 0;
		m_bIsCloseSendThread = FALSE;
		m_bIsCloseConnectThread = FALSE;
		m_hSendThreadEnd = CreateEvent(NULL, FALSE, TRUE, NULL);
		m_hConnectThreadEnd = CreateEvent(NULL, FALSE, TRUE, NULL);

		m_unSendFlag = 1;
		m_unSendSpaceTimeOfOnce = 1000;
		m_unSendPacketNumberOfOnce = 1;
		m_unSendPacketSize = 128;

		m_bShowInList = FALSE;
	};
	~CServerTestTool(void)
	{
		Stop();
		CloseHandle(m_hConnectThreadEnd);
		CloseHandle(m_hSendThreadEnd);
		DeleteCriticalSection(&m_csLock);
	};

	__forceinline void OnConnected(CConnection* pConnection, unsigned int unPerHandle);
	__forceinline void OnDisconnected(CConnection* pConnection, unsigned int unFlag);
	__forceinline void OnSended(CConnection* pConnection, unsigned int unSendID, unsigned int unLen);
	__forceinline void OnReceive(CConnection* pConnection, unsigned char* pBuf, unsigned int unLen);

	BOOL Start(unsigned int unHeartbeatTime			= 60,
			   unsigned int unMaxNetDelayTime		= 5,
			   unsigned int unGuardThreadSleepTime	= 2,
			   unsigned int unHndDataInitNumber		= 1000,
			   unsigned int unProcesserThreadNumber = 0,
			   unsigned int unWorkerThreadNumber	= 0);
	BOOL Stop(void);
	void OpenConnect(wchar_t* pszLocalIP,
					 wchar_t* pszRemoteIP,
					 wchar_t* pszRemotePort,					 
					 unsigned int unInlineTime,
					 unsigned int unConnectTotal,
					 unsigned int unSpaceTimeOfOnce,
					 unsigned int unConnectNumberOfOnce);
	__forceinline void SetSendFlag(unsigned int unSendFlag){m_unSendFlag = unSendFlag;};
	__forceinline void SetSendSpaceTimeOfOnce(unsigned int unValue){m_unSendSpaceTimeOfOnce = unValue;};
	__forceinline void SetSendPacketNumberOfOnce(unsigned int unValue){m_unSendPacketNumberOfOnce = unValue;};
	__forceinline void SetSendPacketSize(unsigned int unValue){m_unSendPacketSize = unValue;};
	__forceinline void SetShowInList(BOOL bValue){m_bShowInList = bValue;};
	__forceinline unsigned int GetSendBytes(void){unsigned int Result = m_unSendBytes; m_unSendBytes = 0; return(Result);};
	__forceinline unsigned int GetRecvBytes(void){unsigned int Result = m_unRecvBytes; m_unRecvBytes = 0; return(Result);};
	__forceinline unsigned int GetLongConnectCount(void){return(m_unLongConnectCount);};
	__forceinline unsigned int GetShortConnectCount(void){return(m_unShortConnectCount);};
};

class CConnection
{
friend class CServerTestTool;
friend DWORD WINAPI SendThreadProc(CServerTestTool* pServerTestTool);
friend DWORD WINAPI ConnectThreadProc(POPEN_CONNECT_INFO pConnectInfo);
private:
	CConnection*		m_pNext;
	CConnection*		m_pPrior;
	CServerTestTool*	m_pOwner;
	unsigned int		m_unTime;			//每个数据包到服务器回答发送者需要的时间（MS）
	float				m_fSpeed;			//根据数据包大小*2/时间得出的网络速率
	unsigned int		m_unSendBytes;		//本连接总共发送了多少数据
	unsigned int		m_unRecvBytes;		//本连接总共接收了多少数据
	unsigned int		m_unIndexError;		//数据包序号错误计数器
	unsigned int		m_unCRCError;		//数据包效验出错计数器
	unsigned int		m_unIndexRecv;		//应该接收的数据包的序号
	unsigned int		m_unIndexSend;		//应该发送的数据包的序号
	unsigned int		m_unSendTickCount;	//最后一次发送时的系统节拍
	unsigned int		m_unConnectTickCount;	//连接完成时的系统节拍
	unsigned int		m_unPerHandle;		//本连接对于的通讯句柄
	unsigned int		m_unInlineTime;		//本连接在线时长，0：长连接，其他：在线时长（MS），超过以后自动关闭连接
	BOOL				m_bAllowFree;		//本连接使命完成标志，可以从队列删除和释放空间了
	char*				m_pBufSave;			//上次处理未完的数据包残部
	unsigned int		m_unLenSave;		//上次处理未完的数据包残部的长度
	PPACK_HEAD			m_pPackHead;		//每个连接产生数据包的缓冲
	unsigned int		m_unPackSize;		//缓冲长度

	__forceinline void OnConnected(unsigned int unPerHandle);
	__forceinline void OnDisconnected(unsigned int unFlag);
	__forceinline void OnSended(unsigned int unSendID, unsigned int unLen);
	__forceinline void OnReceive(unsigned char* pBuf, unsigned int unLen);
	__forceinline void SendPack(unsigned int unSendCount);
public:
	CConnection(CServerTestTool* pOwner, unsigned int unPackSize, unsigned int unInlineTime)
	{
		m_pOwner = pOwner;
		m_unTime = 0;
		m_fSpeed = 0;
		m_unSendBytes = 0;
		m_unRecvBytes = 0;
		m_unIndexError = 0;
		m_unCRCError = 0;
		m_unIndexRecv = 0;
		m_unIndexSend = 0;
		m_unSendTickCount = 0;
		m_unConnectTickCount = 0;
		m_unPerHandle = 0;
		m_unInlineTime = unInlineTime;
		m_bAllowFree = FALSE;
		m_pBufSave = NULL;
		m_unLenSave = 0;
		m_pPackHead = (PPACK_HEAD)HeapAlloc(pOwner->m_hHeap, 0, unPackSize);
		m_unPackSize = unPackSize;
	};
	~CConnection(void)
	{
		if(m_pPackHead)
			HeapFree(m_pOwner->m_hHeap, 0, m_pPackHead);
	};
	__forceinline unsigned int GetPerHandle(void){return(m_unPerHandle);};
	__forceinline unsigned int GetTime(void){return(m_unTime);};
	__forceinline float GetSpeed(void){return(m_fSpeed);};
	__forceinline unsigned int GetSendBytes(void){return(m_unSendBytes);};
	__forceinline unsigned int GetRecvBytes(void){return(m_unRecvBytes);};
	__forceinline unsigned int GetIndexError(void){return(m_unIndexError);};
	__forceinline unsigned int GetCRCError(void){return(m_unCRCError);};
	__forceinline void SetAllowFree(void){m_bAllowFree = TRUE;};
	__forceinline CServerTestTool* GetOwner(void){return(m_pOwner);};
};

__forceinline void CConnection::OnConnected(unsigned int unPerHandle)
{
	if(!m_unConnectTickCount)
		m_unConnectTickCount = GetTickCount();
	//当连接完成时
	m_unPerHandle = unPerHandle;
	//应收数据包序号为1
	m_unIndexRecv = 1;
	//应发数据包序号为0
	m_unIndexSend = 0;
	//上次是否有残包，有则释放
	if(m_pBufSave)
		HeapFree(m_pOwner->m_hHeap, 0, m_pBufSave);
	m_pBufSave = NULL;
	//发送头一次数据包
	if(m_unInlineTime || ((!m_unInlineTime) && (1 != m_pOwner->m_unSendFlag)))
		SendPack(1);
}

__forceinline void CConnection::OnDisconnected(unsigned int unFlag)
{
	{
	//如果是异常断开
	if(1 == unFlag)
	{
		//如果是短连接，则设置释放标志
		//if(m_unInlineTime)
		//	m_bAllowFree = TRUE;
	}else
	//被主动关闭
	if(2 == unFlag)
	{
		//设置释放标志
		m_bAllowFree = TRUE;
	}else
	//心跳时间到
	if(3 == unFlag)
		//是心态时间发送模式
		if((!m_unInlineTime) && (3 == m_pOwner->m_unSendFlag))
			SendPack(1);
	}
}

__forceinline void CConnection::OnSended(unsigned int unSendID, unsigned int unLen)
{
	//发送字节计数
	m_unSendBytes += unLen;
	//是投递完成发送模式
	if(2 == m_pOwner->m_unSendFlag)
		SendPack(1);
}

__forceinline void CConnection::OnReceive(unsigned char* pBuf, unsigned int unLen)
{
	//是数据返回发送模式
	if(4 == m_pOwner->m_unSendFlag)
		SendPack(1);

	PPACK_HEAD pDataPacks;
	char* pDataBuf;
	unsigned int unPacksLen, unProcLen;
		
	//是否有残包为处理完？
	if(m_pBufSave)
	{
		//合并长度
		unPacksLen = m_unLenSave + unLen;
		//超过法定长度，则关闭之
		if(unPacksLen > 1024 * 1024 * 1)
		{
			HeapFree(m_pOwner->m_hHeap, 0, m_pBufSave);
			m_pBufSave = NULL;
			return;
		}
		//分配缓冲
		pDataPacks = (PPACK_HEAD)HeapAlloc(m_pOwner->m_hHeap, 0, unPacksLen);
		//把旧数据放在前面
		CopyMemory(pDataPacks, m_pBufSave, m_unLenSave);
		//把新数据放在后面，完成合并工作
		CopyMemory((char*)pDataPacks + m_unLenSave, pBuf, unLen);
		//释放旧数据的缓冲
		HeapFree(m_pOwner->m_hHeap, 0, m_pBufSave);
		m_pBufSave = NULL;
		pDataBuf = (char*)pDataPacks;
	}else
	{
		pDataPacks = (PPACK_HEAD)pBuf;
		unPacksLen = unLen;
		pDataBuf = (char*)pDataPacks;
	}
	unProcLen = 0;
	//释放处理完？
	while(unProcLen < unPacksLen)
	{
		//头4个字节是否是包头标志
		if(HEAD_FLAG != pDataPacks->unHeadFlag)
		{
			//是新缓冲？
			if((unsigned char*)pDataBuf != pBuf)
				HeapFree(m_pOwner->m_hHeap, 0, pDataBuf);
			return;
		}
		//包长未达到法定长度
		if(pDataPacks->unLen > unPacksLen - unProcLen)
		{
			break;
		}
		//验证效验标志
		if(GetCRC((unsigned char*)&pDataPacks->unIndex, pDataPacks->unLen - sizeof(unsigned int) * 3) != pDataPacks->unCRC)
		{
			m_unCRCError++;
			if((unsigned char*)pDataBuf != pBuf)
				HeapFree(m_pOwner->m_hHeap, 0, pDataBuf);
			return;
		}
		m_unIndexError += (int)(m_unIndexRecv != pDataPacks->unIndex);
		m_unIndexRecv++;
		m_unRecvBytes += pDataPacks->unLen;
		m_unTime = GetTickCount() - pDataPacks->unTickCount;
		if(!m_unTime)
			m_unTime = 1;
		//计数速率
		m_fSpeed = (float)(pDataPacks->unLen * 2) * 1000 / m_unTime;
		//偏移
		unProcLen += pDataPacks->unLen;
		pDataPacks = PPACK_HEAD((char*)pDataPacks + pDataPacks->unLen);
	}
	//还有数据未处理完，则保存之，以在之后合并
	if(unProcLen < unPacksLen)
	{
		m_unLenSave = unPacksLen - unProcLen;
		m_pBufSave = (char*)HeapAlloc(m_pOwner->m_hHeap, 0, m_unLenSave);
		CopyMemory(m_pBufSave, (char*)pDataPacks, m_unLenSave);
	}
	//是新缓冲？
	if((unsigned char*)pDataBuf != pBuf)
		HeapFree(m_pOwner->m_hHeap, 0, pDataBuf);
}

__forceinline void CConnection::SendPack(unsigned int unSendCount)
{
	//如果长度有改变
	if(m_unPackSize != m_pOwner->m_unSendPacketSize)
	{
		HeapFree(m_pOwner->m_hHeap, 0, m_pPackHead);
		m_unPackSize = m_pOwner->m_unSendPacketSize;
		m_pPackHead = (PPACK_HEAD)HeapAlloc(m_pOwner->m_hHeap, 0, m_unPackSize);
	}
	//分配缓冲失效
	if(!m_pPackHead)
		 return;

	unsigned int i;

	//发送多少个包？
	i = unSendCount;
	while(i--)
	{
		//包头标志
		m_pPackHead->unHeadFlag = HEAD_FLAG;
		//包长
		m_pPackHead->unLen = m_unPackSize;
		//发送者
		m_pPackHead->pOwner = this;
		//发送序号开始计数
		m_unIndexSend++;
		if(!m_unIndexSend)
			m_unIndexSend = 1;
		//包序号
		m_pPackHead->unIndex = m_unIndexSend;
		//当前系统节拍
		m_pPackHead->unTickCount = GetTickCount();
		//记录最后一次的系统节拍
		m_unSendTickCount = m_pPackHead->unTickCount;
		//效验标志
		m_pPackHead->unCRC = GetCRC((unsigned char*)m_pPackHead + sizeof(unsigned int) * 3, m_unPackSize - sizeof(unsigned int) * 3);
		//投递发送
		GTcpClt_PostSendBuf(m_unPerHandle, (unsigned char*)m_pPackHead, m_unPackSize);
	}
}

__forceinline void CServerTestTool::ConnectionEnqueue(CConnection* pHead, CConnection* pTail)
{
	//将某段连接队列放入未决队列，等待发送线程获取，并入总的连接队列
	Lock();
	if(m_pPendingConnectedQueueTail)
	{
		pHead->m_pPrior = m_pPendingConnectedQueueTail;
		m_pPendingConnectedQueueTail->m_pNext = pHead;
		m_pPendingConnectedQueueTail = pTail;
	}else
	{
		m_pPendingConnectedQueueHead = pHead;
		m_pPendingConnectedQueueTail = pTail;
	}
	Unlock();
}

__forceinline void CServerTestTool::AddConnectionFromPendingQueue(void)
{
	//发送线程调用，将未决队列合并到总连接队列
	if(!m_pPendingConnectedQueueHead)
		return;

	CConnection *pHead, *pTail, *pTmp;

	Lock();
	pHead = m_pPendingConnectedQueueHead;
	pTail = m_pPendingConnectedQueueTail;
	m_pPendingConnectedQueueHead = NULL;
	m_pPendingConnectedQueueTail = NULL;
	Unlock();

	if(m_bShowInList)
	{
		pTmp = pHead;
		while(pTmp)
		{
			//如果是长连接，给窗口发送信息，增加一个列表记录
			if(!pTmp->m_unInlineTime)
				PostMessage(m_hWnd, WM_USER + 101, (WPARAM)pHead, 0);
			pTmp = pTmp->m_pNext;
		}
	}
	
	//合并到总队列
	if(m_pConnections)
	{
		pTail->m_pNext = m_pConnections;
		m_pConnections->m_pPrior = pTail;
		m_pConnections = pHead;
	}else
		m_pConnections = pHead;
}

__forceinline CConnection* CServerTestTool::DeleteConnection(CConnection* pConnection)
{
	//删除某个队列
	if(pConnection == m_pConnections)
	{			
		m_pConnections = m_pConnections->m_pNext;
		if(m_pConnections)
			m_pConnections->m_pPrior = NULL;
		return(m_pConnections);
	}else
	{
		pConnection->m_pPrior->m_pNext = pConnection->m_pNext;
		if(pConnection->m_pNext)
			pConnection->m_pNext->m_pPrior = pConnection->m_pPrior;
		return(pConnection->m_pNext);
	}
}

__forceinline void CServerTestTool::OnConnected(CConnection* pConnection, unsigned int unPerHandle)
{
	//当连接完成时	
	pConnection->OnConnected(unPerHandle);
}

__forceinline void CServerTestTool::OnDisconnected(CConnection* pConnection, unsigned int unFlag)
{
	//当连接断开时
	pConnection->OnDisconnected(unFlag);
	if((!pConnection->m_unInlineTime) && (2 ==unFlag))
		InterlockedDecrement((LONG*)&m_unLongConnectCount);
}

__forceinline void CServerTestTool::OnSended(CConnection* pConnection, unsigned int unSendID, unsigned int unLen)
{
	//当发送完成时
	pConnection->OnSended(unSendID, unLen);

	unsigned int unOld;

	for(;;)
	{
		unOld = m_unSendBytes;
		if(InterlockedCompareExchange((LONG*)&m_unSendBytes, unOld + unLen, unOld) == unOld)
			break;
	}
}

__forceinline void CServerTestTool::OnReceive(CConnection* pConnection, unsigned char* pBuf, unsigned int unLen)
{
	//当接收数据时
	pConnection->OnReceive(pBuf, unLen);

	unsigned int unOld;

	for(;;)
	{
		unOld = m_unRecvBytes;
		if(InterlockedCompareExchange((LONG*)&m_unRecvBytes, unOld + unLen, unOld) == unOld)
			break;
	}
}


