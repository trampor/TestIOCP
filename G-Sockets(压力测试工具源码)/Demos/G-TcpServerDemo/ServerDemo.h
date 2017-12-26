/********************************************************************************
说明：
本示例代码尚未进行优化也未进行安全性验证，
仅为主动发送模式下服务器设计方法上的介绍，
不等于实际应该或必须如此，请结合实际应用严格编写代码。

注意：
本示例代码是指针关联方式的主动（发送）型服务器设计方法，
您可以使用unPerHandle作为Key来采用其他方式互相关联，诸如：HASH MAP等
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
//格式：DataPack = PackInfo + Data
//长度：PackLen  = sizeof(DATA_PACK_INFO) + DataLen = PackInfo.unLen;

inline unsigned int GetCRC(char* pBuf, unsigned int unLen)
{
	//在这里计算CRC
	//...
	return(0);
}

class CHall;
//在类里封装数据发送函数，并定义数据包格式进行数据封包
class CUserInfo : public CLock
{
private:
	CHall* m_pHall;
	CString m_strUserName;
	unsigned int m_unUserID;
	unsigned int m_unPerHandle;
	char* m_pBufSave;
	unsigned int m_unLenSave;

	//将模块返回的GBuf地址在向前推移sizeof(DATA_PACK_INFO) - 1字节，留下前面的空间作为数据包头
	PGBUF AllocGBuf(){return((unsigned char*)GTcpSvr_AllocGBuf() + sizeof(DATA_PACK_INFO));};
	//将GBuf地址在向后推移sizeof(DATA_PACK_INFO) - 1字节，才能得到模块返回的GBuf地址，
	//用UserInfo的成员函数AllocGBuf()分配的GBuf必须用UserInfo的成员函数FreeGbuf释放
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
		//中途断线，再连接后做上线处理的工作
		if(m_pBufSave)
			HeapFree(GetProcessHeap(), 0, m_pBufSave);
		m_pBufSave = NULL;
	};
	inline void OnDisconnect()
	{
		//断开时m_unPerHandle必须设置为0
		Lock();
		m_unPerHandle = 0;
		Unlock();
		if(m_pBufSave)
			HeapFree(GetProcessHeap(), 0, m_pBufSave);
		m_pBufSave = NULL;
	};
	//在这里封包
	inline void SendGBuf(PGBUF pGBuf, unsigned int unLen)
	{
		if(GetGBufSize() < unLen)
		{
			FreeGBuf(pGBuf);
			return;
		}
		PDATA_PACK_INFO pPack;
		//向后推移(sizeof(DATA_PACK_INFO) - 1)，得到包头地址，即模块返回的GBuf实际地址
		pPack = PDATA_PACK_INFO((char*)pGBuf - sizeof(DATA_PACK_INFO));
		pPack->unCmd = CMD_PACK_HEAD;
		pPack->unLen = unLen + sizeof(DATA_PACK_INFO);
		pPack->unCRC = GetCRC((char*)pGBuf, unLen);
		//断开不发送
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

		//如果大于GBuf缓冲的，则分包发送
		Lock();
		while(GetGBufSize() < unLen)
		{
			pGBuf = AllocGBuf();
			if(!pGBuf)
				break;
			//复制到GBuf中
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
			//复制到GBuf中
			CopyMemory(pGBuf, pBuf, unLen);
			SendGBuf(pGBuf, unLen);
		}
		Unlock();
	}
};

class CUserInterface : public CLock
{
private:
	CList<CUserInfo*> m_lstUserInfos;	//该房间所有用户列表
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

	//遍历式查找，实际开发中，应该考虑其他查找算法
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
		//在这里对数据库进行查询，效验该用户是否存在，存在则返回该用户的ID号
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
		//无效用户名则关闭
		if(!unUserID)
			return(FALSE);

		pUserInfo = FindUpdateUserInfo(unPerHandle, unUserID);
		//是否先前已经登录，中途被网络异常断开？
		if(pUserInfo)
		{
			//设置该客户上下文关联向UserInfo，
			GTcpSvr_SetPerHandleOwner(unPerHandle, pUserInfo);
			//设置相关在线标志
			pUserInfo->OnConnect();
		}else
		{
			//新登录的用户
			pUserInfo = new CUserInfo;
			pUserInfo->SetUserID(unUserID);
			pUserInfo->SetUserName(PLOGIN_INFO(pBuf)->cName);
			//设置该客户上下文关联向UserInfo，
			GTcpSvr_SetPerHandleOwner(unPerHandle, pUserInfo);
			//UserInfo关联该客户上下文
			pUserInfo->SetPerHandle(unPerHandle);
			//加入用户队列
			InsertUser(pUserInfo);
		}
		//在这里再做其他操作
		//...
		return(TRUE);
	}

	inline void OnLogout(CUserInfo* pUserInfo, char* pBuf, unsigned int unLen)
	{
		pUserInfo->Close();
		DeleteUser(pUserInfo);	//如果是被服务器主动关闭的，则清除出队列
		delete pUserInfo;
	}

	#define CMD_LOGIN		0xFFFFFFF1
	#define CMD_LOGOUT		0xFFFFFFF2

	inline void OnConnected(unsigned int unPerHandle, char* pBuf, unsigned int unLen)
	{
		//连接后收到的第一笔数据作为登录信息，如果登录验证失败，应该关闭该连接
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
		//在这里处理数据发送成功以后的善后
	}

	inline void OnDisconnect(unsigned int unPerHandle, unsigned int unFlag, CUserInfo* pUserInfo)
	{
		if(1 == unFlag)
			pUserInfo->OnDisconnect();				//客户异常断线，以此只设置相关断线标志
		else
		//如果是心跳超时清除出队列，服务器主动关闭该连接
		//由于心跳超时的时间相当长，在这个过程中有可能客户已经重新登录或者登录以后发出Logout指令，
		//但原来的连接还没有断开，直到超时时间到，这个过程pUserInfo的不一定具备有效性，因此需要验证
		if((3 == unFlag) && CheckDeleteUser(unPerHandle, pUserInfo))
			delete pUserInfo;	//从队列解除以后，才能安全释放
	}

	//数据包拆分（处理粘包）
	inline void DataPackProcesser(unsigned int unPerHandle, char* pBuf, unsigned int unLen, CUserInfo* pUserInfo)
	{
		PDATA_PACK_INFO pDataPacks;
		char* pDataBuf;
		unsigned int unPacksLen, unProcLen;
		
		//上次有残包？
		if(pUserInfo->GetBufSave())
		{
			//计算总长度
			unPacksLen = pUserInfo->GetLenSave() + unLen;
			//超过法定的长度的，那么数据肯定有问题，假如数据包总长度大于两个最长数据包之和，那么肯定有问题
			if(unPacksLen > 1024 * 1024 * 1)//这里默认1M
			{
				//关闭异常连接
				pUserInfo->Close();
				HeapFree(GetProcessHeap(), 0, pUserInfo->GetBufSave());
				pUserInfo->SetBufSave(NULL);
				return;
			}
			//合并包
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
			//头四个字节不是规定的数据头标志，关闭连接
			if(CMD_PACK_HEAD != pDataPacks->unCmd)
			{	
				//关闭异常连接
				pUserInfo->Close();
				//是否是合并用的缓冲，是则释放
				if((char*)pDataBuf != pBuf)
					HeapFree(GetProcessHeap(), 0, pDataBuf);
				return;
			}
			//包长度大于实际接收长度的，说明是残包，退出while之后保存
			if(pDataPacks->unLen > unPacksLen - unProcLen)
			{
				break;
			}
			if(GetCRC((char*)&pDataPacks->unCRC, pDataPacks->unLen - sizeof(DATA_PACK_INFO)) != pDataPacks->unCRC)
			{
				//关闭异常连接
				pUserInfo->Close();
				//是否是合并用的缓冲，是则释放
				if((char*)pDataBuf != pBuf)
					HeapFree(GetProcessHeap(), 0, pDataBuf);
				return;
			}
			//提取有效数据，调用处理函数
			OnReceive(unPerHandle, (char*)&pDataPacks->unCRC, pDataPacks->unLen - sizeof(DATA_PACK_INFO), pUserInfo);
			//向前推移pDataPacks->unLen字节
			unProcLen += pDataPacks->unLen;
			pDataPacks = PDATA_PACK_INFO((char*)pDataPacks + pDataPacks->unLen);
		}
		//有未处理完的数据
		if(unProcLen < unPacksLen)
		{
			//未处理的数据长度
			pUserInfo->SetLenSave(unPacksLen - unProcLen);
			//再次分配一个缓冲
			pUserInfo->SetBufSave((char*)HeapAlloc(GetProcessHeap(), 0, pUserInfo->GetLenSave()));
			//保存未处理的数据到缓冲里面
			CopyMemory(pUserInfo->GetBufSave(), (char*)pDataPacks, pUserInfo->GetLenSave());			
		}
		//是否是合并用的缓冲，是则释放
		if((char*)pDataBuf != pBuf)
			HeapFree(GetProcessHeap(), 0, pDataBuf);
	}
};