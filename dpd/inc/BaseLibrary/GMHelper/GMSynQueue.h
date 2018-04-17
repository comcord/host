#ifndef _SYN_QUEUE_H_TEMPLATE_2008_GM
#define _SYN_QUEUE_H_TEMPLATE_2008_GM
#include "GMBlist.h"
#include "GMLock.h"
#include "GMEvent.h"
#include "GMMemAlloc.h"
#include "GMautolock.h"
#include "FQueue.h"
/*
*	ͬ�����У��̰߳�ȫ����д����������ŵ����е�β�����������Ӷ���ͷ����
*/
template<class T,template<class> class GMMemAllocMothed = GMListMemAllocMethod>
class GMSynQueue
{
public:
	enum OPERAT_DIRECTION//�����Ķ��еķ���
	{
		OD_HEADER,//ͷ������
		OD_TAIL	//β�Ͳ���
	};

	GMSynQueue(void):m_bCancel(false),m_event(false){}

	~GMSynQueue(void){m_list.clear();}

	//	�����ȴ������� 0 ���пɶ��� , 1 TIMEOUT, -1 ȡ�����ȴ�
	int waitForRead( unsigned int micsec )
	{
		int ret = 0 ;
		if(m_bCancel) return -1;		
		ret = m_event.wait( micsec ) ;

		if ( ret == 1 )
			return m_bCancel?-1: 0 ;
		else
			return m_bCancel?-1: 1 ;
	}
	//ȡ���ȴ���
	void cancelWait()
	{
		GMAutoLock<GMLock> al(&m_lock);
		m_bCancel = true;
		m_event.signal();
	}

	//������д������
	bool write_nosafe(const T& obj, OPERAT_DIRECTION od)
	{
		if(m_bCancel)return false;

		if (m_list.empty())m_event.signal();

		if(od == OD_TAIL)
			m_list.push_back(obj);
		else
			m_list.push_front(obj);

		return true;
	}

	bool multi_write_nosafe(FQueue<T>& o, OPERAT_DIRECTION od)
	{
		if(m_bCancel)
			return false;

		if (m_list.empty())
			m_event.signal();
		
		T v;
		while(o.pop_front(v))
		{
			if(od == OD_TAIL)
				m_list.push_back(v);
			else 
				m_list.push_front(v);
		}

		return true;
	}

	bool write(const T& obj, OPERAT_DIRECTION od)
	{
		GMAutoLock<GMLock> al(&m_lock);
		return write_nosafe(obj,od);
	}

	bool read(T & o,  OPERAT_DIRECTION od)
	{
		GMAutoLock<GMLock> al(&m_lock);
		return  read_nosafe(o,  od);
	}

	bool multi_write(FQueue<T>& o,OPERAT_DIRECTION od)
	{
		GMAutoLock<GMLock> al(&m_lock);
		return multi_write_nosafe(o,od);
	}

	bool multi_read(FQueue<T>& o, OPERAT_DIRECTION od)
	{
		GMAutoLock<GMLock> al(&m_lock);
		return  multi_read_nosafe(o,od);
	}

	//�Ӷ��ж������� Ĭ�ϴ�ͷ��ʼ�����ɹ�����ture,ʧ�ܷ���false
	bool read_nosafe(T & o,  OPERAT_DIRECTION od)
	{
		if(m_bCancel)return false;

		if(m_list.empty())
		{
			m_event.reset();
			return false;
		}

		if(od == OD_HEADER)
		{
			o = m_list.front();
			m_list.pop_front();
		}
		else
		{
			o = m_list.back();
			m_list.pop_back();
		}

		if(m_list.empty())m_event.reset();

		return true;
	}

	U32 multi_read_nosafe(FQueue<T>& o, OPERAT_DIRECTION od)
	{
		if(m_bCancel)return false;

		U32 get_num = gmmin(o.capacity(),m_list.size());
		for (U32 i=0;i<get_num;++i)
		{
			if(od == OD_HEADER)
			{
				o.push_back(m_list.front());
				m_list.pop_front();
			}
			else 
			{
				o.push_back(m_list.back());
				m_list.pop_back();
			}
		}

		if(m_list.empty())m_event.reset();

		return get_num;
	}

	unsigned long size()
	{
		GMAutoLock<GMLock> al(&m_lock);
		return static_cast<unsigned long>(m_list.size());
	}	

protected:
	GMLock m_lock;
	GMSysEvent m_event;
	volatile bool m_bCancel;
	GMBlist<T, GMMemAllocMothed> m_list;
};

#endif
