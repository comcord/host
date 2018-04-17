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
	 *	�����߳�
	 */
	void wakeup()
	{
		m_enent.signal();
	}
	
	/*
	 *	�߳�����ָ����ʱ��
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

