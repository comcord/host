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
	 *	�������� �����׳� CSocketException \ CTimeoutException
	 *  ���ؽ��յ����ֽ���
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

	//���ܶ�ε���
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
	*	�ȴ��߳��˳�. <run���غ�ſ�ִ�еȴ�>
		true ����ȴ��˳��ɹ�
		false ����ȴ���ʱ
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
	 *	�̺߳���������OVERWRITE�ú���
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
	*	�̺߳���������OVERWRITE�ú���
	*/
	virtual unsigned long daemonThreadProc(void) = 0;

	volatile bool m_active;

};

/*
*	 CThread��һ�������
Ҫ����һ���̣߳�ֻ��Ҫʵ����һ��CThreadWrapper����,

�̺߳���д��T��ʵ����������
.*/
template<typename T>
class  CThreadWrapper:public CThread
{
	typedef unsigned long (T::*ThreadProcPtr)(void * pParameter);
public:
	CThreadWrapper(  
		T* objectDoingThreadProc,         //ִ���̺߳����Ķ���
		ThreadProcPtr threadProc,		  //������̺߳���ָ��
		void* pParameter,
		bool  isWaitForRun = false
		)
		:CThread(isWaitForRun),m_objectDoingThreadProc(objectDoingThreadProc),m_threadProc(threadProc),m_pParameter(pParameter){}

	//���캯��û�д����̺߳�����run֮ǰ����init
	CThreadWrapper()
		:CThread(true),m_objectDoingThreadProc(NULL),m_threadProc(NULL),m_pParameter(NULL){}

	void init(T* objectDoingThreadProc,         //ִ���̺߳����Ķ���
		ThreadProcPtr threadProc,		  //������̺߳���ָ��
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
*	 CThread��һ�������
Ҫ����һ���̣߳�ֻ��Ҫʵ����һ��CThreadWrapper����,

�̺߳���Ϊȫ�ֺ������߾�̬����
.*/
class  CThreadWrapper_unObjectThreadPorc:public CThread
{
	typedef unsigned long (*ThreadProcPtr)(void * pParameter);
public:
	CThreadWrapper_unObjectThreadPorc(  
		ThreadProcPtr threadProc,		  //������̺߳���ָ��
		void* pParameter,
		bool  isWaitForRun = false
		)
		:CThread(isWaitForRun),m_threadProc(threadProc),m_pParameter(pParameter){}


	//���캯��û�д����̺߳�����run֮ǰ����init
	CThreadWrapper_unObjectThreadPorc()
		:CThread(true),m_threadProc(NULL),m_pParameter(NULL){}

	void init(ThreadProcPtr threadProc,		  //������̺߳���ָ��
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
*	 CDaemonThread��һ�������
Ҫ����һ���̣߳�ֻ��Ҫʵ����һ��CDaemonThreadWrapper����,

�̺߳���д��T��ʵ����������
.*/
template<typename T>
class  CDaemonThreadWrapperEx:public CDaemonThread
{
	typedef unsigned long (T::*ThreadProcPtr)(void*);
public:
	//���캯��û�д����̺߳�����run֮ǰ����init
	CDaemonThreadWrapperEx()
		:m_objectDoingThreadProc(NULL),m_threadProc(NULL),m_pParameter(NULL){}

	void init(	T* objectDoingThreadProc,         //ִ���̺߳����Ķ���
		ThreadProcPtr threadProc,
		void* pParameter)		  //������̺߳���ָ��
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
*	 CDaemonThread��һ�������
Ҫ����һ���̣߳�ֻ��Ҫʵ����һ��CDaemonThreadWrapper����,

�̺߳���д��T��ʵ����������
.*/
template<typename T>
class  CDaemonThreadWrapper:public CDaemonThread
{
	typedef unsigned long (T::*ThreadProcPtr)();
public:
	CDaemonThreadWrapper(  
		T* objectDoingThreadProc,         //ִ���̺߳����Ķ���
		ThreadProcPtr threadProc		  //������̺߳���ָ��
		)
		:m_objectDoingThreadProc(objectDoingThreadProc),m_threadProc(threadProc){}


	//���캯��û�д����̺߳�����run֮ǰ����init
	CDaemonThreadWrapper()
		:m_objectDoingThreadProc(NULL),m_threadProc(NULL){}

	void init(	T* objectDoingThreadProc,         //ִ���̺߳����Ķ���
		ThreadProcPtr threadProc)		  //������̺߳���ָ��
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

