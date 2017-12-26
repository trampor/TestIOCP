/********************************************************************************
						���Թ�����Ҫģ��
********************************************************************************/


#pragma once
#include "..\..\G-TcpClient.h"

#ifndef DEMO_TYPE
#define DEMO_TYPE
#define HEAD_FLAG		0x1FFFFFF1
typedef struct _PACK_HEAD
{
unsigned int unHeadFlag;		//���ݰ�ͷ��־
unsigned int unLen;				//������
unsigned int unCRC;				//Ч���־
unsigned int unIndex;			//���
void*		 pOwner;			//������
unsigned int unTickCount;		//����ʱ��ϵͳ����
}PACK_HEAD, *PPACK_HEAD;

class CServerTestTool;
typedef struct _OPEN_CONNECT_INFO
{
CServerTestTool*	pOwner;						//������
wchar_t				szLIP[64];					//����IP
wchar_t				szRIP[64];					//Զ��IP
wchar_t				szRPort[12];				//Զ�̶˿�
unsigned int		unInlineTime;				//����ʱ����0Ϊ�������ߣ���λ��MS
unsigned int		unConnectTotal;				//��������
unsigned int		unConnectSpaceTimeOfOnce;	//ÿ�η������ӵļ��ʱ�䣨MS��
unsigned int		unConnectNumberOfOnce;		//ÿ�η������ӵ�����
}OPEN_CONNECT_INFO, *POPEN_CONNECT_INFO;
#endif

__forceinline unsigned int GetCRC(unsigned char* pBuf, unsigned int unLen)
{
	//CRC̫��ʱ��ʹ�ü򵥵�����
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
	CConnection*		m_pConnections;					//�������ӵ��ܶ���
	CRITICAL_SECTION	m_csLock;						//ͬ����
	CConnection*		m_pPendingConnectedQueueHead;	//δ���ж���ͷ
	CConnection*		m_pPendingConnectedQueueTail;	//δ���ж���ͷβ
	unsigned int		m_unSendBytes;					//����GetSendBytesǰ�ķ����ֽ���
	unsigned int		m_unRecvBytes;					//����GetRecvBytesǰ�ķ����ֽ���
	unsigned int		m_unLongConnectCount;			//�����ӵ�����
	unsigned int		m_unShortConnectCount;			//�ܹ������˶��ٶ�����
	unsigned int		m_unConnectThreadCount;			//���ڹ����������̵߳�����
	BOOL				m_bIsCloseSendThread;			//�رշ����̱߳�־
	BOOL				m_bIsCloseConnectThread;		//�ر������̱߳�־
	HANDLE				m_hSendThreadEnd;				//�����߳̽�����־
	HANDLE				m_hConnectThreadEnd;			//�����߳̽�����־
	unsigned int		m_unSendFlag;					//����ģʽ��־��1��OnTimer��ʱ�������ݰ���2��OnSendedÿ������Ͷ����ɺ��ٽ��ŷ�����һ����
														//3��OnHeartbeat����ʱ�䵽ʱ�������ݰ���4��OnResult���͵����ݵȷ������������Ժ��ٽ��ŷ�����һ��
	unsigned int		m_unSendSpaceTimeOfOnce;		//��ʱ���͵�ʱ��
	unsigned int		m_unSendPacketNumberOfOnce;		//��ʱ���Ͷ��ٸ���
	unsigned int		m_unSendPacketSize;				//����ģʽ����ʱ�İ�����

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
	unsigned int		m_unTime;			//ÿ�����ݰ����������ش�������Ҫ��ʱ�䣨MS��
	float				m_fSpeed;			//�������ݰ���С*2/ʱ��ó�����������
	unsigned int		m_unSendBytes;		//�������ܹ������˶�������
	unsigned int		m_unRecvBytes;		//�������ܹ������˶�������
	unsigned int		m_unIndexError;		//���ݰ���Ŵ��������
	unsigned int		m_unCRCError;		//���ݰ�Ч����������
	unsigned int		m_unIndexRecv;		//Ӧ�ý��յ����ݰ������
	unsigned int		m_unIndexSend;		//Ӧ�÷��͵����ݰ������
	unsigned int		m_unSendTickCount;	//���һ�η���ʱ��ϵͳ����
	unsigned int		m_unConnectTickCount;	//�������ʱ��ϵͳ����
	unsigned int		m_unPerHandle;		//�����Ӷ��ڵ�ͨѶ���
	unsigned int		m_unInlineTime;		//����������ʱ����0�������ӣ�����������ʱ����MS���������Ժ��Զ��ر�����
	BOOL				m_bAllowFree;		//������ʹ����ɱ�־�����ԴӶ���ɾ�����ͷſռ���
	char*				m_pBufSave;			//�ϴδ���δ������ݰ��в�
	unsigned int		m_unLenSave;		//�ϴδ���δ������ݰ��в��ĳ���
	PPACK_HEAD			m_pPackHead;		//ÿ�����Ӳ������ݰ��Ļ���
	unsigned int		m_unPackSize;		//���峤��

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
	//���������ʱ
	m_unPerHandle = unPerHandle;
	//Ӧ�����ݰ����Ϊ1
	m_unIndexRecv = 1;
	//Ӧ�����ݰ����Ϊ0
	m_unIndexSend = 0;
	//�ϴ��Ƿ��ва��������ͷ�
	if(m_pBufSave)
		HeapFree(m_pOwner->m_hHeap, 0, m_pBufSave);
	m_pBufSave = NULL;
	//����ͷһ�����ݰ�
	if(m_unInlineTime || ((!m_unInlineTime) && (1 != m_pOwner->m_unSendFlag)))
		SendPack(1);
}

__forceinline void CConnection::OnDisconnected(unsigned int unFlag)
{
	{
	//������쳣�Ͽ�
	if(1 == unFlag)
	{
		//����Ƕ����ӣ��������ͷű�־
		//if(m_unInlineTime)
		//	m_bAllowFree = TRUE;
	}else
	//�������ر�
	if(2 == unFlag)
	{
		//�����ͷű�־
		m_bAllowFree = TRUE;
	}else
	//����ʱ�䵽
	if(3 == unFlag)
		//����̬ʱ�䷢��ģʽ
		if((!m_unInlineTime) && (3 == m_pOwner->m_unSendFlag))
			SendPack(1);
	}
}

__forceinline void CConnection::OnSended(unsigned int unSendID, unsigned int unLen)
{
	//�����ֽڼ���
	m_unSendBytes += unLen;
	//��Ͷ����ɷ���ģʽ
	if(2 == m_pOwner->m_unSendFlag)
		SendPack(1);
}

__forceinline void CConnection::OnReceive(unsigned char* pBuf, unsigned int unLen)
{
	//�����ݷ��ط���ģʽ
	if(4 == m_pOwner->m_unSendFlag)
		SendPack(1);

	PPACK_HEAD pDataPacks;
	char* pDataBuf;
	unsigned int unPacksLen, unProcLen;
		
	//�Ƿ��ва�Ϊ�����ꣿ
	if(m_pBufSave)
	{
		//�ϲ�����
		unPacksLen = m_unLenSave + unLen;
		//�����������ȣ���ر�֮
		if(unPacksLen > 1024 * 1024 * 1)
		{
			HeapFree(m_pOwner->m_hHeap, 0, m_pBufSave);
			m_pBufSave = NULL;
			return;
		}
		//���仺��
		pDataPacks = (PPACK_HEAD)HeapAlloc(m_pOwner->m_hHeap, 0, unPacksLen);
		//�Ѿ����ݷ���ǰ��
		CopyMemory(pDataPacks, m_pBufSave, m_unLenSave);
		//�������ݷ��ں��棬��ɺϲ�����
		CopyMemory((char*)pDataPacks + m_unLenSave, pBuf, unLen);
		//�ͷž����ݵĻ���
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
	//�ͷŴ����ꣿ
	while(unProcLen < unPacksLen)
	{
		//ͷ4���ֽ��Ƿ��ǰ�ͷ��־
		if(HEAD_FLAG != pDataPacks->unHeadFlag)
		{
			//���»��壿
			if((unsigned char*)pDataBuf != pBuf)
				HeapFree(m_pOwner->m_hHeap, 0, pDataBuf);
			return;
		}
		//����δ�ﵽ��������
		if(pDataPacks->unLen > unPacksLen - unProcLen)
		{
			break;
		}
		//��֤Ч���־
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
		//��������
		m_fSpeed = (float)(pDataPacks->unLen * 2) * 1000 / m_unTime;
		//ƫ��
		unProcLen += pDataPacks->unLen;
		pDataPacks = PPACK_HEAD((char*)pDataPacks + pDataPacks->unLen);
	}
	//��������δ�����꣬�򱣴�֮������֮��ϲ�
	if(unProcLen < unPacksLen)
	{
		m_unLenSave = unPacksLen - unProcLen;
		m_pBufSave = (char*)HeapAlloc(m_pOwner->m_hHeap, 0, m_unLenSave);
		CopyMemory(m_pBufSave, (char*)pDataPacks, m_unLenSave);
	}
	//���»��壿
	if((unsigned char*)pDataBuf != pBuf)
		HeapFree(m_pOwner->m_hHeap, 0, pDataBuf);
}

__forceinline void CConnection::SendPack(unsigned int unSendCount)
{
	//��������иı�
	if(m_unPackSize != m_pOwner->m_unSendPacketSize)
	{
		HeapFree(m_pOwner->m_hHeap, 0, m_pPackHead);
		m_unPackSize = m_pOwner->m_unSendPacketSize;
		m_pPackHead = (PPACK_HEAD)HeapAlloc(m_pOwner->m_hHeap, 0, m_unPackSize);
	}
	//���仺��ʧЧ
	if(!m_pPackHead)
		 return;

	unsigned int i;

	//���Ͷ��ٸ�����
	i = unSendCount;
	while(i--)
	{
		//��ͷ��־
		m_pPackHead->unHeadFlag = HEAD_FLAG;
		//����
		m_pPackHead->unLen = m_unPackSize;
		//������
		m_pPackHead->pOwner = this;
		//������ſ�ʼ����
		m_unIndexSend++;
		if(!m_unIndexSend)
			m_unIndexSend = 1;
		//�����
		m_pPackHead->unIndex = m_unIndexSend;
		//��ǰϵͳ����
		m_pPackHead->unTickCount = GetTickCount();
		//��¼���һ�ε�ϵͳ����
		m_unSendTickCount = m_pPackHead->unTickCount;
		//Ч���־
		m_pPackHead->unCRC = GetCRC((unsigned char*)m_pPackHead + sizeof(unsigned int) * 3, m_unPackSize - sizeof(unsigned int) * 3);
		//Ͷ�ݷ���
		GTcpClt_PostSendBuf(m_unPerHandle, (unsigned char*)m_pPackHead, m_unPackSize);
	}
}

__forceinline void CServerTestTool::ConnectionEnqueue(CConnection* pHead, CConnection* pTail)
{
	//��ĳ�����Ӷ��з���δ�����У��ȴ������̻߳�ȡ�������ܵ����Ӷ���
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
	//�����̵߳��ã���δ�����кϲ��������Ӷ���
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
			//����ǳ����ӣ������ڷ�����Ϣ������һ���б��¼
			if(!pTmp->m_unInlineTime)
				PostMessage(m_hWnd, WM_USER + 101, (WPARAM)pHead, 0);
			pTmp = pTmp->m_pNext;
		}
	}
	
	//�ϲ����ܶ���
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
	//ɾ��ĳ������
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
	//���������ʱ	
	pConnection->OnConnected(unPerHandle);
}

__forceinline void CServerTestTool::OnDisconnected(CConnection* pConnection, unsigned int unFlag)
{
	//�����ӶϿ�ʱ
	pConnection->OnDisconnected(unFlag);
	if((!pConnection->m_unInlineTime) && (2 ==unFlag))
		InterlockedDecrement((LONG*)&m_unLongConnectCount);
}

__forceinline void CServerTestTool::OnSended(CConnection* pConnection, unsigned int unSendID, unsigned int unLen)
{
	//���������ʱ
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
	//����������ʱ
	pConnection->OnReceive(pBuf, unLen);

	unsigned int unOld;

	for(;;)
	{
		unOld = m_unRecvBytes;
		if(InterlockedCompareExchange((LONG*)&m_unRecvBytes, unOld + unLen, unOld) == unOld)
			break;
	}
}


