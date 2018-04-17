#ifndef GMRW_CBINLOCK_H_2006_CHANNELSOFT
#define GMRW_CBINLOCK_H_2006_CHANNELSOFT
class GMJRRWLock;
#ifdef WIN32
class GMWinRWLock;
#endif


/*
 *	 读写锁，在VISTA和WIN2008上，采用系统的RWLock实现
 *	，否则采用WIN32核心编程》中第十章关于读写锁的示例
	,运行时检测
 */

class    GMRWLock
{

public:
	GMRWLock();
	~GMRWLock();
	void readLock();
	void writeLock();
	void unReadLock();
	void unWriteLock();

private:
	GMJRRWLock * m_pjrRWLock;
#ifdef WIN32
	GMWinRWLock * m_pwinRWLock;
#endif

};

#endif
//////////////////////////////// End of File //////////////////////////////////

