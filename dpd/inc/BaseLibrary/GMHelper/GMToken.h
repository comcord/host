#pragma once

#include <assert.h>
#include "GMLock.h"
#include "GMautolock.h"

/**
 *	令牌类， 无内核对象的同步
 */

#ifdef WIN32
class    GMToken
{
public:
	GMToken(bool isSyn = true)
		:m_isSyn(isSyn),m_token(1)
	{
	}
	~GMToken(void)
	{
		assert(m_token == 1);
	}

	/**
	* 申请令牌
	*/
	bool Borrow()
	{
		do{
			if(1 == ::InterlockedCompareExchange((long *)&m_token,0,1))
				return true;
			else
				if(m_isSyn)	
					Sleep(0);
				else 
					return false;
		} while(m_isSyn);
		return false;
	}

	/**
	* 归还令牌
	*/
	void Repay()
	{
		if(1 == ::InterlockedCompareExchange((long *)&m_token,1, 0))
			assert(false);
	}
	
private:
	volatile int m_token;
	const bool m_isSyn;
};

#else // linux
#include <pthread.h>

class    GMToken
{
public:
	GMToken()
	{
		//pthread_mutex_init(&_mutex, 0);
	}
	~GMToken(void)
	{
		//pthread_mutex_destroy(&_mutex);
	}

	/**
	* 申请令牌
	*/
	bool Borrow()
	{
		m_lock.lock();
		//pthread_mutex_lock(&_mutex);
		return true;
	}

	/**
	* 归还令牌
	*/
	void Repay()
	{
		m_lock.unlock();
		//pthread_mutex_unlock(&_mutex);
	}
private:
	//pthread_mutex_t _mutex;
	GMLock m_lock;
};

#endif
