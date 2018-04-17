#ifndef GMSLEEPWITHWAKEUP_H_2006_CHANNELSOFT
#define GMSLEEPWITHWAKEUP_H_2006_CHANNELSOFT
#include "GMEvent.h"

/*
 *	
 */
class    GMSleepWithWakeup
{
public:
	/*
	 *	唤醒线程
	 */
	void wakeup()
	{
		m_enent.signal();
	}
	
	/*
	 *	线程休眠指定的时间
	 */
	void sleep( unsigned int nMillSec)
	{
		m_enent.wait(nMillSec);
		m_enent.reset();
	}

private:
	GMSysEvent m_enent;
};
#endif

