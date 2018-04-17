#ifndef GMRW_CBINLOCK_H_2006_CHANNELSOFT
#define GMRW_CBINLOCK_H_2006_CHANNELSOFT
class GMJRRWLock;
#ifdef WIN32
class GMWinRWLock;
#endif


/*
 *	 ��д������VISTA��WIN2008�ϣ�����ϵͳ��RWLockʵ��
 *	���������WIN32���ı�̡��е�ʮ�¹��ڶ�д����ʾ��
	,����ʱ���
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

