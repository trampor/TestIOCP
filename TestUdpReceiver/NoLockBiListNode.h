#pragma once

template <class EleType>
class CNoLockBiListNode
{
public:
	CNoLockBiListNode()
	{

	}
	~CNoLockBiListNode()
	{

	}
	CNoLockBiListNode(EleType e)
	{
		ele = e;
	}
	void SetEle(EleType e)
	{
		ele = e;
	}
	EleType GetEle()
	{
		return ele;
	}

public:
	volatile CNoLockBiListNode* pprev;
	volatile CNoLockBiListNode* pnext;
	EleType ele;
};
