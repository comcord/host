#ifndef GMAutoLock_TEMPLATE_H_2008_GM
#define GMAutoLock_TEMPLATE_H_2008_GM
#include <assert.h>



/*
 *	功能：
		自动锁，在构造的时候加锁，析构的时候解锁
	参数：
		LockType  锁类型支持类型有(GMLock\GMRWLock\LockPoolAgnet\RWLockPoolAgnet)
	
 */
enum GMT_RWLockOperator
{
	GMWriteOperator = 0,//写操作
	GMReadOperator = 1,//读操作
};

template<class LockType>
class GMAutoLock
{

public:
	GMAutoLock(LockType * const plock)
		:m_plock(plock)
	{
		if(m_plock)
		{
			m_plock->lock();
			m_unlockfun = &LockType::unlock;
		}
		else
			assert(false);
	}

	GMAutoLock(LockType * const plock, GMT_RWLockOperator op)
		:m_plock(plock)
	{
		if(m_plock)
		{
			if(op == GMWriteOperator)
			{
				m_plock->writeLock();
				m_unlockfun = 	&LockType::unWriteLock;
			}
			else if(op == GMReadOperator)
			{
				m_plock->readLock();
				m_unlockfun = &LockType::unReadLock;
			}
			else
			{
				assert(false);
			}
		}
		else
		{
			assert(false);
		}
	}
		
	~GMAutoLock(void)
	{
		if(m_plock && m_unlockfun)
			(m_plock->*m_unlockfun)();
		else
			assert(false);

	}
private:
	 typedef void (LockType::*T_UnlockFun)(void);
	 LockType *const m_plock;
	 T_UnlockFun m_unlockfun;
};

#endif
