#pragma once

#include "BaseDataType.h"
#include "GMSleepWithWakeup.h"
#include "GMRWLock.h"
#include "GMCircleQueue.h"
#include "GMThread.h"
#include <vector>

#ifdef WIN32
#ifdef X64
#include "GMCpuPerfC.h"
#endif
#endif

/*
period_width :������ ��λ����
period_num   :�������

������Ի�ȡ �൱ǰn�����ڵ�cpuƽ��ʹ����.���� n = period_width*m; m=1,...,period_num
*/
class  GMCpuUsage
{
#ifdef WIN32
#ifndef X64
	typedef struct
	{
		DWORD dwUnknown1;
		ULONG uKeMaximumIncrement;
		ULONG uPageSize;
		ULONG uMmNumberOfPhysicalPages;
		ULONG uMmLowestPhysicalPage;
		ULONG uMmHighestPhysicalPage;
		ULONG uAllocationGranularity;
		PVOID pLowestUserAddress;
		PVOID pMmHighestUserAddress;
		ULONG uKeActiveProcessors;
		BYTE bKeNumberProcessors;
		BYTE bUnknown2;
		WORD wUnknown3;
	} SYSTEM_BASIC_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liIdleTime;
		DWORD dwSpare[312];
	} SYSTEM_PERFORMANCE_INFORMATION;

	typedef struct
	{
		LARGE_INTEGER liKeBootTime;
		LARGE_INTEGER liKeSystemTime;
		LARGE_INTEGER liExpTimeZoneBias;
		ULONG uCurrentTimeZoneId;
		DWORD dwReserved;
	} SYSTEM_TIME_INFORMATION;
#endif
#endif

public:

	/*
	period_width :������ ��λ����
	period_num   :�������

	������Ի�ȡ �൱ǰn�����ڵ�cpuƽ��ʹ����.���� n = period_width*m; m=1,...,period_num
	*/
	GMCpuUsage(U32 period_width,U32 period_num);

	~GMCpuUsage();

	/*
	brief:
		ȡ�����period���cpuƽ��ʹ����
	params:
		period
			[in] ���䣬
	note:
		period�ᱻȡ�����������ֻ��Ϊperiod_width*period_num
	*/
	U32 getCpuUsage(U32 period);               

private:

	void workProc(void *);
	
private:
	//�������ݽṹ
	U32 m_nCpuUsage;

	GMSleepWithWakeup m_sleeper;

	U32 m_period_width;
	U32 m_period_num;  
	GMRWLock m_cpuUsagesLock;
	GMCircleQueue<U32> m_cpuUsages;
	GMDaemonThread<GMCpuUsage> m_workThread;
	
#ifdef WIN32

#ifdef X64
	GMCpuPerfC m_x64Counter;
#else
	//���Ͷ���
	typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);

	//��������
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
	SYSTEM_TIME_INFORMATION SysTimeInfo;
	SYSTEM_BASIC_INFORMATION SysBaseInfo;

	double dbIdleTime;
	double dbSystemTime;

	LONG m_status;
	LARGE_INTEGER liOldIdleTime;
	LARGE_INTEGER liOldSystemTime;

	PROCNTQSI NtQuerySystemInformation;

#endif

#endif
	
	U64 m_lastSystemTime;
	U64 m_lastIdleTime;

#ifndef WIN32

#endif

};
