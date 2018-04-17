#ifndef GMLOCKIMP_H_2008_GM_HQY__
#define GMLOCKIMP_H_2008_GM_HQY__

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

class GMCSLock
{
public:
	GMCSLock();

	~GMCSLock();

	void lock();

	void unlock();

private:
#ifdef WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t  cs;
	pthread_mutex_t  thislock;
	pthread_t m_pLockThread;
	int m_nLockTime;
#endif
};

#endif
