#pragma once
#include "NoLockBiListNode.h"

template <class EleType>
class CNoLockBiList
{
public:
	CNoLockBiList();
	~CNoLockBiList();

	int InitObject(int initsize);

	EleType* PopHead();
	bool PushTail(EleType e);

public:
	volatile long m_nAllocatedEleNum;

private:

	CNoLockBiListNode<EleType>* PopNullHead();
	bool PushNullTail(CNoLockBiListNode<EleType>* pnode);

private:
	volatile CNoLockBiListNode<EleType>* phead;
	volatile CNoLockBiListNode<EleType>* ptail;

	volatile CNoLockBiListNode<EleType>* pnullhead;
	volatile CNoLockBiListNode<EleType>* pnulltail;

	volatile long m_nEleNum;
	volatile long m_nNodeNum;
};

#include "NoLockBiList.cpp"

