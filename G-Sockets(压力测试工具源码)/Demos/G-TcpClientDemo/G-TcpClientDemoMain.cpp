/********************************************************************************
								���Թ�����Ҫģ��
********************************************************************************/

#include "stdafx.h"
#include "G-TcpClientDemoMain.h"
#include "..\..\G-TcpClient.h"
#include "..\G-TcpCltRunInfo\G-TcpCltRunInfo.h"

/********************************************************************************
				������ G-TcpClient �¼��ص�����
********************************************************************************/
void __stdcall GTcpClt_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
{
	//�̸߳տ�ʼ����
	if(bIsBegin)
	{
		//��������Ϣ��������һ���߳��б�
		GTcpCltInfo_AddThread(unThreadContext, unThreadHandle, unThreadID);
		//�Ǵ����߳�
		if(1 == unFlag)
		{
			//�����̹߳������Ƚϴ�Ӧ�ø��ڹ����߳�
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
			//��SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//ֻ�д����̲߳Ż���ûص�������Ӧ���ӵ������¼������п�����Ҫ��ʼ���̣߳�
			//��ʹ�����ݿ������������Ҫ�߳��ڶ����Ĳ���
			CoInitialize(NULL);
		}else
		//�ǹ����߳�
		if(2 == unFlag)
		{
			//�����߳�Ȩ�޽��ڿ����̺߳ʹ����߳�֮��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}else
		//�ǿ����߳�
		if(3 == unFlag)
		{
			//�����߳�û�б�Ҫ����̫��Ȩ��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_unORMAL);
		}
	}else
	//�߳̽���
	{
		//�Ǵ����߳�
		if(1 == unFlag)
		{
			//����ʼ��
			CoUninitialize();
			//ȡ���߳�ģʽ
			//��SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_END);
		}else
		//�ǹ����߳�
		if(2 == unFlag)
		{
			
		}else
		//�ǿ����߳�
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
				������ G-TcpClient �¼��ص�����
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
		//������û���µ�������δ������
		pServerTestTool->AddConnectionFromPendingQueue();
		//����ǹرղ����Ѿ��ͷ�����������
		if(pServerTestTool->m_bIsCloseSendThread && (!pServerTestTool->m_pConnections))
			break;
		dwTickCount = GetTickCount();
		//�Ƿ�رգ�
		if(!pServerTestTool->m_bIsCloseSendThread)
		{//���ǹر�
			pConnection = pServerTestTool->m_pConnections;
			while(pConnection)
			{
				//�Ѿ����ر���
				if(pConnection->m_bAllowFree)
				{
					pTmp = pConnection;
					pConnection = pServerTestTool->DeleteConnection(pConnection);
					delete pTmp;
					continue;
				}else
				if((pConnection->m_unInlineTime) && pConnection->m_unSendBytes &&
				   ((1 == pConnection->m_unInlineTime) || (dwTickCount - pConnection->m_unConnectTickCount >= pConnection->m_unInlineTime)))
				{//����Ƕ����ӣ����ҳɹ����͵����ݣ�����ʱ��ﵽ�涨��ʱ�䣬��ر�֮
					pConnection->m_unSendBytes = 0;
					GTcpClt_CloseConnect(pConnection->m_unPerHandle);
				}else
				if(1 == pServerTestTool->m_unSendFlag)
				{//��ʱ����ģʽ
					if(HNDS_CONNECTED == GTcpClt_GetState(pConnection->m_unPerHandle))
					{
						//��ʱ����ģʽ���ҷ���ʱ�䵽��
						if(dwTickCount - pConnection->m_unSendTickCount >= pServerTestTool->m_unSendSpaceTimeOfOnce)
						{
							//��ʱ����ģʽ�������趨���������ݰ�
							pConnection->SendPack(pServerTestTool->m_unSendPacketNumberOfOnce);
						}
					}
				}
				pConnection = pConnection->m_pNext;
			}
			Sleep(1);
		}else		
		{//�ǹر�
			if(bIsClose != pServerTestTool->m_bIsCloseSendThread)
			{//��һ�λ�ùر��ź�
				bIsClose = pServerTestTool->m_bIsCloseSendThread;
				pConnection = pServerTestTool->m_pConnections;
				//������Ͷ�ݹر�����
				while(pConnection)
				{
					GTcpClt_CloseConnect(pConnection->m_unPerHandle);
					pConnection = pConnection->m_pNext;
				}
			}
			pConnection = pServerTestTool->m_pConnections;
			//�������У�������û���Ѿ���ɹرչ����ģ�����ɾ���ڵ���ͷ��ڴ�
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
		//����ģʽ�ı�Ϊ�Ƕ�ʱ����ʱ��������������ӷ���һ�����ݰ����Դ�����Ӧ�����ﵽ�趨�ķ���ģʽ
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
	//���Ѿ���ȫ�˳���
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
		//ÿ�η�����ٸ�����
		unCount = pConnectInfo->unConnectNumberOfOnce;
		while(unCount--)
		{
			//�µ����Ӷ���
			pTmp = new CConnection(pConnectInfo->pOwner,
								   pConnectInfo->pOwner->m_unSendPacketSize, 
								   pConnectInfo->unInlineTime);
			if(!pTmp)
				break;
			pTmp->m_unInlineTime = pConnectInfo->unInlineTime;
			//Ͷ����������
			pTmp->m_unPerHandle = GTcpClt_OpenConnect(pConnectInfo->szRIP,
														pConnectInfo->szRPort,
														pConnectInfo->szLIP,
														(PGFN_ON_CONNECTED)&GTcpClt_OnConnected,
														(PGFN_ON_RECEIVED)&GTcpClt_OnReceived,
														(PGFN_ON_SENDED)&GTcpClt_OnSended,
														(PGFN_ON_DISCONNECTED)&GTcpClt_OnDisconnected,
														pTmp);
			//����ʧ��
			if(!pTmp->m_unPerHandle)
			{
				delete pTmp;
				if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
					break;
				continue;
			}
			//���뵽�������ӵ�С�ζ�����
			//��Ϊ��ֹ�����̱߳���ʱ���������ӵ��ܶ��еȴ�ʱ���������������һ��С���У�
			//�ٷ���δ��������Ա㷢���̻߳�ȡ�ϲ����ܶ��У�
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
			//�������������
			if(pConnectInfo->unConnectTotal)
			{
				unSum++;
				//���������������
				if(unSum >= pConnectInfo->unConnectTotal)
				{
					//���������ӵ�С���з���δ��������Ա㷢���̴߳���
					if(pHead)
						pConnectInfo->pOwner->ConnectionEnqueue(pHead, pTail);
					goto End;
				}
			}
			//����ǹر�
			if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
				break;
		}
		if(pHead)
			//���������ӵ�С���з���δ��������Ա㷢���̴߳���
			pConnectInfo->pOwner->ConnectionEnqueue(pHead, pTail);
		//����ǹر�
		if(pConnectInfo->pOwner->m_bIsCloseConnectThread)
			break;
		//����һ��ʱ���ٷ�������
		Sleep(pConnectInfo->unConnectSpaceTimeOfOnce);
	}
End:
	//�ǲ������һ���˳��������̣߳�
	if(0 == InterlockedDecrement((LONG*)&pConnectInfo->pOwner->m_unConnectThreadCount))
		//�Ұ�ȫ�˳���
		SetEvent(pConnectInfo->pOwner->m_hConnectThreadEnd);
	//�ͷ��ڴ�
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
	//����ǵ�һ�������������߳�
	if(1 == InterlockedIncrement((LONG*)&m_unConnectThreadCount))
		//�����¼�״̬
		ResetEvent(m_hConnectThreadEnd);

	unsigned int unID;

	//���������߳�
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
