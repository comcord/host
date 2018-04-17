#pragma once
#include "GMThreadEx.h"
#include "GMSingleTon.h"
#include "GMRWLock.h"
//#include "AverageCounterByTime.h"
#include "FQueue.h"
/*
Ԥ�ȴ���һ��������̡߳�?
���ص��߳���Ҫ������delete,�̴߳ӳ���ȡ���˾Ͳ��ڹ黹�ˡ�
*/
template<class ThreadOwner>
class GMThreadPool:public GMSingleTon<GMThreadPool<ThreadOwner> >
{
	typedef void (ThreadOwner::*GMThreadFromPoolThreadProc)(void* );

	friend class  GMSingleTon<GMThreadPool>;

public:
	/*�ȴ���һЩ�߳�
	�ɹ�����0,-1��ʾ�ظ�����init -2��ʾ�����������߳��߳�ʧ��
	thread_stack_size����0 ʹ��Ĭ�ϴ�С
	*/
	int init(unsigned int pre_created_thread_num,unsigned int thread_stack_size = 0);

	//���ٶ�������δʹ�õ��߳�
	void uninit();

	//�Ӷ�����ȡһ������������Ϊ�գ��򴴽�һ��
	GMThreadEx<ThreadOwner>* create(ThreadOwner* owner,GMThreadFromPoolThreadProc proc);

private:
	GMThreadPool():m_thread_stack_size(0),m_pre_created_thread_num(0),m_notEnoughThreadEvent(true,true),m_bWorking(false){}

	void createThreadProc(void* );
private:

	typedef FQueue<GMThreadEx<ThreadOwner>* > ThreadList;
	unsigned int m_thread_stack_size;
	unsigned int m_pre_created_thread_num;
	GMLock m_threadPoolLock;
	ThreadList m_threadPool;
	
	GMRWLock m_bWorkingLock;
	bool m_bWorking;

	GMThreadEx<GMThreadPool>  m_createThreadThread;
	GMSysEvent m_notEnoughThreadEvent;

	//AverageCounterByTime 
};

template<class ThreadOwner>
int GMThreadPool<ThreadOwner>::init(unsigned int preCreatedThreadNum,unsigned int thread_stack_size)
{
	GMAutoLock<GMRWLock> al(&m_bWorkingLock,GMWriteOperator);

	if (m_bWorking)return -1;

	m_bWorking = true;

	m_pre_created_thread_num = preCreatedThreadNum;
	m_thread_stack_size = thread_stack_size;

	if(!m_threadPool.init(m_pre_created_thread_num*2))//�Ŵ�һ�㣬��Ϊ�����ǹ̶���
		return -2;

	if(!m_createThreadThread.create())
		return -3;

	m_createThreadThread.run(&GMThreadPool<ThreadOwner>::createThreadProc,this,NULL);

	return 0;
}

template<class ThreadOwner>
void GMThreadPool<ThreadOwner>::uninit() 
{
	GMAutoLock<GMRWLock> al(&m_bWorkingLock,GMWriteOperator);

	if(!m_bWorking)return;

	m_bWorking = false;
	m_notEnoughThreadEvent.signal();
	m_createThreadThread.waitThreadExit();


	GMThreadEx<ThreadOwner>* pThread = NULL;
	GMAutoLock<GMLock> al2(&m_threadPoolLock);
	while(m_threadPool.pop_front(pThread))
	{
		pThread->run(NULL,NULL,NULL);
		pThread->waitThreadExit();
		delete pThread;
	}

	m_threadPool.uninit();
}

template<class ThreadOwner>
GMThreadEx<ThreadOwner>* GMThreadPool<ThreadOwner>::create(ThreadOwner* owner,GMThreadFromPoolThreadProc proc)
{
	GMAutoLock<GMRWLock> al(&m_bWorkingLock,GMReadOperator);
	if(!m_bWorking)return NULL;

	GMThreadEx<ThreadOwner>* pThread = NULL;

	{
		GMAutoLock<GMLock> al(&m_threadPoolLock);
	
		if (m_threadPool.pop_front(pThread))
			pThread->run(proc,owner,NULL);

		if (m_threadPool.size() < 2*m_pre_created_thread_num / 3)//����signal����
			m_notEnoughThreadEvent.signal();

		if (NULL != pThread)
			return pThread;
	}

	//printf("%d fail to get thread from pool\r\n",GetCurrentThreadId());

	pThread = new  GMThreadEx<ThreadOwner>();

	if (NULL == pThread)
		return NULL;

	if(!pThread->create(m_thread_stack_size))
	{
		delete pThread;
		return NULL;
	}

	pThread->run(proc,owner,NULL);

	return pThread;
}

template<class ThreadOwner>
void GMThreadPool<ThreadOwner>::createThreadProc(void* )
{
	GMThreadEx<ThreadOwner>* pThread = NULL;

	while(m_bWorking)
	{
		m_notEnoughThreadEvent.wait();

		while(true)
		{
			if(!m_bWorking)return;

		/*	{
				GMAutoLock<GMLock> al(&m_threadPoolLock);

				if(m_threadPool.size() >= m_pre_created_thread_num)
					break;
			}*/

			pThread = new  GMThreadEx<ThreadOwner>();
			if (NULL == pThread)
			{
				Sleep(100);
				continue;;
			}

			//U32 bt = GetTickCount();

			if(!pThread->create(m_thread_stack_size))
			{
				delete pThread;
				pThread = NULL;
				Sleep(100);
				continue;
			}
			
	/*		U32 interval = GetTickCount() - bt;
			if (interval>0)
				printf("%d create takes %d\n",GetCurrentThreadId() ,interval);*/

			{
				GMAutoLock<GMLock> al(&m_threadPoolLock);
			
				if(!m_threadPool.push_back(pThread))
				{
					printf("GMThreadPool<ThreadOwner>::createThreadProc push_back logic error,please tell yecw\r\n");

					assert(false);//�����ܵ��ߵ���·��
					pThread->run(NULL,NULL,NULL);
					pThread->waitThreadExit();
					delete pThread;
					break;
				}

				if(m_threadPool.size() >= m_pre_created_thread_num)
					break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

class  GMThreadFromPool
{
public:

	GMThreadFromPool( bool isWaitForRun = false):m_thread(NULL)
	{

	}
	
	virtual ~GMThreadFromPool()
	{
		if (m_thread)delete m_thread;
	}
	
	//���ܶ��ε���
	bool run()
	{
		m_thread = GMThreadPool<GMThreadFromPool>::GetInst()->create(this,&GMThreadFromPool::internalThreadProc);
		return NULL != m_thread;
	}

	void shutdown()
	{
		if (m_thread)
			m_thread->kill();
	}

	//ʧ�ܷ���0
#ifdef WIN32
	HANDLE getHandle()
	{
		if (m_thread)
			return m_thread->getHandle();
		else
			return NULL;
	};
#endif
	/*
	*	�ȴ��߳��˳�. <run���غ��ſ�ִ�еȴ�>
	 */
	bool waitThreadExit(unsigned long time = INFINITE)
	{
		if (m_thread)
			return m_thread->waitThreadExit(time);
		else
			return true;
	}
#ifdef WIN32
	DWORD getThreadID()
#else
	pthread_t getThreadID()
#endif
	{
		if (m_thread)
			return m_thread->getThreadID().m_threadId;
		else
			return 0;
	};

protected:	
	/*
	 *	�̺߳���������OVERWRITE�ú���
	 */
	virtual unsigned long threadProc(void) = 0;	

private:
	void internalThreadProc(void* )//�����еļ��ݣ�ת������ԭ��
	{
		threadProc();
		//printf("GMThreadFromPool %d, handle %d.\r\n",getThreadID(),getHandle());
	}

private:
	GMThreadEx<GMThreadFromPool>* m_thread;
};

typedef GMThreadPool<GMThreadFromPool> ThreadPoolForGMThreadFromPool;
