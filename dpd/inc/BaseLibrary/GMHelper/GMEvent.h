#ifndef EVENT_H_2010_CHANNELSOFT_GMHELPER
#define EVENT_H_2010_CHANNELSOFT_GMHELPER
#include "SysDiffDefine.h"
#ifdef WIN32
#include <windows.h>

/*
 *	系统信号
 */
class  GMSysEvent
{
public:
	GMSysEvent( bool autoRest = false, bool hasSignal =false);
	~GMSysEvent();
	
	bool signal();
	bool reset();
	//成功返回1  失败=0  超时= -1
	int	wait(unsigned int timeout = INFINITE);
	HANDLE getHandle(){return event;}
private:

	HANDLE event;
};



#else
/*
 *	LINUX IMP
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <errno.h>

	class   GMSysEvent
	{
	public:
		typedef struct 
		{
			pthread_mutex_t	mutex;
			pthread_cond_t	condition;
			volatile int	semCount;
		}  event_private_struct;

		GMSysEvent( bool autoRest =false, bool hasSignal =false);
		~GMSysEvent();

		bool signal();
		bool reset();
		//成功返回1  失败=0  超时= -1 timeout单位：毫秒
		int	wait(unsigned int timeout = INFINITE);
		
		event_private_struct* getHandle(){return &token;}
	private:
		bool m_autoRest;
		event_private_struct  token;

		//timeout 时间毫秒 
		bool getTimeoutPointer(unsigned int timeout
			, timespec &tsp, bool &waitInFinite );
	};


#endif //end linx imp

#endif//end file
