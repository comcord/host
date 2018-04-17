#ifndef GMRecyclableThreadPool_h__
#define GMRecyclableThreadPool_h__

#include "GMThreadEx.h"
#include "GMLock.h"
#include "GMautolock.h"
#include "FQueue.h"
#include "GMEvent.h"
#include "GMRWLock.h"
#include <cstdio>
class GMRecyclableThreadPool;

class TestDelClass
{
	int i;
};

template<class MethodOwner>
class GMThreadInRecyclableThreadPool
{
	typedef void (MethodOwner::*MethodDeclare)(void* );
	typedef void (MethodOwner::*MethodDoneNoticeDeclare)(void);

public:
	GMThreadInRecyclableThreadPool():m_excuteMethodEvent(true,false),m_bStart(false),m_owner(NULL),m_method_param(NULL),m_methodDoneNotice(NULL),m_bSelfKill(false){}

	int start(unsigned int thread_stack_size)
	{
		if (m_bStart)return 1;

		if(!m_thread.create(thread_stack_size))
			return -1;

		m_bStart = true;

		m_thread.run(&GMThreadInRecyclableThreadPool::workProc,this,NULL);

		return 0;
	}

	void stop()
	{
		if (!m_bStart)return;

		m_bStart = false;

		m_excuteMethodEvent.signal();
		m_thread.waitThreadExit();
	}

	bool excute(MethodOwner* owner,MethodDeclare method,void* method_param,MethodDoneNoticeDeclare methodDoneNotice)
	{
		if (!m_bStart)
			return false;
 
		m_owner = owner;
		m_method = method;
		m_method_param = method_param;
		m_methodDoneNotice = methodDoneNotice;
		m_bSelfKill = false;

		m_excuteMethodEvent.signal();

		return true;
	}

	GMThreadExID getThreadID()
	{
		return m_thread.getThreadID();
	}
#ifdef WIN32
	HANDLE getHandle()
	{
		return m_thread.getHandle();
	}
#endif
	void selfKillNotice()
	{
		m_bSelfKill = true;
	}
private:
	void workProc(void *param);

private:
	bool m_bStart;
	GMSysEvent m_excuteMethodEvent;
	GMThreadEx<GMThreadInRecyclableThreadPool> m_thread;
	
	MethodOwner* m_owner;
	MethodDeclare m_method;
	void* m_method_param;
	
	MethodDoneNoticeDeclare m_methodDoneNotice;

	bool m_bSelfKill;
};


class GMRecyclableThreadPool
{
	friend class GMThreadInRecyclableThreadPool<GMRecyclableThreadPool>;
public:

	static bool init(unsigned int base_thread_num,unsigned int max_idle_thread_num,unsigned int thread_stack_size=0);

	static void uninit();

protected:
	GMThreadInRecyclableThreadPool<GMRecyclableThreadPool>* excute(void* param);

	virtual void method(void*) = 0;	

	virtual void methodDoneNotice() = 0;

private:
	static bool returnPool(GMThreadInRecyclableThreadPool<GMRecyclableThreadPool>*);
	static void recycleIdleThreadProc(void*);
private:
	static GMLock m_threadPoolLock;
	static FQueue<GMThreadInRecyclableThreadPool<GMRecyclableThreadPool>* > m_threadPool;
	static unsigned int m_thread_stack_size;
	static unsigned int m_init_thread_num;
#ifdef WIN32
	static long volatile m_thread_at_large_num;
#else 
#ifdef _ANDROID_NDK_
	static volatile int m_thread_at_large_num;
#else	//Linux
	static U32 volatile m_thread_at_large_num;

#endif //_ANDROID_NDK_
#endif //WIN32	

	static bool m_bRuning;
	static GMRWLock m_bRuningLock;
	
	static GMThreadEx<GMRecyclableThreadPool,GMTFT_GLOBAL_FUN> m_recycleIdleThreadThread;
};

class GMThreadFromRecyclableThreadPool:private GMRecyclableThreadPool
{
public:

	GMThreadFromRecyclableThreadPool( bool isWaitForRun = false):m_thread(NULL),m_threadExitEvent(false,true){}

	virtual ~GMThreadFromRecyclableThreadPool();

	using GMRecyclableThreadPool::init;

	using GMRecyclableThreadPool::uninit;

	bool run();

#ifdef WIN32
	HANDLE getHandle();
#endif

	bool waitThreadExit(unsigned long time = INFINITE);
#ifdef WIN32
	DWORD getThreadID();
#else
	pthread_t getThreadID();
#endif

protected:	
	
	virtual unsigned long threadProc(void) = 0;	

private:
	virtual void method(void*);

	virtual void methodDoneNotice();

private:
	GMThreadInRecyclableThreadPool<GMRecyclableThreadPool>* m_thread;
	GMSysEvent m_threadExitEvent;
	GMLock m_threadLock;
};

#endif // GMRecyclableThreadPool_h__
