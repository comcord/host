#ifndef GMLOCK_H_2006_CHANNELSOFT
#define GMLOCK_H_2006_CHANNELSOFT

#ifdef WIN32
class GMWinRWLock;
#endif
class GMCSLock;


/*
*	 互斥锁，在VISTA和WIN2008上，采用系统的RWLock实现
*	，否则临界区实现，运行时检测
*/

class    GMLock
{
public:
	GMLock( bool bReAcquire = false );//bReAcquire表示是否同一线程内
									//，lock和unlock内是否能够够嵌套使用
	~GMLock();

	void lock();
	void unlock();

private:
#ifdef WIN32
	GMWinRWLock * m_pwinRWLock;
#endif
	GMCSLock * m_pCSLock;
};
#endif

