#include "stdafx.h"
#include "IOCompletionPortBase.h"

HANDLE CIOCompletionPortBase::m_hCompletionPort;

CIOCompletionPortBase::CIOCompletionPortBase()
{
}


CIOCompletionPortBase::~CIOCompletionPortBase()
{
}

BOOL CIOCompletionPortBase::AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey)
{
	return (CreateIoCompletionPort(hDevice, m_hCompletionPort, CompKey, 0) != NULL);
}
