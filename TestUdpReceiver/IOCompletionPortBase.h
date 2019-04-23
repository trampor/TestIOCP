#pragma once


#define CK_FILE_READ  1
#define CK_FILE_WRITE 2
#define CK_SOCKET_READ  11
#define CK_SOCKET_WRITE 12

class CIOCompletionPortBase
{
public:
	CIOCompletionPortBase();
	~CIOCompletionPortBase();

	virtual BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey) = 0;
};

