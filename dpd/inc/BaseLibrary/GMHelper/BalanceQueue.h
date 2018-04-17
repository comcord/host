#pragma once
#include "FQueue.h"

template<typename T>
class BalanceQueue
{
	typedef FQueue<T> ItemQueue;

public:
	bool init(U32 queue_num,U32 queue_size);
	
	void uninit();

	bool push_back(const T& req)
	{
		return push(req,true);
	}

	bool pop_back(T& req)
	{
		return pop(req,true);
	}
	
	bool push_front(const T& req)
	{
		return push(req,false);
	}

	bool pop_front(T& req)
	{
		return pop(req,false);
	}

	bool empty()
	{
		return m_currentNum == 0;
	}

private:
	bool push(const T& req,bool bBack);
	bool pop(T& req,bool bFront);

	ItemQueue* m_ItemQueues;
	GMLock* m_ItemQueueLocks;
	U32 m_pushIndex;
	U32 m_popIndex;
	U32 m_queue_num;
	U32 m_currentNum;//当前总的元素个数
};

template<typename T>
bool BalanceQueue<T>::init(U32 queue_num,U32 queue_size)
{
	if (queue_num == 0 || queue_size == 0)
		return false;

	m_queue_num = queue_num;
	m_currentNum = 0;
	m_pushIndex = 0;
	m_popIndex  = 0;
	do 
	{
		m_ItemQueues = new ItemQueue[m_queue_num];
		if (NULL == m_ItemQueues)break;

		for (U32 i=0;i<m_queue_num;++i)
			if(!m_ItemQueues[i].init(queue_size))break;

		m_ItemQueueLocks = new GMLock[m_queue_num];
		if (NULL == m_ItemQueueLocks)break;
		
		return true;

	} while (false);

	uninit();

	return false;
}

template<typename T>
void BalanceQueue<T>::uninit()
{
	if (NULL != m_ItemQueues)
	{
		for (U32 i=0;i<m_queue_num;++i)
			m_ItemQueues[i].uninit();

		delete[] m_ItemQueues;
		m_ItemQueues = NULL;
	}

	if (NULL != m_ItemQueueLocks)
	{
		delete[] m_ItemQueueLocks;
		m_ItemQueueLocks = NULL;
	}
}

template<typename T>
bool BalanceQueue<T>::push(const T& req,bool bBack)
{
	if(NULL == m_ItemQueues)return false;
	U32 index = InterlockedIncrement(&m_pushIndex);
	index %= m_queue_num;

	bool ret = false;
	m_ItemQueueLocks[index].lock();
	if (bBack)
		ret = m_ItemQueues[index].push_back(req);
	else
		ret = m_ItemQueues[index].push_front(req);
	
	//必须在锁里，否则无法保证正确性。如果在锁外面。放入后，还没执行加，另外一个线程执行pop，可能导致m_currentNum先执行减。
	if (ret)InterlockedIncrement(&m_currentNum);

	m_ItemQueueLocks[index].unlock();
	
	return ret;
}

template<typename T>
bool BalanceQueue<T>::pop(T& req,bool bBack)
{
	if(NULL == m_ItemQueues)return false;

	U32 index = InterlockedIncrement(&m_popIndex);
	index %= m_queue_num;

	bool haveGotten = false;
	m_ItemQueueLocks[index].lock();
	if(bBack)
		haveGotten = m_ItemQueues[index].pop_back(req);
	else
		haveGotten = m_ItemQueues[index].pop_front(req);

	if(haveGotten)InterlockedDecrement(&m_currentNum);//必须在锁里，否则无法保证正确性

	m_ItemQueueLocks[index].unlock();

	if (haveGotten)return true;

	//从其他队列中找，最多找一圈
	for (U32 i=1;i<m_queue_num;++i)
	{
		index=(index+i)%m_queue_num;

		m_ItemQueueLocks[index].lock();

		if(bBack)
			haveGotten = m_ItemQueues[index].pop_back(req);
		else
			haveGotten = m_ItemQueues[index].pop_front(req);

		if(haveGotten)
		{
			InterlockedDecrement(&m_currentNum);//必须在锁里，否则无法保证正确性
			InterlockedExchange(&m_popIndex,index);
			m_ItemQueueLocks[index].unlock();
			break;
		}

		m_ItemQueueLocks[index].unlock();
	}

	return haveGotten;
}
