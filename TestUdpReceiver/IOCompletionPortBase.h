#pragma once

#define STATUSSTOP 0
#define STATUSRUN 1
#define STATUSERROR 2

#define CK_FILE_READ  1
#define CK_FILE_WRITE 2
#define CK_SOCKET_READ  11
#define CK_SOCKET_WRITE 12
#define CK_SOCKET_LISTEN 13

#define LISTENBUFSIZE 200
#define RECVBUFSIZE 2048

class CIOCompletionPortBase
{
public:
	CIOCompletionPortBase();
	~CIOCompletionPortBase();

	static BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey);

protected:
	static HANDLE m_hCompletionPort;
};

