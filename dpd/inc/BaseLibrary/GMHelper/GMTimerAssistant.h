#ifndef GMTimerAssistant_H_2008__COM_GM_
#define GMTimerAssistant_H_2008__COM_GM_

#include "BaseDataType.h"

#ifndef WIN32		// linux��ʹ��
#include <sys/time.h>

class GMlockInterval
{
public:
	GMlockInterval();
	unsigned int GetTickInterval();
private:
	timeval m_startimeval;
};

#endif // linux��ʹ��



class GMTimerAssistant
{
public:
	
	/**
	 *	��ȡ����1970��1��1��00:00:00 UTC ��ʼ�����ھ������ĺ��룬��λ������
	 */

	static U64 GetSysCurrentTime();
	


	static void SetSysTime(U64 currentTime);


	/* �߾��� ��λ�� */
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

