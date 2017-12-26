/********************************************************************************
˵����
��ʾ��������δ�����Ż�Ҳδ���а�ȫ����֤��
��Ϊ��������ģʽ�·�������Ʒ����ϵĽ��ܣ�
������ʵ��Ӧ�û������ˣ�����ʵ��Ӧ���ϸ��д���롣

ע�⣺
��ʾ��������ָ�������ʽ�����������ͣ��ͷ�������Ʒ�����
������ʹ��unPerHandle��ΪKey������������ʽ������������磺HASH MAP��
********************************************************************************/

#include "stdafx.h"
#include "..\..\G-TcpServer.h"

class CUserInfo;
void __stdcall Callback_OnThread(unsigned int unThreadContext, unsigned int unThreadHandle, unsigned int unThreadID, BOOL bIsBegin, unsigned int unFlag);
void __stdcall Callback_OnConnected(unsigned int unPerHandle, char* pBuf, unsigned int unLen);
void __stdcall Callback_OnReceived(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo);
void __stdcall Callback_OnSended(unsigned int unPerHandle, unsigned int unSendID, unsigned int unLen, CUserInfo* pUserInfo);
void __stdcall Callback_OnDisconnected(unsigned int unPerHandle, unsigned int unFlag, CUserInfo* pUserInfo);

class CLock
{
private:
	CRITICAL_SECTION	cs;
public:
	CLock(void)
	{
		InitializeCriticalSection(&cs);
	}
	~CLock(void)
	{
		DeleteCriticalSection(&cs);
	}
	inline void Lock()
	{
		EnterCriticalSection(&cs);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&cs);
	}
};

#define	CMD_PACK_HEAD	0xFFFFFFF1
typedef struct _DATA_PACK_INFO
{
unsigned int unCmd;
unsigned int unLen;
unsigned int unCRC;
}DATA_PACK_INFO, *PDATA_PACK_INFO;
//��ʽ��DataPack = PackInfo + Data
//���ȣ�PackLen  = sizeof(DATA_PACK_INFO) + DataLen = PackInfo.unLen;

inline unsigned int GetCRC(char* pBuf, unsigned int unLen)
{
	//���������CRC
	//...
	return(0);
}

class CHall;
//�������װ���ݷ��ͺ��������������ݰ���ʽ�������ݷ��
class CUserInfo : public CLock
{
private:
	CHall* m_pHall;
	CString m_strUserName;
	unsigned int m_unUserID;
	unsigned int m_unPerHandle;
	char* m_pBufSave;
	unsigned int m_unLenSave;

	//��ģ�鷵�ص�GBuf��ַ����ǰ����sizeof(DATA_PACK_INFO) - 1�ֽڣ�����ǰ��Ŀռ���Ϊ���ݰ�ͷ
	PGBUF AllocGBuf(){return((unsigned char*)GTcpSvr_AllocGBuf() + sizeof(DATA_PACK_INFO));};
	//��GBuf��ַ���������sizeof(DATA_PACK_INFO) - 1�ֽڣ����ܵõ�ģ�鷵�ص�GBuf��ַ��
	//��UserInfo�ĳ�Ա����AllocGBuf()�����GBuf������UserInfo�ĳ�Ա����FreeGbuf�ͷ�
	inline void FreeGBuf(PGBUF pGBuf){GTcpSvr_FreeGBuf((unsigned char*)pGBuf - sizeof(DATA_PACK_INFO));};
protected:
public:
	CUserInfo(void)
	{
		m_pBufSave = NULL;
		m_unLenSave = 0;
		m_strUserName.Format(_T(""));
		m_unUserID = 0;
		m_unPerHandle = 0;
	}
	~CUserInfo(void)
	{
		if(m_pBufSave)
			HeapFree(GetProcessHeap(), 0, m_pBufSave);
	};
	inline void SetPerHandle(unsigned int unPerHandle){m_unPerHandle = unPerHandle;};
	inline void SetUserName(char* pszUserName){m_strUserName.Format(_T("%d"), pszUserName);};
	inline void SetUserID(unsigned int unUserID){m_unUserID = unUserID;};
	inline unsigned int GetUserID(){return(m_unUserID);}
	inline unsigned int GetPerHandle(){return(m_unPerHandle);}
	inline char* GetBufSave(void){return(m_pBufSave);};
	inline unsigned int GetLenSave(void){return(m_unLenSave);};
	inline void SetBufSave(char* pBuf){m_pBufSave = pBuf;};
	inline void SetLenSave(unsigned int unLen){m_unLenSave = unLen;};
	inline void Close(void)
	{
		unsigned int unPH;
		Lock();
		unPH = m_unPerHandle;
		m_unPerHandle = 0;
		Unlock();
		GTcpSvr_CloseClient(unPH);
	};
	inline void OnConnect()
	{
		//��;���ߣ������Ӻ������ߴ���Ĺ���
		if(m_pBufSave)
			HeapFree(GetProcessHeap(), 0, m_pBufSave);
		m_pBufSave = NULL;
	};
	inline void OnDisconnect()
	{
		//�Ͽ�ʱm_unPerHandle��������Ϊ0
		Lock();
		m_unPerHandle = 0;
		Unlock();
		if(m_pBufSave)
			HeapFree(GetProcessHeap(), 0, m_pBufSave);
		m_pBufSave = NULL;
	};
	//��������
	inline void SendGBuf(PGBUF pGBuf, unsigned int unLen)
	{
		if(GetGBufSize() < unLen)
		{
			FreeGBuf(pGBuf);
			return;
		}
		PDATA_PACK_INFO pPack;
		//�������(sizeof(DATA_PACK_INFO) - 1)���õ���ͷ��ַ����ģ�鷵�ص�GBufʵ�ʵ�ַ
		pPack = PDATA_PACK_INFO((char*)pGBuf - sizeof(DATA_PACK_INFO));
		pPack->unCmd = CMD_PACK_HEAD;
		pPack->unLen = unLen + sizeof(DATA_PACK_INFO);
		pPack->unCRC = GetCRC((char*)pGBuf, unLen);
		//�Ͽ�������
		Lock();
		if(m_unPerHandle)
		{
			GTcpSvr_PostSendBuf(m_unPerHandle, (unsigned char*)pPack, pPack->unLen);
			Unlock();
		}else
		{
			Unlock();
			FreeGBuf(pGBuf);
		}		
	}
	inline unsigned int GetGBufSize(){return(GTcpSvr_GetGBufSize() - sizeof(DATA_PACK_INFO));};
	inline void SendBuf(char* pBuf, unsigned int unLen)
	{
		PGBUF pGBuf;

		//�������GBuf����ģ���ְ�����
		Lock();
		while(GetGBufSize() < unLen)
		{
			pGBuf = AllocGBuf();
			if(!pGBuf)
				break;
			//���Ƶ�GBuf��
			CopyMemory(pGBuf, pBuf, unLen);
			SendGBuf(pGBuf, GetGBufSize());
			unLen -= GetGBufSize();
			pBuf += GetGBufSize();
		}
		if(!unLen)
		{
			Unlock();
			return;
		}
		pGBuf = AllocGBuf();
		if(pGBuf)
		{
			//���Ƶ�GBuf��
			CopyMemory(pGBuf, pBuf, unLen);
			SendGBuf(pGBuf, unLen);
		}
		Unlock();
	}
};

class CUserInterface : public CLock
{
private:
	CList<CUserInfo*> m_lstUserInfos;	//�÷��������û��б�
protected:
public:
	CUserInterface(void){};
	~CUserInterface(void){};

	inline void Broadcast(char* pBuf, unsigned int unLen)
	{
		POSITION Pos;
		CUserInfo* pUserInfo;

		Lock();
		if(m_lstUserInfos.GetCount())
		{
			Pos = m_lstUserInfos.GetHeadPosition();
			pUserInfo = m_lstUserInfos.GetAt(Pos);
			while(pUserInfo)
			{
				pUserInfo->SendBuf(pBuf, unLen);
				pUserInfo = m_lstUserInfos.GetNext(Pos);
			}
		}
		Unlock();
	}

	//����ʽ���ң�ʵ�ʿ����У�Ӧ�ÿ������������㷨
	inline CUserInfo* FindUpdateUserInfo(unsigned int unPerHandle, unsigned int unUserID)
	{
		POSITION Pos;
		CUserInfo* pUserInfo;

		Lock();
		if(m_lstUserInfos.GetCount())
		{
			Pos = m_lstUserInfos.GetHeadPosition();
			pUserInfo = m_lstUserInfos.GetAt(Pos);
			while(pUserInfo)
			{
				if(pUserInfo->GetUserID() == unUserID)
				{
					pUserInfo->SetPerHandle(unPerHandle);
					Unlock();
					return(pUserInfo);
				}
				pUserInfo = m_lstUserInfos.GetNext(Pos);
			}
		}
		Unlock();
		return(NULL);
	}

	inline void DeleteUser(CUserInfo* pDelUserInfo)
	{
		POSITION Pos;
		CUserInfo* pUserInfo;

		Lock();
		if(m_lstUserInfos.GetCount())
		{
			Pos = m_lstUserInfos.GetHeadPosition();
			pUserInfo = m_lstUserInfos.GetAt(Pos);
			while(pUserInfo)
			{
				if(pUserInfo == pDelUserInfo)
				{
					m_lstUserInfos.RemoveAt(Pos);
					Unlock();
					return;
				}
				pUserInfo = m_lstUserInfos.GetNext(Pos);
			}
		}
		Unlock();
	}

	inline BOOL CheckDeleteUser(unsigned int unPerHandle, CUserInfo* pDelUserInfo)
	{
		POSITION Pos;
		CUserInfo* pUserInfo;
		
		Lock();
		if(m_lstUserInfos.GetCount())
		{
			Pos = m_lstUserInfos.GetHeadPosition();
			pUserInfo = m_lstUserInfos.GetAt(Pos);
			while(pUserInfo)
			{
				if((pUserInfo == pDelUserInfo) && (pUserInfo->GetPerHandle() == unPerHandle))
				{
					m_lstUserInfos.RemoveAt(Pos);
					Unlock();
					return(TRUE);
				}
				pUserInfo = m_lstUserInfos.GetNext(Pos);
			}
		}
		Unlock();
		return(FALSE);
	}

	inline void InsertUser(CUserInfo* pUserInfo)
	{
		Lock();
		m_lstUserInfos.AddTail(pUserInfo);
		Unlock();
	}
};

class CHall : public CUserInterface
{
private:
protected:
public:
	CHall(void)
	{
		if(GTcpSvr_Start(1000, 65,5, &Callback_OnThread))
		{
			GTcpSvr_OpenListen(_T("127.0.0.1"), _T("6800"), 
								(PGFN_ON_CONNECTED)&Callback_OnConnected,
								(PGFN_ON_RECEIVED)&Callback_OnReceived,
								(PGFN_ON_SENDED)&Callback_OnSended,
								(PGFN_ON_DISCONNECTED)&Callback_OnDisconnected,
								this);
		}
	};
	~CHall(void){};

	BOOL CheckLoginData(char* pBuf, unsigned int unLen)
	{
		//...
		return(TRUE);
	}

	inline unsigned int CheckUserName(char* pszUserName)
	{
		//����������ݿ���в�ѯ��Ч����û��Ƿ���ڣ������򷵻ظ��û���ID��
		return(0);
	}

	inline BOOL OnLogin(unsigned int unPerHandle, char* pBuf, unsigned int unLen)
	{
		typedef struct _LOGIN_INFO
		{
		unsigned int unCmd;
		char		 cName[1];
		}LOGIN_INFO, *PLOGIN_INFO;

		unsigned int unUserID;
		CUserInfo* pUserInfo;

		if(!CheckLoginData(pBuf, unLen))
			return(FALSE);

		unUserID = CheckUserName(PLOGIN_INFO(pBuf)->cName);
		//��Ч�û�����ر�
		if(!unUserID)
			return(FALSE);

		pUserInfo = FindUpdateUserInfo(unPerHandle, unUserID);
		//�Ƿ���ǰ�Ѿ���¼����;�������쳣�Ͽ���
		if(pUserInfo)
		{
			//���øÿͻ������Ĺ�����UserInfo��
			GTcpSvr_SetPerHandleOwner(unPerHandle, pUserInfo);
			//����������߱�־
			pUserInfo->OnConnect();
		}else
		{
			//�µ�¼���û�
			pUserInfo = new CUserInfo;
			pUserInfo->SetUserID(unUserID);
			pUserInfo->SetUserName(PLOGIN_INFO(pBuf)->cName);
			//���øÿͻ������Ĺ�����UserInfo��
			GTcpSvr_SetPerHandleOwner(unPerHandle, pUserInfo);
			//UserInfo�����ÿͻ�������
			pUserInfo->SetPerHandle(unPerHandle);
			//�����û�����
			InsertUser(pUserInfo);
		}
		//������������������
		//...
		return(TRUE);
	}

	inline void OnLogout(CUserInfo* pUserInfo, char* pBuf, unsigned int unLen)
	{
		pUserInfo->Close();
		DeleteUser(pUserInfo);	//����Ǳ������������رյģ������������
		delete pUserInfo;
	}

	#define CMD_LOGIN		0xFFFFFFF1
	#define CMD_LOGOUT		0xFFFFFFF2

	inline void OnConnected(unsigned int unPerHandle, char* pBuf, unsigned int unLen)
	{
		//���Ӻ��յ��ĵ�һ��������Ϊ��¼��Ϣ�������¼��֤ʧ�ܣ�Ӧ�ùرո�����
		if((0xFFFFFFF1 != *(unsigned int*)pBuf) || (!OnLogin(unPerHandle, pBuf, unLen)))
			GTcpSvr_CloseClient(unPerHandle);
	}

	inline void OnReceive(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo)
	{		
		switch(*(unsigned int*)pBuf)
		{
		case 0xFFFFFFF2:
			OnLogout(pUserInfo, pBuf, unLen);
			break;
		}
	}

	inline void OnSended(unsigned int unPerHandle, unsigned int unSendID, unsigned int unLen, CUserInfo* pUserInfo)
	{
		//�����ﴦ�����ݷ��ͳɹ��Ժ���ƺ�
	}

	inline void OnDisconnect(unsigned int unPerHandle, unsigned int unFlag, CUserInfo* pUserInfo)
	{
		if(1 == unFlag)
			pUserInfo->OnDisconnect();				//�ͻ��쳣���ߣ��Դ�ֻ������ض��߱�־
		else
		//�����������ʱ��������У������������رո�����
		//����������ʱ��ʱ���൱����������������п��ܿͻ��Ѿ����µ�¼���ߵ�¼�Ժ󷢳�Logoutָ�
		//��ԭ�������ӻ�û�жϿ���ֱ����ʱʱ�䵽���������pUserInfo�Ĳ�һ���߱���Ч�ԣ������Ҫ��֤
		if((3 == unFlag) && CheckDeleteUser(unPerHandle, pUserInfo))
			delete pUserInfo;	//�Ӷ��н���Ժ󣬲��ܰ�ȫ�ͷ�
	}

	//���ݰ���֣�����ճ����
	inline void DataPackProcesser(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo)
	{
		PDATA_PACK_INFO pDataPacks;
		char* pDataBuf;
		unsigned int unPacksLen, unProcLen;
		
		//�ϴ��ва���
		if(pUserInfo->GetBufSave())
		{
			//�����ܳ���
			unPacksLen = pUserInfo->GetLenSave() + unLen;
			//���������ĳ��ȵģ���ô���ݿ϶������⣬�������ݰ��ܳ��ȴ�����������ݰ�֮�ͣ���ô�϶�������
			if(unPacksLen > 1024 * 1024 * 1)//����Ĭ��1M
			{
				//�ر��쳣����
				pUserInfo->Close();
				HeapFree(GetProcessHeap(), 0, pUserInfo->GetBufSave());
				pUserInfo->SetBufSave(NULL);
				return;
			}
			//�ϲ���
			pDataPacks = (PDATA_PACK_INFO)HeapAlloc(GetProcessHeap(), 0, unPacksLen);
			CopyMemory(pDataPacks, pUserInfo->GetBufSave(), pUserInfo->GetLenSave());
			CopyMemory((char*)pDataPacks + pUserInfo->GetLenSave(), pBuf, unLen);
			HeapFree(GetProcessHeap(), 0, pUserInfo->GetBufSave());
			pUserInfo->SetBufSave(NULL);
			pDataBuf = (char*)pDataPacks;
		}else
		{
			pDataPacks = (PDATA_PACK_INFO)pBuf;
			unPacksLen = unLen;
			pDataBuf = (char*)pDataPacks;
		}
		unProcLen = 0;
		//
		while(unProcLen < unPacksLen)
		{
			//ͷ�ĸ��ֽڲ��ǹ涨������ͷ��־���ر�����
			if(CMD_PACK_HEAD != pDataPacks->unCmd)
			{	
				//�ر��쳣����
				pUserInfo->Close();
				//�Ƿ��Ǻϲ��õĻ��壬�����ͷ�
				if((char*)pDataBuf != pBuf)
					HeapFree(GetProcessHeap(), 0, pDataBuf);
				return;
			}
			//�����ȴ���ʵ�ʽ��ճ��ȵģ�˵���ǲа����˳�while֮�󱣴�
			if(pDataPacks->unLen > unPacksLen - unProcLen)
			{
				break;
			}
			if(GetCRC((char*)&pDataPacks->unCRC, pDataPacks->unLen - sizeof(DATA_PACK_INFO)) != pDataPacks->unCRC)
			{
				//�ر��쳣����
				pUserInfo->Close();
				//�Ƿ��Ǻϲ��õĻ��壬�����ͷ�
				if((char*)pDataBuf != pBuf)
					HeapFree(GetProcessHeap(), 0, pDataBuf);
				return;
			}
			//��ȡ��Ч���ݣ����ô�����
			OnReceive(unPerHandle, (char*)&pDataPacks->unCRC, pDataPacks->unLen - sizeof(DATA_PACK_INFO), pUserInfo);
			//��ǰ����pDataPacks->unLen�ֽ�
			unProcLen += pDataPacks->unLen;
			pDataPacks = PDATA_PACK_INFO((char*)pDataPacks + pDataPacks->unLen);
		}
		//��δ�����������
		if(unProcLen < unPacksLen)
		{
			//δ��������ݳ���
			pUserInfo->SetLenSave(unPacksLen - unProcLen);
			//�ٴη���һ������
			pUserInfo->SetBufSave((char*)HeapAlloc(GetProcessHeap(), 0, pUserInfo->GetLenSave()));
			//����δ��������ݵ���������
			CopyMemory(pUserInfo->GetBufSave(), (char*)pDataPacks, pUserInfo->GetLenSave());			
		}
		//�Ƿ��Ǻϲ��õĻ��壬�����ͷ�
		if((char*)pDataBuf != pBuf)
			HeapFree(GetProcessHeap(), 0, pDataBuf);
	}
};