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


% Processor Time ָ����������ִ�з������߳�ʱ��İٷֱȡ�
���㷽���ǣ�������������ڷ������̻߳��ʱ�䣬�÷��������ȥ��ֵ��
(ÿ̨��������һ�������̣߳����߳���û�������߳̿�������ʱ��������)��
����������Ǵ����������Ҫ˵������
��ʾ�ڷ������ʱ���۲�ķ�æʱ��ƽ���ٷֱȡ�
���ֵ���� 100% ��ȥ�÷��񲻻��ʱ���������ġ�

*/