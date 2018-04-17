#ifndef _GMCPUPERFC_H
#define _GMCPUPERFC_H

#include <windows.h>
#include "BaseDataType.h"
class GMCpuPerfC
{
public:
	GMCpuPerfC();
	virtual ~GMCpuPerfC();

// Methods
	int GetCpuUsage();
	int GetCpuUsage(LPCTSTR pProcessName);
	int GetCpuUsage(DWORD dwProcessID);

	BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);

// Attributes
private:
	bool			m_bFirstTime;
	LONGLONG		m_lnOldValue ;
	LARGE_INTEGER	m_OldPerfTime100nSec;
};


#endif


/*
Understanding Multiple Processor Counters 
Some performance counters were designed for single processor systems and might
not be accurate for multiprocessor computers. For example, a process is limited 
to 100 percent of a single processor; however, its threads can use several 
processors, totaling more than 100 percent.

The "\Processor(_Total)\% Processor Time" counter value is the average usage of
all processors. For example, if you have two processors, one at 100 percent and 
another at 0 percent, this counter will report 50 percent. So the range is from 
0 through 100.
The "\Process(X)\% Process Time" counter value is the sum of the processor usage
by all threads of process X. For example, in a computer with two processors, if 
a process has two threads, one taking up 75 percent of a CPU and the other 
taking 80 percent of another CPU, this counter will report 155 percent. The 
range for this counter is from 0 through 100 * ProcessorCount.


% Processor Time 指处理器用来执行非闲置线程时间的百分比。
计算方法是，测量范例间隔内非闲置线程活动的时间，用范例间隔减去该值。
(每台处理器有一个闲置线程，该线程在没有其他线程可以运行时消耗周期)。
这个计数器是处理器活动的主要说明器，
显示在范例间隔时所观察的繁忙时间平均百分比。
这个值是用 100% 减去该服务不活动的时间计算出来的。

*/