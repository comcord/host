#ifndef CLASSES_FOR_COMMON_H
#define CLASSES_FOR_COMMON_H

#include "commonPub.h"
#include "GMRWLockImp.h"
#ifdef WIN32
#else
#define WSAGetLastError() errno
#endif
class CTcpClient:public GMTcpClient
{
public:
	/*
	 *	接收数据 错误抛出 CSocketException \ CTimeoutException
	 *  返回接收到的字节数
	 */
	int recvData(unsigned char * szData, int len);
};

class CPlatformHelper 
{
public:
	static bool  getCurrentDirectory( char path[MAX_PATH]);
};

class CAutoRWLock
{
public:
	CAutoRWLock(CRWLock* pLock, bool isReadLock);
	~CAutoRWLock(void);

private:
	CRWLock* m_pLock;
	bool m_is_read_lock;
};

class CThread
{
public:
	CThread( bool isWaitForRun = false){};
	
	virtual ~CThread(){};

	//不能多次调用
	bool run(){
		m_thread.init(this,&CThread::thread_proc,NULL);
		return m_thread.run();
	}
	
	void shutdown(){
		m_thread.kill();
	}

#ifdef WIN32
	HANDLE getHandle(){
		return m_thread.getHandle();
	};
#endif
	
	/*
	*	等待线程退出. <run返回后才可执行等待>
		true 代表等待退出成功
		false 代码等待超时
	 */
	bool waitThreadExit(unsigned int time = INFINITE){
		return m_thread.waitThreadExit(time);
	}

#ifdef WIN32
	DWORD getThreadID(){return m_thread.getThreadID().m_threadId;}
#else
	pthread_t getThreadID(){return m_thread.getThreadID().m_threadId;}
#endif

protected:	
	/*
	 *	线程函数，子类OVERWRITE该函数
	 */
	virtual unsigned long threadProc(void) = 0;	

private:
	void thread_proc(void*){threadProc();}

private:
	GMThread<CThread,Class_Mem_Thread_Fun> m_thread;
};

class CDaemonThread : public CThread
{
public:
	CDaemonThread():CThread(true), m_active(false){}

	void start();

	void stop();

protected:

	virtual unsigned long threadProc(void);

	/*		
	*	线程函数，子类OVERWRITE该函数
	*/
	virtual unsigned long daemonThreadProc(void) = 0;

	volatile bool m_active;

};

/*
*	 CThread的一个外包类
要创建一个线程，只需要实例化一个CThreadWrapper对象,

线程函数写在T的实例化对象中
.*/
template<typename T>
class  CThreadWrapper:public CThread
{
	typedef unsigned long (T::*ThreadProcPtr)(void * pParameter);
public:
	CThreadWrapper(  
		T* objectDoingThreadProc,         //执行线程函数的对象
		ThreadProcPtr threadProc,		  //对象的线程函数指针
		void* pParameter,
		bool  isWaitForRun = false
		)
		:CThread(isWaitForRun),m_objectDoingThreadProc(objectDoingThreadProc),m_threadProc(threadProc),m_pParameter(pParameter){}

	//构造函数没有传递线程函数，run之前调用init
	CThreadWrapper()
		:CThread(true),m_objectDoingThreadProc(NULL),m_threadProc(NULL),m_pParameter(NULL){}

	void init(T* objectDoingThreadProc,         //执行线程函数的对象
		ThreadProcPtr threadProc,		  //对象的线程函数指针
		void* pParameter)
	{
		m_objectDoingThreadProc = objectDoingThreadProc;
		m_threadProc = threadProc;
		m_pParameter = pParameter; 
	}

	~CThreadWrapper()
	{
		waitThreadExit(INFINITE);
	}

private:
	unsigned long threadProc()
	{
		return (m_objectDoingThreadProc->*m_threadProc)(m_pParameter);
	}
private:
	T*  m_objectDoingThreadProc;
	ThreadProcPtr m_threadProc;
	void* m_pParameter;
};

/*
*	 CThread的一个外包类
要创建一个线程，只需要实例化一个CThreadWrapper对象,

线程函数为全局函索或者静态函索
.*/
class  CThreadWrapper_unObjectThreadPorc:public CThread
{
	typedef unsigned long (*ThreadProcPtr)(void * pParameter);
public:
	CThreadWrapper_unObjectThreadPorc(  
		ThreadProcPtr threadProc,		  //对象的线程函数指针
		void* pParameter,
		bool  isWaitForRun = false
		)
		:CThread(isWaitForRun),m_threadProc(threadProc),m_pParameter(pParameter){}


	//构造函数没有传递线程函数，run之前调用init
	CThreadWrapper_unObjectThreadPorc()
		:CThread(true),m_threadProc(NULL),m_pParameter(NULL){}

	void init(ThreadProcPtr threadProc,		  //对象的线程函数指针
		void* pParameter)
	{
		m_threadProc = threadProc;
		m_pParameter = pParameter; 
	}

	~CThreadWrapper_unObjectThreadPorc()
	{
		waitThreadExit(INFINITE);
	}
private:
	unsigned long threadProc()
	{
		return m_threadProc(m_pParameter);
	}
private:

	ThreadProcPtr m_threadProc;
	void* m_pParameter;
};

/*
*	 CDaemonThread的一个外包类
要创建一个线程，只需要实例化一个CDaemonThreadWrapper对象,

线程函数写在T的实例化对象中
.*/
template<typename T>
class  CDaemonThreadWrapperEx:public CDaemonThread
{
	typedef unsigned long (T::*ThreadProcPtr)(void*);
public:
	//构造函数没有传递线程函数，run之前调用init
	CDaemonThreadWrapperEx()
		:m_objectDoingThreadProc(NULL),m_threadProc(NULL),m_pParameter(NULL){}

	void init(	T* objectDoingThreadProc,         //执行线程函数的对象
		ThreadProcPtr threadProc,
		void* pParameter)		  //对象的线程函数指针
	{
		m_threadProc = threadProc;
		m_objectDoingThreadProc = objectDoingThreadProc; 
		m_pParameter = pParameter;
	}

	~CDaemonThreadWrapperEx()
	{
		waitThreadExit(INFINITE);
	}
private:
	unsigned long daemonThreadProc()
	{
		return (m_objectDoingThreadProc->*m_threadProc)(m_pParameter);
	}
private:
	T*  m_objectDoingThreadProc;
	ThreadProcPtr m_threadProc;
	void* m_pParameter;
};


/*
*	 CDaemonThread的一个外包类
要创建一个线程，只需要实例化一个CDaemonThreadWrapper对象,

线程函数写在T的实例化对象中
.*/
template<typename T>
class  CDaemonThreadWrapper:public CDaemonThread
{
	typedef unsigned long (T::*ThreadProcPtr)();
public:
	CDaemonThreadWrapper(  
		T* objectDoingThreadProc,         //执行线程函数的对象
		ThreadProcPtr threadProc		  //对象的线程函数指针
		)
		:m_objectDoingThreadProc(objectDoingThreadProc),m_threadProc(threadProc){}


	//构造函数没有传递线程函数，run之前调用init
	CDaemonThreadWrapper()
		:m_objectDoingThreadProc(NULL),m_threadProc(NULL){}

	void init(	T* objectDoingThreadProc,         //执行线程函数的对象
		ThreadProcPtr threadProc)		  //对象的线程函数指针
	{
		m_threadProc = threadProc;
		m_objectDoingThreadProc = objectDoingThreadProc; 
	}

	~CDaemonThreadWrapper()
	{
		waitThreadExit(INFINITE);
	}
private:
	unsigned long daemonThreadProc()
	{
		return (m_objectDoingThreadProc->*m_threadProc)();
	}
private:
	T*  m_objectDoingThreadProc;
	ThreadProcPtr m_threadProc;
};

#endif

