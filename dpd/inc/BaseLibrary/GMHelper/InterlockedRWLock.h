#pragma once
#ifdef WIN32
#include <malloc.h>

#elif defined(__IOS__)
#include <malloc/malloc.h>

#endif
#include "GMlockimp.h"

//使用InterlockedCompareExchange实现的读写锁
class InterlockedRWLock
{
	enum State
	{
		IDLE = 0,
		READ,
		WRITE,
	};

public:
	InterlockedRWLock():m_readlockRef(0),m_state(IDLE)
	{
	}

	void readLock();

	void unReadLock();

	void writeLock();

	void unWriteLock();

private:
	GMCSLock m_lock;
	volatile long m_readlockRef;
	volatile long m_state;
};
