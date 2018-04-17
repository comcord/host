#ifndef GM_THREAD_H
#define GM_THREAD_H

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

enum GMT_ThreadProcType
{
	Class_Mem_Thread_Fun = 0,
	Global_Thread_Fun,			
};

#ifdef WIN32

#endif

class GMThreadID
{
public:

	GMThreadID();

	GMThreadID(const GMThreadID& id);

	GMThreadID& operator=(const GMThreadID& id);

	static GMThreadID getCurrentThreadID();

	bool operator == (const GMThreadID& id);

	bool operator != (const GMThreadID& id);

	void setNull();

	bool isNull();
public:

#ifdef WIN32
	U32 m_threadId;
#else
	pthread_t m_threadId;
#endif
};
template<class ThreadOwner, GMT_ThreadProcType = Class_Mem_Thread_Fun>
class GMThread
{
public:
	typedef void (ThreadOwner::*T_ClassMemThreadProc)(void *);
	typedef void (*T_GlobalThreadProc)(void *);

	GMThread();
	GMThread(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param);
	GMThread(ThreadOwner* const owner,T_ClassMemThreadProc proc,void * param){new(this)GMThread(proc,owner,param);}
	GMThread(T_GlobalThreadProc proc, void * param);
	void init(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param);
	void init(ThreadOwner* const owner,T_ClassMemThreadProc proc,void * param){init(proc,owner,param);}
	
	void init(T_GlobalThreadProc proc, void * param);

	~GMThread();
	bool run();
	void kill();

	bool waitThreadExit(unsigned int timeout = INFINITE);

#ifdef WIN32
	HANDLE getHandle() { return m_handle; }
#endif

	GMThreadID getThreadID() { return m_threadId; }


private:

#ifdef WIN32
	static unsigned int __stdcall threadFun(void* lpParameter);
#else
	static void* threadFun( void *lpParameter);
#endif

protected:	

#ifdef WIN32
	HANDLE m_handle;

#endif

	GMThreadID m_threadId;

	ThreadOwner* m_pOwner;
	void * m_pParam;
	union 
	{
		T_ClassMemThreadProc m_classMemFunProc;
		T_GlobalThreadProc m_globalProc;
	};

	GMSysEvent* m_runEvent;
	GMSysEvent* m_syncEvent;
	volatile bool m_bCreateSuc;	
};

template<class ThreadOwner, GMT_ThreadProcType proctype>
GMThread<ThreadOwner,proctype>::GMThread(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param):
m_pOwner(powner), m_classMemFunProc(proc),m_pParam(param),m_bCreateSuc(false),m_syncEvent(NULL),m_runEvent(NULL)
{
	assert(proctype == Class_Mem_Thread_Fun);

#ifdef WIN32
	m_handle = NULL;
#endif

#ifndef WIN32
	void create_thread_freekey(void);

	extern pthread_once_t create_thread_freekey_init_done;
	pthread_once(&create_thread_freekey_init_done,create_thread_freekey);
#endif
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
GMThread<ThreadOwner,proctype>::GMThread(T_GlobalThreadProc proc, void * param):
m_pParam(param), m_globalProc(proc),m_bCreateSuc(false),m_syncEvent(NULL),m_runEvent(NULL)
{
	assert(proctype == Global_Thread_Fun);

#ifdef WIN32
	m_handle = NULL;
#else
	void create_thread_freekey(void);
	extern pthread_once_t create_thread_freekey_init_done;
	pthread_once(&create_thread_freekey_init_done,create_thread_freekey);
#endif
}


template<class ThreadOwner, GMT_ThreadProcType proctype>
GMThread<ThreadOwner,proctype>::GMThread():
m_pOwner(NULL),m_pParam(NULL), m_globalProc(NULL),m_bCreateSuc(false),m_syncEvent(NULL),m_runEvent(NULL)
{
#ifdef WIN32
	m_handle = NULL;
#else
	void create_thread_freekey(void);
	extern pthread_once_t create_thread_freekey_init_done;
	pthread_once(&create_thread_freekey_init_done,create_thread_freekey);
#endif
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMThread<ThreadOwner,proctype>::init(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param)
{
	assert(proctype == Class_Mem_Thread_Fun);

	m_classMemFunProc = proc;
	m_pOwner = powner;
	m_pParam = param;
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMThread<ThreadOwner,proctype>::init(T_GlobalThreadProc proc, void * param)
{
	assert(proctype == Global_Thread_Fun);

	m_globalProc = proc;
	m_pParam = param;
}

#ifdef WIN32
extern unsigned long  freeSelfThreadFlag;
#endif

template<class ThreadOwner, GMT_ThreadProcType proctype>
GMThread<ThreadOwner,proctype>::~GMThread()
{
	if(m_runEvent)delete m_runEvent;
	
	if(m_syncEvent)delete m_syncEvent;

	if (GMThreadID::getCurrentThreadID() == m_threadId)
	{
#ifdef WIN32
		if (m_handle)CloseHandle(m_handle);
		
		::TlsSetValue(freeSelfThreadFlag,(PVOID)1);
#else
		extern pthread_key_t freeSelfThreadFlagKey;
		pthread_setspecific(freeSelfThreadFlagKey,(void*)1);
#endif
	}
}



template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMThread<ThreadOwner,proctype>::kill()
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


template<class ThreadOwner, GMT_ThreadProcType proctype>
bool GMThread<ThreadOwner,proctype>::run()
{
	if(m_bCreateSuc)return true;

	assert(NULL == m_runEvent);
	assert(NULL == m_syncEvent);

	m_runEvent = new GMSysEvent(true);
	if (NULL == m_runEvent)return false;

	m_syncEvent = new GMSysEvent(true);
	if(NULL == m_syncEvent)
	{
		delete m_runEvent;
		m_runEvent = NULL;
		return false;
	}

#ifdef WIN32
	m_handle = (HANDLE)_beginthreadex(NULL, 0, threadFun,(void *)this, 0,(U32*)&m_threadId.m_threadId);
	m_bCreateSuc = (m_handle != NULL);
#else
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
	 
	bool bCreateSuc = m_bCreateSuc;

	if(m_bCreateSuc)
	{	
		int ret = m_syncEvent->wait();
		assert(1 == ret);
		delete(m_syncEvent);
		m_syncEvent = NULL;

		bool ret2 = m_runEvent->signal();
		assert(ret2);
	}

	return bCreateSuc;
}


template<class ThreadOwner, GMT_ThreadProcType proctype>
#ifdef WIN32
unsigned int __stdcall GMThread<ThreadOwner,proctype>::threadFun(void* lpParameter)
#else
void* GMThread<ThreadOwner,proctype>::threadFun( void *lpParameter)
#endif
{
	assert(lpParameter);
	GMThread<ThreadOwner,proctype> *pthis = static_cast<GMThread<ThreadOwner,proctype> *>(lpParameter);

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

	if(proctype == Class_Mem_Thread_Fun)
		((static_cast<ThreadOwner* const>(pthis->m_pOwner))->*(pthis->m_classMemFunProc))(pthis->m_pParam);
	else if(proctype == Global_Thread_Fun)
		(pthis->m_globalProc)(pthis->m_pParam);
	else
		assert(false);

	
#ifdef WIN32
	if ((DWORD)::TlsGetValue(freeSelfThreadFlag) != 1)
	{
		pthis->m_bCreateSuc = false;
		::CloseHandle(pthis->m_handle);
		pthis->m_handle = NULL;
		pthis->m_threadId.setNull();
	}

	return 0;

#else
	extern pthread_key_t freeSelfThreadFlagKey;
	// // Linux
#ifdef X64	
	long long flag = (long long)pthread_getspecific(freeSelfThreadFlagKey);
#else
	long flag = (long)pthread_getspecific(freeSelfThreadFlagKey);
#endif //X64
	if (flag != 1)
	{
		pthis->m_bCreateSuc = false;
		pthis->m_threadId.setNull();
	}

	return NULL;
#endif
}


template<class ThreadOwner, GMT_ThreadProcType proctype>
bool GMThread<ThreadOwner,proctype>::waitThreadExit(unsigned int t)
{
	if (!m_bCreateSuc)return false;

	if (GMThreadID::getCurrentThreadID() != m_threadId)
	{
		while (m_bCreateSuc)
		{
			Sleep(100);
			t > 100 ? t -= 100 : t = 0;

			if(t == 0) 
				return false;
		}
	}

	return true;
}



template<class ThreadOwner, GMT_ThreadProcType = Class_Mem_Thread_Fun>
class GMDaemonThread
{
	typedef void (ThreadOwner::*T_ClassMemThreadProc)(void *);
	typedef void (*T_GlobalThreadProc)(void *);

public:
	GMDaemonThread(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param);
	GMDaemonThread(ThreadOwner* const owner,T_ClassMemThreadProc proc, void * param){new(this)GMDaemonThread(proc,owner,param);}
	GMDaemonThread(T_GlobalThreadProc proc, void * param);
	GMDaemonThread();

	void init(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param);
	void init(ThreadOwner* const owner,T_ClassMemThreadProc proc, void * param){init(proc,owner,param);}
	void init(T_GlobalThreadProc proc, void * param);

	bool start();
	void stop();
	bool waitThreadExit(unsigned int timeout = INFINITE){return m_thread.waitThreadExit(timeout);}
	GMThreadID getThreadID() { return m_thread.getThreadID(); }
#ifdef WIN32
	HANDLE getHandle() { return m_thread.getHandle(); }
#endif
private:
	void threadProc(void* param);

private:
	bool m_bStart;
	GMThread<GMDaemonThread,Class_Mem_Thread_Fun> m_thread;

	ThreadOwner* m_pOwner;
	void * m_pParam;
	union 
	{
		T_ClassMemThreadProc m_classMemFunProc;
		T_GlobalThreadProc m_globalProc;
	};

};

#pragma warning(disable:4355)

template<class ThreadOwner, GMT_ThreadProcType proctype>
GMDaemonThread<ThreadOwner,proctype>::GMDaemonThread(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param)
:m_pOwner(powner), m_classMemFunProc(proc),m_pParam(param),
m_thread(&GMDaemonThread::threadProc,this,NULL),m_bStart(false)
{
	assert(proctype == Class_Mem_Thread_Fun);
}


template<class ThreadOwner, GMT_ThreadProcType proctype>
GMDaemonThread<ThreadOwner,proctype>::GMDaemonThread(T_GlobalThreadProc proc, void * param)
:m_pParam(param), m_globalProc(proc),m_pOwner(NULL),
m_thread(&GMDaemonThread::threadProc,this,NULL),m_bStart(false)
{
	assert(proctype == Global_Thread_Fun);
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
GMDaemonThread<ThreadOwner,proctype>::GMDaemonThread():
m_thread(&GMDaemonThread::threadProc,this,NULL),
m_classMemFunProc(NULL),m_pOwner(NULL),m_pParam(NULL),m_bStart(false)
{

}
#pragma warning(default:4355)

template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMDaemonThread<ThreadOwner,proctype>::init(T_ClassMemThreadProc proc, ThreadOwner* const powner,void * param)
{
	assert(proctype == Class_Mem_Thread_Fun);
	m_classMemFunProc = proc;
	m_pOwner = powner;
	m_pParam = param;
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMDaemonThread<ThreadOwner,proctype>::init(T_GlobalThreadProc proc, void * param)
{
	assert(proctype == Global_Thread_Fun);
	m_globalProc = proc;
	m_pParam = param;
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMDaemonThread<ThreadOwner,proctype>::threadProc(void* param)
{
	while (m_bStart)
	{
		if(proctype == Class_Mem_Thread_Fun)
			((static_cast<ThreadOwner* const>(m_pOwner))->*(m_classMemFunProc))(m_pParam);
		else if(proctype == Global_Thread_Fun)
			(m_globalProc)(m_pParam);
		else
			assert(false);
	}
}

template<class ThreadOwner, GMT_ThreadProcType proctype>
bool GMDaemonThread<ThreadOwner,proctype>::start()
{
	m_bStart = true;
	return m_thread.run();
}


template<class ThreadOwner, GMT_ThreadProcType proctype>
void GMDaemonThread<ThreadOwner,proctype>::stop()
{
	m_bStart = false;
}

#endif
