#ifndef GMLOCK_H_2006_CHANNELSOFT
#define GMLOCK_H_2006_CHANNELSOFT

#ifdef WIN32
class GMWinRWLock;
#endif
class GMCSLock;


/*
*	 ����������VISTA��WIN2008�ϣ�����ϵͳ��RWLockʵ��
*	�������ٽ���ʵ�֣�����ʱ���
*/

class    GMLock
{
public:
	GMLock( bool bReAcquire = false );//bReAcquire��ʾ�Ƿ�ͬһ�߳���
									//��lock��unlock���Ƿ��ܹ���Ƕ��ʹ��
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

