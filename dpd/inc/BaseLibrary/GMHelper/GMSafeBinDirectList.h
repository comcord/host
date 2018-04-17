#ifndef SAFEBINDIRECTLIST_H_2006_CHANNELSOFT
#define SAFEBINDIRECTLIST_H_2006_CHANNELSOFT

#include <blist.h>

template<class Item>
class CMCSafeBinDirectList : public GMBlist<Item>
{
public:
	
	/*
	*	申请锁
	*/
	void requestLock()
	{
		m_lock.lock();
	}
	/*
	*	释放锁
	*/
	void releaseLock()
	{
		m_lock.unlock();
	}

public:
	/*
	 *	添加
	 */
	void safe_add( Item itm);
	/*
	 *	删除
	 */
	void safe_remove( Item itm);
	/*
	 *	清除所有元素
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
