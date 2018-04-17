#ifndef SAFEBINDIRECTLIST_H_2006_CHANNELSOFT
#define SAFEBINDIRECTLIST_H_2006_CHANNELSOFT

#include <blist.h>

template<class Item>
class CMCSafeBinDirectList : public GMBlist<Item>
{
public:
	
	/*
	*	������
	*/
	void requestLock()
	{
		m_lock.lock();
	}
	/*
	*	�ͷ���
	*/
	void releaseLock()
	{
		m_lock.unlock();
	}

public:
	/*
	 *	���
	 */
	void safe_add( Item itm);
	/*
	 *	ɾ��
	 */
	void safe_remove( Item itm);
	/*
	 *	�������Ԫ��
	 */
	void safe_clear();


private:
	GMLock m_lock;
	
};

template<class Item>
void CMCSafeBinDirectList<Item>::safe_add(Item itm)
{
	requestLock();
	blist<Item>::add(itm);
	releaseLock();
}
template<class Item>
void CMCSafeBinDirectList<Item>::safe_clear()
{
	requestLock();
	blist<Item>::clear();
	releaseLock();
}
template<class Item>
void CMCSafeBinDirectList<Item>::safe_remove(Item itm)
{
	requestLock();
	blist<Item>::remove(itm);
	releaseLock();
}
#endif
