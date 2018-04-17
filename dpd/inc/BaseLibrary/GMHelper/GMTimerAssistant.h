#ifndef GMTimerAssistant_H_2008__COM_GM_
#define GMTimerAssistant_H_2008__COM_GM_

#include "BaseDataType.h"

#ifndef WIN32		// linux下使用
#include <sys/time.h>

class GMlockInterval
{
public:
	GMlockInterval();
	unsigned int GetTickInterval();
private:
	timeval m_startimeval;
};

#endif // linux下使用



class GMTimerAssistant
{
public:
	
	/**
	 *	获取本从1970年1月1日00:00:00 UTC 开始到现在经历过的豪秒，单位：豪秒
	 */

	static U64 GetSysCurrentTime();
	


	static void SetSysTime(U64 currentTime);


	/* 高精度 单位秒 */
	static void Init();
	static double GetSystemTimeD();
	
private:
		static long long m_TimeRegulator;

#ifdef WIN32
		static double m_SecondsPerTick;//WIN32
#else // linux
		static GMlockInterval s_ticketer;
#endif
	  	static U64 m_TuningTime;
};



#endif //GMTimerAssistant_H_2008__COM_GM_

