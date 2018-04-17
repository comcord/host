/****************************************************************************
*                                                                           *
* GMLockPool.h --	constant and macros definitions	                        *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/

#define INFINITE            0xFFFFFFFF  // Infinite timeout
#ifndef GF_LOCKPOOL_H_GOUMANG_2008
#define GF_LOCKPOOL_H_GOUMANG_2008

#include "GMBlist.h"
#include "GMSmartPtr.h"
#include "GMLock.h"
#include "GMRWLock.h"
#include "GMMemAlloc.h"

template <class LockType>
class GMLockPoolIMP
{
	friend class GMLockPoolAgent;
	friend class GMRWLockPoolAgent;
public:
	static GMSmartPtr<GMLockPoolIMP<LockType> > Create();

	~GMLockPoolIMP()
	{
		destroy();
	}

	void init(unsigned int maxLockNum = INFINITE);
	void destroy();
	
private:
	GMLockPoolIMP():m_maxLockNum(INFINITE), m_totalLockNum(0){}

	unsigned int m_maxLockNum;
	unsigned int m_totalLockNum;

	typedef GMBlist<LockType*, GMListMemAllocMethod> LockList;
	//typedef std::list<LockType *> LockList;
	LockList m_lockList;
	GMLock m_accessLLLock;
};

typedef GMLockPoolIMP<GMLock> GMLockPool;
typedef GMSmartPtr<GMLockPool> GMLockPool_SmartPtr;


typedef GMLockPoolIMP<GMRWLock> GMRWLockPool;
typedef GMSmartPtr<GMRWLockPool> GMRWLockPool_SmartPtr;


template <class LockType>
GMSmartPtr<GMLockPoolIMP<LockType> > GMLockPoolIMP<LockType>::Create()
{
	return GMSmartPtr<GMLockPoolIMP<LockType> >(new(std::nothrow) GMLockPoolIMP<LockType> );
}

template <class LockType>
void GMLockPoolIMP<LockType>::init(unsigned int maxLockNum)
{
	m_maxLockNum = maxLockNum;
}

template <class LockType>
void GMLockPoolIMP<LockType>::destroy()
{
	GMAutoLock<GMLock> al(&m_accessLLLock);
	// typename告诉编译器LockList::iterator是类型表达式
	typename LockList::iterator it = m_lockList.begin();
	for ( ; it != m_lockList.end(); it++)
	{
		delete (*it);
	}
}

#endif
