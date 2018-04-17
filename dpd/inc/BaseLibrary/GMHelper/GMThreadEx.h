#pragma once

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include <assert.h>
#include "GMEvent.h"
#include "BaseDataType.h"
#include "SysDiffDefine.h"
#include <stdlib.h>

enum GM_THREAD_FUN_TYPE
{
	GMTFT_MEMBER_FUN = 0, 
	GMTFT_GLOBAL_FUN,
};

class GMThreadExID
{
public:

	GMThreadExID();

	GMThreadExID(const GMThreadExID& id);

	GMThreadExID& operator=(const GMThreadExID& id);

	static GMThreadExID getCurrentThreadID();

	bool operator == (const GMThreadExID& id);

	bool operator != (const GMThreadExID& id);

	void setNull();

	bool isNull();
public:

#ifdef WIN32
	U32 m_threadId;
#else
	pthread_t m_threadId;
#endif
};

template<class ThreadOwner, GM_THREAD_FUN_TYPE = GMTFT_MEMBER_FUN>
class GMThreadEx
{
public:
	typedef void (ThreadOwner::*MEMBER_FUN_THREAD_PRC)(void *);
	typedef void (*GLOBAL_FUN_THREAD_PRC)(void *);

	GMThreadEx();

	~GMThreadEx();

	bool create(unsigned int stack_size=0);
	
	void run(MEMBER_FUN_THREAD_PRC proc, ThreadOwner* const powner,void * param);
	void run(GLOBAL_FUN_THREAD_PRC proc, void * param);

	void kill();

	bool waitThreadExit(unsigned int timeout = INFINITE);
	// linux
#ifdef WIN32
	HANDLE getHandle() { return m_handle; }
#endif

	GMThreadExID getThreadID() { return m_threadId; }

private:

#ifdef WIN32
	static unsigned int __stdcall threadFun(void* lpParameter);
#else
	static void* threadFun( void *lpParameter);
#endif

protected:	
	// linux
#ifdef WIN32
	HANDLE m_handle;
#endif

	GMThreadExID m_threadId;

	ThreadOwner* m_pOwner;
	void * m_pParam;
	union 
	{
		MEMBER_FUN_THREAD_PRC m_classMemFunProc;
		GLOBAL_FUN_THREAD_PRC m_globalProc;
	};

	GMSysEvent* m_runEvent;
	GMSysEvent* m_syncEvent;
	volatile bool m_bCreateSuc;	
};

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
GMThreadEx<ThreadOwner,proctype>::GMThreadEx():
m_pOwner(NULL),m_pParam(NULL), m_globalProc(NULL),m_bCreateSuc(false),m_syncEvent(NULL),m_runEvent(NULL)
{	// linux
#ifdef WIN32
	m_handle = NULL;
#else
	void create_threadex_slef_free_key(void);
	extern pthread_once_t g_create_gmthreadex_slef_free_key_init_done;
	pthread_once(&g_create_gmthreadex_slef_free_key_init_done,create_threadex_slef_free_key);
#endif
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
GMThreadEx<ThreadOwner,proctype>::~GMThreadEx()
{
	if(m_runEvent)delete m_runEvent;

	if(m_syncEvent)delete m_syncEvent;

	if (GMThreadExID::getCurrentThreadID() == m_threadId)
	{
#ifdef WIN32
		if (NULL != m_handle)CloseHandle(m_handle);
		//extern __declspec( thread )  int g_gmthreadex_slef_free_flag;
		//g_gmthreadex_slef_free_flag = 1;
		extern unsigned long  g_gmthreadex_slef_free_flag;
		::TlsSetValue(g_gmthreadex_slef_free_flag,(PVOID)1);
#else
		extern pthread_key_t g_gmthreadex_slef_free_key;
		pthread_setspecific(g_gmthreadex_slef_free_key,(void*)1);
#endif
	}
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
bool GMThreadEx<ThreadOwner,proctype>::create(unsigned int stack_size)
{
	if(m_bCreateSuc)return true;

	if(NULL != m_runEvent)
	{
		m_runEvent->reset();
	}
	else
	{
		m_runEvent = new GMSysEvent(true);
		if (NULL == m_runEvent)return false;
	}

	if(NULL != m_syncEvent)
	{
		m_syncEvent->reset();
	}
	else
	{
		m_syncEvent = new GMSysEvent(true);
		if(NULL == m_syncEvent)
		{
			delete m_runEvent;
			m_runEvent = NULL;
			return false;
		}
	}

#ifdef WIN32
	m_handle = (HANDLE)_beginthreadex(NULL, stack_size, threadFun,(void *)this, 0,(U32*)&m_threadId.m_threadId);
	DWORD le = GetLastError();
	m_bCreateSuc = (m_handle != NULL);
#else//TODO stack size
	m_bCreateSuc = (0 == pthread_create(&m_threadId.m_threadId, NULL, threadFun, this));
#ifndef _ANDROID_NDK_
	if(m_bCreateSuc)
	{
		int cancelstate,oldcancelstate;
		cancelstate = PTHREAD_CANCEL_ENABLE;
		pthread_setcancelstate(cancelstate,&oldcancelstate);
		int canceltype,oldcanceltype;
		canceltype = PTHREAD_CANCEL_ASYNCHRONOUS;
		pthread_setcanceltype(canceltype,&oldcanceltype);
	}
#endif //_ANDROID_NDK_
#endif
	
	return m_bCreateSuc;
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
void GMThreadEx<ThreadOwner,proctype>::run(MEMBER_FUN_THREAD_PRC proc, ThreadOwner* const powner,void * param)
{
	assert(proctype == GMTFT_MEMBER_FUN);

	m_classMemFunProc = proc;
	m_pOwner = powner;
	m_pParam = param;

	if(m_bCreateSuc)
	{	
		int ret = m_syncEvent->wait();
		assert(1 == ret);
		delete(m_syncEvent);
		m_syncEvent = NULL;

		bool ret2 = m_runEvent->signal();
		assert(ret2);
	}
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
void GMThreadEx<ThreadOwner,proctype>::run(GLOBAL_FUN_THREAD_PRC proc, void * param)
{
	assert(proctype == GMTFT_GLOBAL_FUN);

	m_globalProc = proc;
	m_pParam = param;

	if(m_bCreateSuc)
	{	
		int ret = m_syncEvent->wait();
		assert(1 == ret);
		delete(m_syncEvent);
		m_syncEvent = NULL;

		bool ret2 = m_runEvent->signal();
		assert(ret2);
	}
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
#ifdef WIN32
unsigned int __stdcall GMThreadEx<ThreadOwner,proctype>::threadFun(void* lpParameter)
#else
void* GMThreadEx<ThreadOwner,proctype>::threadFun( void *lpParameter)
#endif
{
	assert(lpParameter);
	GMThreadEx<ThreadOwner,proctype> *pthis = static_cast<GMThreadEx<ThreadOwner,proctype> *>(lpParameter);

#ifdef WIN32

	MSG	msg	;
	PeekMessage(&msg,NULL,0,0xffffffff,PM_NOREMOVE);
#else
	pthread_detach(pthis->m_threadId.m_threadId);
#endif

	srand(GMGetTickCount());

	pthis->m_syncEvent->signal();	

	pthis->m_runEvent->wait();  
	delete pthis->m_runEvent;
	pthis->m_runEvent = NULL;

	if(proctype == GMTFT_MEMBER_FUN)
	{
		if (NULL != pthis->m_pOwner && NULL != pthis->m_classMemFunProc)
			((static_cast<ThreadOwner* const>(pthis->m_pOwner))->*(pthis->m_classMemFunProc))(pthis->m_pParam);
	}
	else
	{
		if (NULL != pthis->m_globalProc)
			(pthis->m_globalProc)(pthis->m_pParam);
	}

	//printf("gmthreadex %d, handle %d.\r\n",pthis->m_threadId.m_threadId,pthis->m_handle);

#ifdef WIN32
	//extern __declspec( thread )  int g_gmthreadex_slef_free_flag;
	extern unsigned long  g_gmthreadex_slef_free_flag;
	if ((DWORD)::TlsGetValue(g_gmthreadex_slef_free_flag) != 1)
	//if (g_gmthreadex_slef_free_flag != 1)
	{
		pthis->m_bCreateSuc = false;
		::CloseHandle(pthis->m_handle);
		pthis->m_handle = NULL;
		pthis->m_threadId.setNull();
	}

	return 0;

#else
	extern pthread_key_t g_gmthreadex_slef_free_key;
	long long  flag = (long long)pthread_getspecific(g_gmthreadex_slef_free_key);
	if (flag != 1)
	{
		pthis->m_bCreateSuc = false;
		pthis->m_threadId.setNull();
	}

	return NULL;
#endif
}


template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
bool GMThreadEx<ThreadOwner,proctype>::waitThreadExit(unsigned int t)
{
	if (!m_bCreateSuc)return true;

	if (GMThreadExID::getCurrentThreadID() != m_threadId)
	{
#ifdef WIN32
		if(m_bCreateSuc && NULL != m_handle) 
			return WAIT_OBJECT_0 == WaitForSingleObject(m_handle,t);
		else
			return true;
#else
		while (m_bCreateSuc) 
		{
			Sleep(100);
			t > 100 ? t -= 100 : t = 0;

			if(t == 0) 
				return false;
		}
#endif
	}
	
	return true;
}

template<class ThreadOwner, GM_THREAD_FUN_TYPE proctype>
void GMThreadEx<ThreadOwner,proctype>::kill()
{
#ifdef WIN32
	if(m_handle)TerminateThread(m_handle, 0);
	m_handle = NULL;
#else
#ifndef _ANDROID_NDK_
	pthread_cancel(m_threadId.m_threadId);
#endif //_ANDROID_NDK_
#endif

	m_threadId.setNull();
}
