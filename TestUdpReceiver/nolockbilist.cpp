#include "stdafx.h"
#include "NoLockBiList.h"

template <class EleType>
CNoLockBiList<EleType>::CNoLockBiList()
	:phead(NULL), ptail(NULL), pnullhead(NULL), pnulltail(NULL), m_nAllocatedEleNum(0), m_nNodeNum(0)
{
}

template <class EleType>
CNoLockBiList<EleType>::~CNoLockBiList()
{
	while (phead != NULL)
	{
		ptail = phead->pnext;
		delete phead;
		phead = ptail;
	}

	while (pnullhead != NULL)
	{
		pnulltail = pnullhead->pnext;
		delete pnullhead;
		pnullhead = pnulltail;
	}
}

template <class EleType>
int CNoLockBiList<EleType>::InitObject(int initsize)
{
	CNoLockBiListNode<EleType>* ptempnode;
	for (int i = 0; i < initsize; i++)
	{
		ptempnode = new CNoLockBiListNode<EleType>;
		PushNullTail(ptempnode);
	}

	return 0;
}

template <class EleType>
EleType* CNoLockBiList<EleType>::PopHead()
{
	EleType* pele;
	CNoLockBiListNode<EleType>* pheadnode, *ptailnode;
	for (;;)
	{
		ptailnode = (CNoLockBiListNode<EleType>*)ptail;
		pheadnode = (CNoLockBiListNode<EleType>*)phead;
		if (ptailnode == NULL)
			return NULL;
		else if (pheadnode == ptailnode)
		{
			if (InterlockedCompareExchangePointer((PVOID*)&ptail, NULL, (PVOID)ptailnode) == ptailnode)
			{
				phead = NULL;
				pele = (EleType*)ptailnode->ele;
				PushNullTail((CNoLockBiListNode<EleType>*)ptailnode);
				return pele;
			}
		}
		else
		{
			ptailnode = (CNoLockBiListNode<EleType>*)pheadnode->pnext;
			if ((ptailnode != NULL) && (InterlockedCompareExchangePointer((PVOID*)&phead, (PVOID)ptailnode, (PVOID)pheadnode) == pheadnode))
			{
				ptailnode->pprev = NULL;
				pele = (EleType*)pheadnode->ele;
				PushNullTail((CNoLockBiListNode<EleType>*)pheadnode);
				return pele;
			}
		}
	}
}

template <class EleType>
bool CNoLockBiList<EleType>::PushTail(EleType e)
{
	CNoLockBiListNode<EleType>* pnode = PopNullHead();
	if (pnode == NULL)
	{
		pnode = new CNoLockBiListNode<EleType>;
		InterlockedIncrement(&m_nNodeNum);
	}
	pnode->SetEle(e);
	pnode->pprev = NULL;
	pnode->pnext = NULL;

	CNoLockBiListNode<EleType>* ptailnode;
	for (;;)
	{
		ptailnode = (CNoLockBiListNode<EleType>*)ptail;
		if (ptailnode == NULL)
		{
			if (InterlockedCompareExchangePointer((PVOID*)&phead, pnode, NULL) == NULL)
			{
				ptail = pnode;
				return true;
			}
		}
		else
		{
			pnode->pprev = ptailnode;
			if (InterlockedCompareExchangePointer((PVOID*)&ptail, pnode, (PVOID)ptailnode) == ptailnode)
			{
				ptailnode->pnext = pnode;
				return true;
			}
		}
	}
}


template <class EleType>
CNoLockBiListNode<EleType>* CNoLockBiList<EleType>::PopNullHead()
{
	volatile CNoLockBiListNode<EleType>* pheadnode, *ptailnode;
	for (;;)
	{
		ptailnode = pnulltail;
		pheadnode = pnullhead;
		if (ptailnode == NULL)
			return NULL;
		else if (pheadnode == ptailnode)
		{
			if (InterlockedCompareExchangePointer((PVOID*)&pnulltail, NULL, (PVOID)ptailnode) == ptailnode)
			{
				pnullhead = NULL;
				return (CNoLockBiListNode<EleType>*)ptailnode;
			}
		}
		else
		{
			ptailnode = pheadnode->pnext;
			if ((ptailnode != NULL) && (InterlockedCompareExchangePointer((PVOID*)&pnullhead, (PVOID)ptailnode, (PVOID)pheadnode) == pheadnode))
			{
				ptailnode->pprev = NULL;
				return (CNoLockBiListNode<EleType>*)pheadnode;
			}
		}
	}
}

template <class EleType>
bool CNoLockBiList<EleType>::PushNullTail(CNoLockBiListNode<EleType>* pnode)
{
	pnode->pprev = NULL;
	pnode->pnext = NULL;
	volatile CNoLockBiListNode<EleType>* ptailnode;
	for (;;)
	{
		ptailnode = pnulltail;
		if (ptailnode == NULL)
		{
			if (InterlockedCompareExchangePointer((PVOID*)&pnullhead, pnode, NULL) == NULL)
			{
				pnulltail = pnode;
				return true;
			}
		}
		else
		{
			pnode->pprev = ptailnode;
			if (InterlockedCompareExchangePointer((PVOID*)&pnulltail, pnode, (PVOID)ptailnode) == ptailnode)
			{
				ptailnode->pnext = pnode;
				return true;
			}
		}
	}
}