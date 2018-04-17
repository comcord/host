#ifndef GMCustomTimer_TEMPLATE_H_2008__COM_GM_
#define GMCustomTimer_TEMPLATE_H_2008__COM_GM_


#include "BaseDataType.h"
#include "GMMemAlloc.h"
#include "GMBlist.h"
#include "GMautolock.h"
#include "GMTimerAssistant.h"
#include "GMUdpSocket.h"
#include "GMEvent.h"
#include "GMThread.h"
#include "GMSocketException.h"
#ifdef WIN32
#define LASTINGWAITTIME 0xf0ffffffffffffff
#define FOREVER		0x7fffffffffffffff
#else // linux
#define  LASTINGWAITTIME 0x7fffffffffffffffll
#define  FOREVER  0xffffffffffffffffll
#endif

enum GMCustomTimerCallBackType
{
	GMCustomTimerDue = 0,		// 时间到期
	GMCustomTimerByCancel,		// 取消
};

typedef unsigned int (*GMCalculateBucketIndex)(unsigned int time);

/**
 *	定时器的模版类
 */
template <class TimerOwner>
class GMCustomTimerEx
{
public:

	typedef int (TimerOwner::*T_TimerCallbackProcEx)(GMCustomTimerCallBackType type,void* prama);

	GMCustomTimerEx(unsigned int bucketNum = 1, GMCalculateBucketIndex pCalFun = NULL);

	~GMCustomTimerEx();

	//提供毫秒级的定时功能，escapetime单位毫秒为相对于当前的时间值
	bool SetTimer(unsigned int escapetime,TimerOwner* pOwner,T_TimerCallbackProcEx pFun,void* prama);

	//清空所有的定时器事件，调用所有回调函数
	void ClearAllTimerEvent();

	//使Timer有效
	void Enable();

	//使Timer无效，在此状态下Timer不接收始终订阅，
	void Disable();

	// 判断当前Timer是否处在可用状态
	bool IsEnable();

#ifdef _DEBUG
	/**
	*	打印出各个队列中存储的所有定时值
	*/
	void PrintList();
#endif

private:
	//定时器线程处理函数
	void  TimerProc(void* prama);

	//将用户设置的时钟事件,按照时间先后排序后加入的队列中
	bool InsertToList(unsigned int escapetime,TimerOwner* pOwner,T_TimerCallbackProcEx pFun, void* prama);

	//从列表中至多找到100个超时的对象并发送超时消息到消息队列
	bool TimeDueProc();

	//从监控队列里获取下一个有效的监视时间，将下一个有效时间赋给m_currentMonitorTime
#ifdef WIN32
	void GetNextMonitorTime(LARGE_INTEGER& latestTime);
#else
	void GetNextMonitorTime(U64& latestTime);
#endif
private:
	//用UDP SOCKET实现的时事件
	class GMSysEventBySocketIMP 
	{
		//friend GMCustomTimerEx<TimerOwner>;
	public:
		GMSysEventBySocketIMP();
		bool signal();
		bool reset();
		GMSysEventBySocketIMP * getHandle(){return this;}
		void setMaxSingalCount(U32 c);
		bool fethSingal();	// 获取信号
		GMUdpSocket m_udp;
	private:
		GMLock m_lock;
		T_IP m_localip; 
		T_PORT m_localport;
		volatile U32 m_eventCount;
	};
	//等待多个用SOCKET实现的EVENT(to单位是毫秒)
	int WaitMultiGMSysEventBySocketIMP(/*GMSysEventBySocketIMP parray[], int len*/ unsigned long long to);

private:
	struct MonitorInfo
	{

		unsigned long long  m_escapeTime;		// 要监视的到期时间（单位毫秒）
		T_TimerCallbackProcEx m_pCallbackFun;		// 时间到期后执行的回调函数

		void* m_callbackFunPrama;		// 回调函数的参数

		TimerOwner* m_owner;				// 指向回调函数所属的类

		MonitorInfo()
		{
			memset(this,0,sizeof(MonitorInfo));
		}

		MonitorInfo(
			unsigned long long escapeTime,
			T_TimerCallbackProcEx pCallbackFun,

			void* callbackFunPrama,

			TimerOwner* m_owner):
			m_escapeTime(escapeTime),
			m_pCallbackFun(pCallbackFun),
			m_callbackFunPrama(callbackFunPrama),
			m_owner(m_owner)
		{}

		MonitorInfo& operator= (const MonitorInfo& right)
		{
			m_escapeTime = right.m_escapeTime;
			m_pCallbackFun = right.m_pCallbackFun;
			m_callbackFunPrama = right.m_callbackFunPrama;
			m_owner = right.m_owner;
			return *this;
		}
	};

	typedef GMBlist<MonitorInfo, GMListMemAllocMethod> MonitorList;

	GMThread<GMCustomTimerEx<TimerOwner>, Class_Mem_Thread_Fun> m_thread;
	MonitorList* m_list;
	GMLock		 m_lock;
	
	unsigned int m_bucketNum;	// 桶数量
	unsigned long long  m_currentMonitorTime; //当前系统Timer监视的到期时间
#ifdef WIN32 
	GMSysEvent m_timerResetEvent; //时钟重新设置通知事件
	GMSysEvent m_exitEvent;		 //Timer线程退出通知事件
#else
	GMSysEventBySocketIMP m_timerResetEvent;	//时钟重新设置通知事件
	GMSysEventBySocketIMP m_exitEvent;		//Timer线程退出通知事件
	//GMSysEventBySocketIMP m_timeEvent;
#endif

	GMCalculateBucketIndex m_calcuBucketIndexfun; // 指向计算桶索引函数地址

	bool m_isExit;			// ???
	bool m_isEnable;		// 能否使用定时器
};

template <class TimerOwner>
GMCustomTimerEx<TimerOwner>::~GMCustomTimerEx(void)
{
	if (IsEnable())
		Disable();

	m_exitEvent.signal();
	//WaitForSingleObject(m_threadHandle,INFINITE); 
	//CloseHandle(m_threadHandle);
	m_thread.waitThreadExit();
	ClearAllTimerEvent();
	delete []m_list;
}

#pragma warning(disable:4355)

template <class TimerOwner>
#ifdef WIN32
GMCustomTimerEx<TimerOwner>::GMCustomTimerEx(unsigned int bucketNum,GMCalculateBucketIndex pCalFun):
								m_calcuBucketIndexfun(pCalFun),
								m_isEnable(false),
								m_timerResetEvent(true),
								m_lock(true),
								m_thread(&GMCustomTimerEx<TimerOwner>::TimerProc, this, NULL)
#else // linux
GMCustomTimerEx<TimerOwner>::GMCustomTimerEx(unsigned int bucketNum,GMCalculateBucketIndex pCalFun):
								m_calcuBucketIndexfun(pCalFun),
								m_isEnable(false),
								m_lock(true),
								m_thread(&GMCustomTimerEx<TimerOwner>::TimerProc, this, NULL)
#endif
{	
	m_bucketNum = (bucketNum < 1 ? 1: bucketNum);
	m_list = new(std::nothrow) MonitorList[m_bucketNum]; // 创建桶数个MonitorList
//	m_threadHandle = CreateThread(NULL,0,TimerProc,this,0,NULL);
	m_currentMonitorTime = FOREVER;
	m_thread.run();	
	//m_timeEvent.setMaxSingalCount(); // ???
}

#pragma warning(default:4355)


template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::SetTimer(unsigned int escapetime,TimerOwner* pOwner,T_TimerCallbackProcEx pFun,void* prama)
{
	if ( pOwner == NULL || pFun == NULL || escapetime < 0 )
		return false;

	GMAutoLock<GMLock> al(&m_lock);
	if (!IsEnable()) // 定时器不能使用则返回
		return false;
	
	return InsertToList(escapetime,pOwner,pFun,prama);
}

template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::InsertToList(unsigned int escapetime,
										   TimerOwner* pOwner,
										   T_TimerCallbackProcEx pFun, 
										   void* prama)
{
	MonitorList* pList = NULL;
	if (m_calcuBucketIndexfun == NULL) // 没有指定计算桶索引函数
	{
		pList = &m_list[0];
	}
	else // 根据escapetime计算出使用的桶号
	{
		unsigned int ret = m_calcuBucketIndexfun(escapetime);
		if (ret < m_bucketNum)
			pList = &m_list[ret];//liuju20081120修改
		else
			return false;
	}
	
	//按照从小到大的顺序插入到m_list
	unsigned long long iEscapetime = (unsigned long long)escapetime + GMTimerAssistant::GetSysCurrentTime();  // 计算出超时时间点（单位毫秒）

	if (pList->empty())
	{
		pList->push_back(MonitorInfo(iEscapetime,pFun,prama,pOwner));
	}
	else
	{
		typename MonitorList::iterator last = pList->last(); // 找出队列中最晚的时间
		while (last != pList->end())
		{
			if ((*last).m_escapeTime > iEscapetime) // 需要插入时间早于list当前时间
			{
				if (last == pList->first())
				{
					pList->push_front(MonitorInfo(iEscapetime,pFun,prama,pOwner));
					break;
				}
				last--;
			}
			else // 需要插入时间晚于list当前的时间
			{
				if (last == pList->last())
				{
					pList->push_back(MonitorInfo(iEscapetime,pFun,prama,pOwner));
				}
				else
				{
					last++;
					pList->insert(last,MonitorInfo(iEscapetime,pFun,prama,pOwner));
				}
				break;
			}
		}
	}

	if ( iEscapetime < m_currentMonitorTime) // 要插入的时间点晚于当前监控的时间点
	{
		m_timerResetEvent.signal();
	}

	return true;
}


template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::TimeDueProc()
{	
	typename MonitorList::iterator next;
	unsigned long long escapeTime;
	MonitorList* pList;
	MonitorInfo tempMonitorInfo[100];
	unsigned int monitorInfoNum = 0;
	bool checkFinished = false;
	size_t i = 0;

	while ( i < m_bucketNum ) // 遍历所有监视队列
	{
		escapeTime = GMTimerAssistant::GetSysCurrentTime(); //取当前时间
		pList = &m_list[i]; // 根据桶号取得监控队列

		{
			GMAutoLock<GMLock> al(&m_lock);
			next = pList->first();

			while ( next != pList->end() )
			{	
				if (next->value.m_escapeTime <= escapeTime) // 需要监视的时间已经到期
				{
					tempMonitorInfo[monitorInfoNum] = next->value; // 将监视信息保存
					monitorInfoNum ++;
					next = pList->erase(next);
					if (monitorInfoNum >= 100)
						break;
				}
				else
				{
					break;
				}
			}
			
			if ( monitorInfoNum < 100 ) // 没有找到100个监视信息，且所有队列已经查找完毕
			{
				if ( i == m_bucketNum - 1 )
					checkFinished = true;
				++i;
			}
		}
		
		if ((monitorInfoNum >= 100) || checkFinished)
		{
			for ( size_t j = 0; j < monitorInfoNum; ++j ) // 遍历所有到期的监视信息，调取监视信息中的回调函数
			{
				try
				{
					if (tempMonitorInfo[j].m_pCallbackFun && tempMonitorInfo[j].m_owner)
						((static_cast<TimerOwner* const>(tempMonitorInfo[j].m_owner))->*(tempMonitorInfo[j].m_pCallbackFun))(GMCustomTimerDue, tempMonitorInfo[j].m_callbackFunPrama);
				}
				catch (...)
				{
					std::cout << "GMCustomTimerEx<TimerOwner>::TimeDueProc()"<<__LINE__ << "unknown exception" <<std::endl; 
				}
			}

			monitorInfoNum = 0;
		}

		if ( checkFinished )
			break;
	} // while

	return true;
}


#ifdef WIN32//win32
template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::TimerProc(void* prama)
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER dueTime;
	dueTime.LowPart = 0;

	hTimer = CreateWaitableTimer(NULL,true,NULL); //100纳秒精度的计时器
	if (!hTimer)
	{
		return ;
	}

	while(1)
	{
		GetNextMonitorTime(dueTime);

		if (!SetWaitableTimer(hTimer, &dueTime, 0, NULL, NULL, 0))
		{
			CloseHandle(hTimer);
			return ;
		}

		HANDLE hEventArray[4];
		hEventArray[0] = hTimer;
		hEventArray[1] = m_timerResetEvent.getHandle();
		hEventArray[2] = m_exitEvent.getHandle();

		unsigned int ret = WaitForMultipleObjects(3,hEventArray,false,INFINITE);
		switch( ret)
		{
			case WAIT_OBJECT_0:
				TimeDueProc();
				break;
			case WAIT_OBJECT_0 + 1:
				//my->m_timerResetEvent.reset();
				break;
			case WAIT_OBJECT_0 + 2:
				CloseHandle(hTimer);
				return ;
			default:
				CloseHandle(hTimer);
				return ;
		}
	}
	return ;
}


template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::GetNextMonitorTime(LARGE_INTEGER& latestTime)
{
	MonitorList* pList = NULL;
	MonitorList::iterator next;
	unsigned __int64 currentTime;
	unsigned __int64 nextTime = 0;

	bool listIsEmpty = true;
	{
		GMAutoLock<GMLock> al(&m_lock);
		if(m_timerResetEvent.wait(0) == 1)
		{
			m_timerResetEvent.reset();
		}

		currentTime = GMTimerAssistant::GetSysCurrentTime();
		U64 minDueTime = 0;

		for(size_t i=0; i<m_bucketNum; ++i)
		{	
			pList = &m_list[i];
			if(!pList->empty())
			{
				next = pList->first();
				nextTime = next->value.m_escapeTime;
				if(currentTime >= nextTime)
				{
					latestTime.QuadPart = -1;
					listIsEmpty = false;
					return;
				}
				else
				{
					if(minDueTime == 0)
						minDueTime = nextTime;
					else
					{
						if(nextTime < minDueTime)
							minDueTime = nextTime;
					}
					//latestTime.QuadPart = (currentTime - nextTime)*10*1000;
					//m_currentMonitorTime = nextTime;
					listIsEmpty = false;
				}
			}
		}

		if(listIsEmpty)
		{
			latestTime.QuadPart  = LASTINGWAITTIME;
			m_currentMonitorTime = FOREVER;
		}
		else
		{
			latestTime.QuadPart = (currentTime - minDueTime)*10*1000;
			m_currentMonitorTime = minDueTime;
		}
	}
}
#else //linux
template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::TimerProc(void* prama)
{
	//GMCustomTimerEx<TimerOwner>* my = (GMCustomTimerEx<TimerOwner>*)prama;
	U64 dueTime = 0;
	while (1)
	{
		GetNextMonitorTime(dueTime);
		
		if (dueTime == 0)//当前时间比监视列表中时间更晚，则从队列中找到超时对象并回调
		{
			TimeDueProc();
			continue;
		}
		int ret = WaitMultiGMSysEventBySocketIMP(dueTime);
	
		switch( ret)
		{
		case 0:// 超时,则将所有到期定时器回调
			TimeDueProc();
			break;

		case 1://时钟重置
			break;

		case 2: // 退出事件产生,TimerProc线程退出
			printf(" case 2 TimerProc Thread exit!\n");
			return;

		default: // WaitMultiGMSysEventBySocketIMP中select函数发生错误
			printf("default TimerProc Thread exit!\n");
			return ;
		}
	}
	return ;
}

template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::GetNextMonitorTime(U64& latestTime)
{
	MonitorList* pList = NULL;
	typename MonitorList::iterator next;
	U64 currentTime;
	U64 nextTime = 0;

	bool listIsEmpty = true;

	{
		GMAutoLock<GMLock> al(&m_lock);

		currentTime = GMTimerAssistant::GetSysCurrentTime();

		/**
		 *	找出现有队列中头值最小的队列号
		 */
		size_t minListNO = 0;
		U64 uTempMin = FOREVER;
		for ( size_t i = 0; i < m_bucketNum; ++i )
		{
			if ( !m_list[i].empty() )
			{
				typename MonitorList::iterator itor = m_list[i].first();
				if ( (*itor).m_escapeTime < uTempMin )
				{
					minListNO = i;
					uTempMin = (*itor).m_escapeTime;
				}
			}
		}

		pList = &m_list[minListNO]; // 指向拥有当前最早定时时间的队列

		if (!pList->empty())
		{
			next = pList->first();
			nextTime = next->value.m_escapeTime;
			if (currentTime >= nextTime)		// 当前时间比最小监视时间晚
			{
				latestTime = 0;
			}
			else
			{
				latestTime= (nextTime - currentTime);
				m_currentMonitorTime = nextTime;
			}
			listIsEmpty = false;
		}


		//for (size_t i = 0; i < m_bucketNum; ++i )
		//{	
		//	pList = &m_list[i];
		//	if (!pList->empty())
		//	{
		//		next = pList->first();
		//		nextTime = next->value.m_escapeTime;
		//		if (currentTime >= nextTime)		// 当前时间比最小监视时间晚
		//		{
		//			latestTime = 0;
		//		}
		//		else
		//		{
		//			latestTime= (nextTime - currentTime);
		//			m_currentMonitorTime = nextTime;
		//		}
		//		listIsEmpty = false;
		//		break;
		//	}
		//}

		if (listIsEmpty)
		{
			latestTime  = LASTINGWAITTIME;
			m_currentMonitorTime = FOREVER;
		}
	}
}

#endif


template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::ClearAllTimerEvent()
{
	GMAutoLock<GMLock> al(&m_lock);
	MonitorList* pList = NULL;

	for ( size_t i = 0; i < m_bucketNum; ++i ) // 遍历所有监视列表，调用所有回调函数
	{
		pList = &m_list[i];
		typename MonitorList::iterator next = pList->first();
		while ( next != pList->end() )
		{
			try
			{
				if (next->value.m_pCallbackFun && next->value.m_owner)
					((static_cast<TimerOwner* const>(next->value.m_owner))->*(next->value.m_pCallbackFun))(GMCustomTimerByCancel, next->value.m_callbackFunPrama);
			}
			catch (...)
			{
				std::cout << "GMCustomTimerEx<TimerOwner>::TimeDueProc()"<<__LINE__ << "unknown exception" <<std::endl; 
			}
			next = pList->erase(next);
		}
	}
}

template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::Enable()
{
	GMAutoLock<GMLock> al(&m_lock);
	m_isEnable = true;
}

template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::Disable()
{
	GMAutoLock<GMLock> al(&m_lock);
	m_isEnable = false;
}

template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::IsEnable()
{
	return m_isEnable;
}

template <class TimerOwner>
int GMCustomTimerEx<TimerOwner>::WaitMultiGMSysEventBySocketIMP(unsigned long long to )
{
	timeval tm;
	fd_set rd;
	FD_ZERO(&rd);
	int maxfd = 0;

	//FD_SET(m_timeEvent.m_udp.m_nSock, &rd);
	FD_SET(m_timerResetEvent.m_udp.m_nSock, &rd);
	FD_SET(m_exitEvent.m_udp.m_nSock, &rd);

	//maxfd = gmmax(m_timeEvent.m_udp.m_nSock, m_timerResetEvent.m_udp.m_nSock);
	//maxfd = gmmax(m_exitEvent.m_udp.m_nSock, maxfd);
	maxfd = gmmax(m_timerResetEvent.m_udp.m_nSock, m_exitEvent.m_udp.m_nSock);

	// 如果队列中没有定时信息,则无限阻塞等待
	int r = 0;
	if ( to == LASTINGWAITTIME )
	{
		tm.tv_sec = 60;
		tm.tv_usec = 0;
		r = select(maxfd + 1, &rd, NULL, NULL, &tm);

	}
	else // 根据to计算等待时间,进行等待
	{
		tm.tv_sec = to / 1000;
		tm.tv_usec = (to % 1000) * 1000;

		r = select(maxfd + 1, &rd, NULL, NULL, &tm);
	}

	if ( r == 0 ) // 超时
	{
		return 0;
	}
	else if ( r > 0 )
	{
		if (FD_ISSET( m_timerResetEvent.m_udp.m_nSock, &rd)) // 时间置位事件,将重获监控时间
		{
			m_timerResetEvent.fethSingal();
			return 1;
		}
		if (FD_ISSET( m_exitEvent.m_udp.m_nSock, &rd)) // GMCustomTimerEx类析构，监控线程退出
		{
			return 2;
		}
		//if (FD_ISSET( m_timeEvent.m_udp.m_nSock, &rd))
		//{
		//	std::cerr << "WaitMultiGMSysEventBySocketIMP select, please be sure timeevent nerver no singal"<<std::endl;
		//	m_timeEvent.fethSingal();
		//	assert(false);
		//	return -1;
		//}
		printf("is there\n");
		return r;
	}
	else
	{
		perror("WaitMultiGMSysEventBySocketIMP select error:");
		return r;
	}
}

template <class TimerOwner>
GMCustomTimerEx<TimerOwner>::GMSysEventBySocketIMP::GMSysEventBySocketIMP()
{
	IPZERO(m_localip);
	m_localport = 0;
	m_eventCount = 0;

	m_udp.createSocket();
	m_udp.bind();//由计算自己选择一个端口
	m_udp.getLocalAddr(m_localip, m_localport);
}
template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::GMSysEventBySocketIMP::signal()
{
	const char buff[] = {"event_signal"};
	try
	{
		GMAutoLock<GMLock> a(&m_lock);
		if (m_eventCount == 0)
		{
			++m_eventCount;
			m_udp.sendDgramTo((unsigned char*)buff, strlen(buff), m_localip, m_localport);
		}

	}
	catch (GMSocketException &e)
	{
		perror(e.what());
		return false;
	}
	return true;
}

template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::GMSysEventBySocketIMP::reset()
{
	char buff[2048];//防止受到其他UDP发送过来的包，缓存大些
	T_IP fromip;
	T_PORT fromport;
		
	GMAutoLock<GMLock> a(&m_lock);
	while (m_eventCount > 0)
	{
		memset(buff, 0, sizeof(buff));
		try
		{
			if(m_udp.recvDgramFrom(buff, sizeof(buff),fromip, fromport) > 0)
			{
				if ( 0 == strncmp(buff, "event_signal", strlen("event_signal")) )
					--m_eventCount;
			}
		}
		catch (GMSocketException& e)
		{
			perror(e.what());
			return false;
		}
	}
	return true;
}
template <class TimerOwner>
bool GMCustomTimerEx<TimerOwner>::GMSysEventBySocketIMP::fethSingal()
{
	char buff[2048];
	T_IP fromip;
	T_PORT fromport;

	GMAutoLock<GMLock> a(&m_lock);
	while (m_eventCount > 0)
	{
		memset(buff, 0, sizeof(buff));
		try
		{
			if (m_udp.recvDgramFrom((unsigned char*)(buff), sizeof(buff),fromip, fromport) >= strlen("event_signal"))
			{
				if ( 0 == strncmp(buff, "event_signal", strlen("event_signal")) )
				{
					--m_eventCount;
					return true;
				}
			}
		}
		catch (GMSocketException& e)
		{
			perror(e.what());
			return false;
		}
	}
	return false;
}

template <class TimerOwner>
void GMCustomTimerEx<TimerOwner>::GMSysEventBySocketIMP::setMaxSingalCount(U32 c)
{
	m_udp.setSockBuffSize(8 *1024 , gmmax(8*1024, c * 16));
}

#ifdef _DEBUG
template<class TimerOwner>
void GMCustomTimerEx<TimerOwner>::PrintList()
{
	MonitorList* pList = NULL;
	for ( unsigned int i = 0; i < m_bucketNum; ++i ) // 遍历每个List，将其中存储的定时值依次打印出来
	{
		pList = &m_list[i];
		printf("List %u:\n", i);
		typename MonitorList::iterator itor = pList->begin();
		for ( ; itor != pList->end(); itor++ )
		{
			printf("%t %llu para=%u\n", (*itor).m_escapeTime, (*itor).m_callbackFunPrama);
		}
		printf("\n");
	}
}
#endif

template <class TimerOwner>
class GMCustomTimer
{

public:

#ifdef X64
	typedef int (TimerOwner::*T_TimerCallbackProc)(GMCustomTimerCallBackType type,void* prama);
#else
	typedef int (TimerOwner::*T_TimerCallbackProc)(GMCustomTimerCallBackType type,unsigned int prama);
#endif

	class Param
	{
	public:
		TimerOwner* m_owner;
		T_TimerCallbackProc m_userFun;

#ifdef X64
		void* m_prama;
#else
		unsigned int m_prama;
#endif
	};

	GMCustomTimer(unsigned int bucketNum = 1, GMCalculateBucketIndex pCalFun = NULL):m_timer(bucketNum,pCalFun){}

#ifdef X64
	bool SetTimer(unsigned int escapetime,TimerOwner* pOwner,T_TimerCallbackProc pFun,void* prama)
	{
		Param* p = new Param();
		p->m_owner = pOwner;
		p->m_userFun = pFun;
		p->m_prama = prama;
		if (NULL == p)
			return false;

		return m_timer.SetTimer(escapetime,this,&GMCustomTimer<TimerOwner>::timerCallbackProc,p);
	}
#else
	bool SetTimer(unsigned int escapetime,TimerOwner* pOwner,T_TimerCallbackProc pFun,unsigned int prama)
	{
		Param* p = new Param();
		p->m_owner = pOwner;
		p->m_userFun = pFun;
		p->m_prama = prama;
		if (NULL == p)
			return false;

		return m_timer.SetTimer(escapetime,this,&GMCustomTimer<TimerOwner>::timerCallbackProc,p);
	}
#endif

	//清空所有的定时器事件，调用所有回调函数
	void ClearAllTimerEvent()
	{
		m_timer.ClearAllTimerEvent();
	}

	//使Timer有效
	void Enable()
	{
		m_timer.Enable();
	}

	//使Timer无效，在此状态下Timer不接收始终订阅，
	void Disable()
	{
		m_timer.Disable();
	}

	// 判断当前Timer是否处在可用状态
	bool IsEnable()
	{
		return m_timer.IsEnable();
	}

#ifdef _DEBUG
	/**
	*	打印出各个队列中存储的所有定时值
	*/
	void PrintList()
	{
		m_timer.PrintList();
	}
#endif

private:
	int timerCallbackProc(GMCustomTimerCallBackType type,void* prama)
	{
		Param* p = (Param*)prama;
		int r = (p->m_owner->*p->m_userFun)(type,p->m_prama);
		delete p;
		return r;
	}

private:
	GMCustomTimerEx<GMCustomTimer<TimerOwner> > m_timer;
};

#endif //GMCustomTimer_TEMPLATE_H_2008__COM_GM_
