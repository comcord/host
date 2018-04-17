#ifndef GMQuickTimer_TEMPLATE_H_2013__COM_GM_
#define GMQuickTimer_TEMPLATE_H_2013__COM_GM_

#include "GMThread.h"
#include "BaseDataType.h"
#include "GMMemAlloc.h"
#include "GMBlist.h"
#include "GMautolock.h"
#include "GMTimerAssistant.h"
#include "GMUdpSocket.h"
#include "GMRWLock.h"
#include "GMEmbedSmartPtr.h"

enum GMQuickTimerCallBackType
{
	GMQuickTimerDue = 0,		// 时间到期
	GMQuickTimerByCancel,	// 取消
};

class QuickTimerMonitorInfo;
typedef void (*T_QuickTimerCallbackProc)(GMQuickTimerCallBackType type,QuickTimerMonitorInfo* prama);

struct QuickTimerMonitorInfo:public GMMemAlloc<QuickTimerMonitorInfo, GMPrivateHeapMemAllocMethod>
{
	unsigned long long					m_escapeTime;		// 要监视的到期时间（单位毫秒）
	T_QuickTimerCallbackProc			m_pCallbackFun;     // 回调函数
	void*										m_pUserData;			// 用户自定义参数
	void*										m_ctx1;//m_pCallbackFun;		// 时间到期后执行的回调函数
	char										m_ctx2[16];//m_callbackFunPrama;	// 回调函数的参数
	void*										m_ctx3;//m_owner;					// 指向回调函数所属的类
	void*										m_ctx4;//预留参数

	QuickTimerMonitorInfo()
	{
		memset(this,0,sizeof(QuickTimerMonitorInfo));
	}
};

#define QUICKTIMER_MAXCHECKLISTNUM					2000 //正式list的个数
#define QUICKTIMER_ESCAPETIMEINTERVALPERLIST	15	    //每个list的时间间隔为15ms
#define QUICKTIMER_MAXTEMPLISTNUM					100   //临时list的个数

typedef GMBlist<QuickTimerMonitorInfo*, GMListMemAllocMethod> MonitorList;

//提供一个带引用计数的
class GMQuickTimer:public GMRefCounterBase<GMQuickTimer>
{
public:
	int GetQueueIndex()
	{
		return m_counter++%QUICKTIMER_MAXTEMPLISTNUM;
	}

	bool Enable()
	{
		GMAutoLock<GMRWLock> al(&m_lock,GMWriteOperator);
		if(!m_isEnable&&m_tempList!=NULL&&m_tempListLock!=NULL&&m_list!=NULL)
		{
			m_isEnable = true;
			return true;
		}
		return false;
	}

	void Disable()
	{
		GMAutoLock<GMRWLock> al(&m_lock,GMWriteOperator);
		if(m_isEnable)
		{
			m_isEnable = false;
			ClearAllTimerEvent();
		}
	}

	bool isEnable()
	{
		return m_isEnable;
	}

	bool SetTimer(U32 queueIndex,QuickTimerMonitorInfo* pContext)
	{
		GMAutoLock<GMRWLock> al(&m_lock,GMReadOperator);
		if(m_isEnable && queueIndex <= QUICKTIMER_MAXTEMPLISTNUM)
		{
			GMLock*		  pLock = &m_tempListLock[queueIndex];
			MonitorList* pList  = &m_tempList[queueIndex];
			pLock->lock();
			pList->push_back(pContext);
			pLock->unlock();
			return true;
		}
		else
			return false;
	}

	void PreAllocMem()
	{
		MonitorList tempList;
		//提前预分配内存
		int counter = 0;
		for(int i=0;i<100000;i++)
		{
			QuickTimerMonitorInfo* pInfo = new(std::nothrow) QuickTimerMonitorInfo;
			if(pInfo)
				tempList.push_back(pInfo);
		}

		MonitorList::iterator iter = tempList.begin();
		while(iter != tempList.end())
		{
			delete iter->value;
			iter = tempList.erase(iter);
		}	
	}

	GMQuickTimer():m_checkIndex(0),m_counter(0),m_isEnable(false),m_isExit(false),
						   m_thread(&GMQuickTimer::TimerProc, this, NULL)
	{
		m_tempList = new (std::nothrow)MonitorList[QUICKTIMER_MAXTEMPLISTNUM]; 
		m_tempListLock = new (std::nothrow)GMLock[QUICKTIMER_MAXTEMPLISTNUM];
		m_list = new (std::nothrow)MonitorList[QUICKTIMER_MAXCHECKLISTNUM];
		if((m_tempList!=NULL) && (m_tempListLock!=NULL) && (m_list!=NULL))
		{
			m_thread.run();
			//PreAllocMem();
		}
	}

	~GMQuickTimer()
	{
		m_lock.writeLock();
		m_isEnable = false;
		m_isExit = true;
		m_lock.unWriteLock();

		m_thread.waitThreadExit();
		ClearAllTimerEvent();
		delete []m_list;
		delete []m_tempListLock;
		delete []m_tempList;
	}
private:
	void TimerProc(void* p)
	{
		m_checkIndex = 0;
		memset(&m_minOtherDureTime,0xff,sizeof(m_minOtherDureTime));
		
		while(1)
		{
			{
				GMAutoLock<GMRWLock> al(&m_lock,GMReadOperator);
				TimeDueProc();
				if(m_isExit)
				{
					printf("GMQuickTimer::TimerProc:receive a destory msg and exit ,%s\r\n",m_isEnable?"true":"false");
					return;
				}
			}

			Sleep(15);
		}
		return ;
	}

	bool TimeDueProc()
	{	
		m_lastCheckTime = 0;
		MonitorList::iterator next;
		MonitorList tempMonitorInfo;

		U64 currentTime = GMTimerAssistant::GetSysCurrentTime(); //取当前时间
		if(m_lastCheckTime == 0)
			m_lastCheckTime = (currentTime/15)*15;

		//上次的检查时间和检查的checkindex相对应
		RemoveFromSortList(m_lastCheckTime,currentTime,tempMonitorInfo);

		InsertToSortList(currentTime,m_lastCheckTime,tempMonitorInfo);

		//检查otherList 缺少
		CheckOtherlist(currentTime,tempMonitorInfo);

		GMQuickTimerCallBackType type = m_isEnable?GMQuickTimerDue:GMQuickTimerByCancel;
		MonitorList::iterator iter = tempMonitorInfo.begin();
		while(iter != tempMonitorInfo.end()) //遍历所有到期的监视信息，调取监视信息中的回调函数
		{
			try
			{
				iter->value->m_pCallbackFun(type,iter->value);
			}
			catch (...)
			{
				std::cout << "GMQuickTimer::TimeDueProc()"<<__LINE__ << "unknown exception" <<std::endl; 
			}
			iter = tempMonitorInfo.erase(iter);
		}
		return true;
	}

	void  RemoveFromSortList(U64& lastCheckTime,U64 currentTime,MonitorList& outList)
	{
		int counter = 0;
		if(currentTime > lastCheckTime)
		{
			counter = (currentTime - lastCheckTime) / QUICKTIMER_ESCAPETIMEINTERVALPERLIST;
		}
		if(counter == 0)
		{
			counter++;
		}
		if(!m_isEnable) //如果已经被关闭则全部坦率
		{
			;
			MonitorList::iterator iter;
			for(int i=0; i<QUICKTIMER_MAXCHECKLISTNUM;++i)
			{
				iter = m_list[ i ].begin();
				//将sortList中的所有数据dump到outList,并将m_checkIndex++
				while (iter != m_list[ i ].end())
				{
					GMQuickTimerCallBackType type = m_isEnable?GMQuickTimerDue:GMQuickTimerByCancel;
					try
					{
						iter->value->m_pCallbackFun(type,iter->value);
					}
					catch (...)
					{
						std::cout << "GMQuickTimer::RemoveFromSortList()"<<__LINE__ << "unknown exception" <<std::endl; 
					}

					iter = m_list[ i ].erase(iter);
				}
				
			}
			
			m_checkIndex = (m_checkIndex+counter) % QUICKTIMER_MAXCHECKLISTNUM;
		}
		else
		{
			MonitorList::iterator iter;
			for(int i=0; i<counter;++i)
			{
				iter = m_list[m_checkIndex].begin();
				//将sortList中的所有数据dump到outList,并将m_checkIndex++
				while (iter != m_list[m_checkIndex].end())
				{
					GMQuickTimerCallBackType type = m_isEnable?GMQuickTimerDue:GMQuickTimerByCancel;
					try
					{
						iter->value->m_pCallbackFun(type,iter->value);
					}
					catch (...)
					{
						std::cout << "GMQuickTimer::RemoveFromSortList()"<<__LINE__ << "unknown exception" <<std::endl; 
					}

					iter = m_list[m_checkIndex].erase(iter);
				}
				m_checkIndex = (m_checkIndex+1) % QUICKTIMER_MAXCHECKLISTNUM;
			}
		}
		
		lastCheckTime += counter*QUICKTIMER_ESCAPETIMEINTERVALPERLIST;
	}

	void InsertToSortList(U64 currentTime, U64 lastCheckTime,MonitorList& tempMonitorInfo)
	{
		for (size_t i = 0; i < QUICKTIMER_MAXTEMPLISTNUM;i++ ) //遍历所有监视队列
		{
			MonitorList* pList = &m_tempList[i]; //根据桶号取得监控队列
			unsigned int monitorInfoNum = 0;

			m_tempListLock[i].lock();
			MonitorList::iterator next = pList->first();
			bool bFinished = false;
			while(!bFinished)
			{
				if ( next != pList->end() )
				{	
					if (!m_isEnable || next->value->m_escapeTime <= currentTime) //需要监视的时间已经到期
					{
						tempMonitorInfo.push_back(next->value); //将监视信息保存	
					}
					else
					{
						//加入到排序的列表中

						U64 diffTime = 0;
						if(  ( next->value->m_escapeTime > lastCheckTime) &&
							(diffTime = next->value->m_escapeTime - lastCheckTime) >= QUICKTIMER_MAXCHECKLISTNUM * QUICKTIMER_ESCAPETIMEINTERVALPERLIST )
						//if(diffTime >=  QUICKTIMER_MAXCHECKLISTNUM * QUICKTIMER_ESCAPETIMEINTERVALPERLIST)
						{
							//加入到其它检测队列
							if(next->value->m_escapeTime < m_minOtherDureTime)
							{
								m_minOtherDureTime = next->value->m_escapeTime;
								m_otherList.push_front(next->value);
							}
							else
							{
								m_otherList.push_back(next->value);							
							}
						}
						else
						{

							int index = (diffTime / QUICKTIMER_ESCAPETIMEINTERVALPERLIST + m_checkIndex) % QUICKTIMER_MAXCHECKLISTNUM;
							m_list[index].push_back(next->value);
						}
					}
					next = pList->erase(next);
					if(next == pList->end())
					{
						bFinished  = true;
						break;
					}

					monitorInfoNum++;
					if(monitorInfoNum >= 10)
					{
						m_tempListLock[i].unlock();
						monitorInfoNum = 0;
						next = pList->begin();
						m_tempListLock[i].lock();
					}
				}
				else
				{
					bFinished  = true;
				}	
			}
			m_tempListLock[i].unlock();	
		} 
	}

	void CheckOtherlist( U64 currentTime, MonitorList& tempMonitorInfo)
	{
		if(m_isEnable && m_minOtherDureTime > currentTime)
		{
			return;
		}

		MonitorList::iterator it =  m_otherList.begin();
		while(it != m_otherList.end())
		{
			if (!m_isEnable || it->value->m_escapeTime < (currentTime + 15))
			{
				tempMonitorInfo.push_back(it->value);
				m_minOtherDureTime = it->value->m_escapeTime;
				it = m_otherList.erase(it);
			}else
				it++;
		}

		if(!m_isEnable)
			memset(&m_minOtherDureTime,0xff,sizeof(m_minOtherDureTime));
			
	}

	void ClearAllTimerEvent()
	{
		MonitorList* pList = NULL;
		GMLock*		  pLock = NULL;
		for ( size_t i = 0; i < QUICKTIMER_MAXTEMPLISTNUM; ++i ) // 遍历所有监视列表，调用所有回调函数
		{
			pList = &m_tempList[i];
			pLock= &m_tempListLock[i];
			pLock->lock();
			MonitorList::iterator next = pList->first();
			while ( next != pList->end() )
			{
				try
				{
					next->value->m_pCallbackFun(GMQuickTimerByCancel,next->value);
				}
				catch (...)
				{
					std::cout << "GMQuickTimer::ClearAllTimerEvent()"<<__LINE__ << "unknown exception" <<std::endl; 
				}
				next = pList->erase(next);
			}
			pLock->unlock();
		}
	}

private:
	GMThread<GMQuickTimer, Class_Mem_Thread_Fun> m_thread;
	
	MonitorList*	m_tempList; 
	GMLock*			m_tempListLock;
	
	MonitorList*	m_list;
	U32				m_checkIndex;
	MonitorList		m_otherList;
	U64				m_minOtherDureTime;//当前系统Timer监视的到期时间
	
	GMRWLock		m_lock;				//状态锁
	U32				m_counter;
	bool			m_isEnable;
	bool			m_isExit;
	U64				m_lastCheckTime ;
};

typedef GMEmbedSmartPtr<GMQuickTimer> SptrQuickTimer;

/**
 *	定时器的模版类
 */
template <class TimerOwner>
class GMQuickTimerQueue
{
public:
	typedef int (TimerOwner::*T_QuickTimerCallbackProcEx)(GMQuickTimerCallBackType type,void* prama);

	GMQuickTimerQueue(GMQuickTimer* pQuickTimer)
	{
		m_sptrTimer.Reset(pQuickTimer);
		if (pQuickTimer)
			m_queueIndex = pQuickTimer->GetQueueIndex();
	}

	GMQuickTimerQueue():m_queueIndex(-1)
	{		
	}

	void SetQuickTimer(GMQuickTimer* pQuickTimer)
	{
		m_sptrTimer.Reset(pQuickTimer);
		if (pQuickTimer)
			m_queueIndex = pQuickTimer->GetQueueIndex();
	}

	~GMQuickTimerQueue()
	{
	}

	static void TimerDue(GMQuickTimerCallBackType type,QuickTimerMonitorInfo* prama)
	{
		try
		{
			union transType
			{
				char m_generalPoint[16];
				T_QuickTimerCallbackProcEx	m_fun;
			}t;
			memcpy(&t.m_generalPoint,&prama->m_ctx2,sizeof(prama->m_ctx2));
			TimerOwner* obj = static_cast<TimerOwner* const>(prama->m_ctx1);
			((static_cast<TimerOwner* const>(prama->m_ctx1))->*(t.m_fun))(type, prama->m_ctx3);
		}
		catch (...)
		{
			std::cout << "GMQuickTimerQueue<TimerOwner>::TimeDueProc()"<<__LINE__ << "unknown exception" <<std::endl; 
		}
		delete prama;
	}

	//提供毫秒级的定时功能，escapetime单位毫秒为相对于当前的时间值
	 bool SetTimer(unsigned int escapetime,TimerOwner* pOwner,T_QuickTimerCallbackProcEx pFun,void* prama)
	{
		if(m_sptrTimer.Get() == NULL)
			return false;
		else
		{
			union typeTrans
			{
				T_QuickTimerCallbackProcEx	m_fun;
				char									m_generalFun[16];
			}t;
			t.m_fun = pFun;
			QuickTimerMonitorInfo* pContext = new(std::nothrow) QuickTimerMonitorInfo;
			pContext->m_escapeTime = GMTimerAssistant::GetSysCurrentTime() + escapetime;
			pContext->m_pCallbackFun = &GMQuickTimerQueue<TimerOwner>::TimerDue;
			pContext->m_ctx1 = pOwner;
			memcpy(pContext->m_ctx2,&t.m_generalFun,sizeof(pContext->m_ctx2));
			pContext->m_ctx3 = prama;
			if(pContext == NULL)
				return false;
			bool bret = m_sptrTimer->SetTimer(m_queueIndex,pContext);
			if(!bret)
			{
				delete pContext;
			}
			return bret;
		}
	}
private:
	SptrQuickTimer	m_sptrTimer;
	int						m_queueIndex;
};

#endif //GMQuickTimer_TEMPLATE_H_2013__COM_GM_
