/********************************************************************************
˵����
��ʾ��������δ�����Ż�Ҳδ���а�ȫ����֤��
��Ϊ��������ģʽ�·�������Ʒ����ϵĽ��ܣ�
������ʵ��Ӧ�û������ˣ�����ʵ��Ӧ���ϸ��д���롣
********************************************************************************/

#include "stdafx.h"
#include "ServerDemo.h"

/********************************************************************************
				������ GTcpServer �¼��ص�����
********************************************************************************/
void __stdcall Callback_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag)
//˵�����߳̿�ʼ�ͽ���ʱ���߳����ڵĻص����������������������߳�Ȩ�޺ͽ����߳���صĳ�ʼ��������
//���룺hThreadHandle���߳̾��
//		nThreadID���߳�ID
//		bIsBegin��ΪTRUE��ʾ�̸߳տ�ʼ���У�������ʾ�߳̽����ˡ�
//		unFlag��1��Ϊ���ݴ������̣߳�2��ΪIO�������̣߳�3��Ϊ�����̣߳�����δ���塣
//�������
//ע�⣺�߳�Ȩ�����ÿɸ��ݴ����̡߳��������̡߳��ŷ��̵߳Ľ�ɫ�����ã�����߳��ڲ���Ҫ�ĳ�ʼ�����������Ը���
//		unFlagΪ1��bIsBeginΪ��ʱ���г�ʼ�����������磺CoInitialize(nil)��unFlagΪ1��bIsBeginΪ�ٽ��з���ʼ��
//		���������磺CoUninitialize()��
{
	//�̸߳տ�ʼ����
	if(bIsBegin)
	{
		//�Ǵ����߳�
		if(1 == unFlag)
		{
			//�����̹߳������Ƚϴ�Ӧ�ø��ڹ����߳�
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
			//��SetThreadPriority(hThreadHandle, THREAD_MODE_BACKGROUND_BEGIN);

			//ֻ�д����̲߳Ż���ûص�������Ӧ���ӵ������¼������п�����Ҫ��ʼ���̣߳�
			//��ʹ�����ݿ������������Ҫ�߳��ڶ����Ĳ���
			CoInitialize(NULL);
		}else
		//�ǹ����߳�
		if(2 == unFlag)
		{
			//�����߳�Ȩ�޽��ڿ����̺߳ʹ����߳�֮��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
		}else
		//�ǿ����߳�
		if(3 == unFlag)
		{
			//�����߳�û�б�Ҫ����̫��Ȩ��
			//SetThreadPriority(hThreadHandle, THREAD_PRIORITY_NORMAL);
		}
	}else
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

void __stdcall Callback_OnConnected(unsigned int unPerHandle, char* pBuf, unsigned int unLen)
//˵�����ͻ����Ӻ�Ļص�����
//���룺unPerHandle��ÿ�����ӿͻ������ģ�ÿ��������Ψһ
//		pBuf�����յ����ݻ�������ַָ�룬
//		nLen���������ݻ������ĳ��ȣ����������ݵĴ�С��
//�������
//ע�⣺������pBuf����д���κ����ݣ�nLen��󲻳���Callback_GetGBufSize����ֵ����������
//		�η�ʽ�ͷ�pBuf�����ص�����ֻ�ڴ������߳����ڱ����ã�������ڴ������߳̿�ʼ�ͽ�
//		��ʱ������س�ʼ���ͷ���ʼ��������unPerHandle��������OnDisconnect��
//		OnOvertime����֮�󽫽������˺��unPerHandle�Ĳ���������Ч��
{
	((CHall*)GTcpSvr_GetListenOwner(unPerHandle))->OnConnected(unPerHandle, pBuf, unLen);
}

void __stdcall Callback_OnReceived(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo)
//˵�����ͻ�������ӦOnConnect���յ��ͻ����ݵĻص�����
//		pUserInfo��CUserInfo��ָ��
//		pBuf�����յ����ݻ�������ַָ�룬
//		nLen���������ݻ������ĳ��ȣ����������ݵĴ�С��
//�������
//ע�⣺������pBuf����д���κ����ݣ�nLen��󲻳���Callback_GetGBufSize����ֵ����������
//		�η�ʽ�ͷ�pBuf�����ص�����ֻ�ڴ������߳����ڱ����ã�������ڴ������߳̿�ʼ�ͽ�
//		��ʱ������س�ʼ���ͷ���ʼ��������
{
	((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->DataPackProcesser(unPerHandle, pBuf, unLen, pUserInfo);
}

void __stdcall Callback_OnSended(unsigned int unPerHandle, unsigned int unSendID, unsigned int unLen, CUserInfo* pUserInfo)
//˵����ÿ�ʷ���Ͷ���Ѿ������͵��Է���·��֮��ص�
//		pUserInfo��CUserInfo��ָ��
//		unSendID����PostSendBuf��PostSendGBuf����ֵһ��
//		unLen���Ѿ�������ϵ��ֽ���
//�������
{
	((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->OnSended(unPerHandle, unSendID, unLen, pUserInfo);
}

void __stdcall Callback_OnDisconnected(unsigned int unPerHandle, unsigned int unFlag, CUserInfo* pUserInfo)
//˵�����ͻ��Ͽ���Ļص�����
//		pUserInfo��CUserInfo��ָ��
//		bIsClose����Ϊ��������ǿ�ƹرգ�����CloseClient������Ϊ�ͻ��˿ڶϿ���������ϵ��¶Ͽ�
//�������
//ע�⣺ʹ��Callback_CloseClient����ص��˺���
{
	if(pUserInfo)
		((CHall*)GTcpSvr_GetListenOwner(pUserInfo->GetPerHandle()))->OnDisconnect(unPerHandle, unFlag, pUserInfo);
}
/********************************************************************************
				������ GTcpServer �¼��ص�����
********************************************************************************/