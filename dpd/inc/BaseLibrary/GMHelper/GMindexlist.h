#ifndef INDEX_GM_LIST_H_2008_GOUMANG_KYG
#define INDEX_GM_LIST_H_2008_GOUMANG_KYG
#include <map>
#include <functional>
#include "GMMemAlloc.h"
#include "GMBlist.h"
#include <assert.h>
#include "GMWidgetAlloctor.h"
#include <utility>


/*
 *	���������� ��������LIST���ܹ����ٸ���������λ�����������
    ��key�����ظ������������multimapʵ��,ʵ����STL list�Ĳ��ֽӿ�
	iterator��ָ��list_item�ĵ�������
 * 
 */



template <class Index, class Item
		, template<class>  class GMMemAllocMethod = GMListMemAllocMethod>
class GMIndexlist 
{

public:
	struct list_item//ʵ����е�ÿ������
	{
	typedef	typename std::multimap<Index
			, typename GMBlist<list_item, GMMemAllocMethod>::iterator
			, std::less<Index>
			, GMWidgetAlloctor<std::pair<Index, typename GMBlist<list_item, GMMemAllocMethod>::iterator> , GMMemAllocMethod> 
	> list_item_map;

		list_item()
		{}
		list_item(
			typename list_item_map::iterator &itm
			, const Item& im)
			:mapit(itm), item(im){}
		Item					  item;//LIST��ŵ�Ԫ��
		typename list_item_map::iterator mapit;//ָ��MAP�еĵ�����
		/*list_item & operator=(const list_item &src)
		{
			item = src.item;
			mapit = src.mapit;
			return *this;            
		}*/
	};
	
	typedef typename GMBlist<list_item, GMMemAllocMethod>::iterator list_iterator;//list����������
	typedef list_iterator iterator;//index_list �ĵ�����
	typedef typename list_item::list_item_map::iterator map_iterator;//map iterator
	typedef GMBlist<list_item, GMMemAllocMethod> entry_table;//ʵ������Ԫ�ص�LIST
	typedef typename list_item::list_item_map index_table;// ��������MAPʵ��

public:
	//���ؿ�ʼλ�õĵ�����
	iterator begin(){return m_list.begin();}
	
	//�������λ�õĵ�������ΪNULL
	iterator end(){return m_list.end();}
	
	//��INDEX��ʽ �����ֵ��
	void push_back(const Index& key, const Item &itm)
	{
		m_list.push_back(list_item(m_map.end(), itm));
		m_list.back().mapit
			= m_map.insert(m_map.end()
				,index_table::value_type(key,m_list.last()));
		assert(m_map.size() == m_list.size());
	}

	//��INDEX ǰ����ֵ
	void push_front(const Index& key, const Item &itm)	
	{
		m_list.push_front(list_item(m_map.end(), itm));
		m_list.front().mapit
			= m_map.insert(m_map.end(),
				index_table::value_type(key,m_list.first()));
		assert(m_map.size() == m_list.size());
	}

	//�ڿ�ʼλ�õ���ֵ
	void pop_front()
	{
		m_map.erase(m_list.front().mapit);
		m_list.pop_front();
		assert(m_map.size() == m_list.size());
	}

	//�����λ�õ���ֵ
	void pop_back()
	{
		m_map.erase(m_list.back().mapit);
		m_list.pop_back();
		assert(m_map.size() == m_list.size());
	}
	
	//����Ԫ������
	size_t size()
	{
		assert(m_map.size() == m_list.size());
		return m_list.size();
	}

	//�Ƿ�Ϊ��
	bool empty()
	{
		assert(m_map.size() == m_list.size());
		return m_list.empty();
	}

	//�������Ԫ��
	void clear()
	{
		assert(m_map.size() == m_list.size());
		m_map.clear();
		m_list.clear();
	}

	//������ǰ���ڵ�ĵ�����
	iterator first()
	{
		return m_list.first();
	}
	//���������ڵ�ĵ�����
	iterator last()
	{
		return m_list.last();
	}

	//������ǰ�ڵ��ֵ
	Item & front()
	{
		return m_list.front().item;
	}

	//�������ڵ��ֵ
	Item & back()
	{
		return m_list.back().item;
	}

	//��ָ���ĵ�����λ�ò����INDEX��Ԫ��
	iterator insert(iterator& it, const Index& key,  const Item &itm)
	{
		iterator rtn =  m_list.insert(it, list_item(m_map.end(), itm));
		(*rtn).mapit
			= m_map.insert(m_map.end(),index_table::value_type(key,rtn));
		assert(m_map.size() == m_list.size());
		return rtn;
	}

	//�����������ٲ��ҵ�λ�ã����ص�����
	iterator find(const Index &key)
	{
		assert(m_map.size() == m_list.size());
		map_iterator it = m_map.find(key);
		if(it != m_map.end())
			return (*it).second;
		else 
			return end();
	}

	//ɾ��ָ��������λ�õ�Ԫ�أ����ص�����λ�õ���һ��λ�ã���������
	iterator erase(iterator & lit)
	{
		iterator rtn = end();
		map_iterator it = m_map.end();
		if(lit != m_list.end())
		{
			it = (*lit).mapit;
		}
		if(it != m_map.end())
		{
			rtn = lit;
			++rtn;
			assert((*it).second == lit);
			m_list.erase(lit);
			m_map.erase(it);
			assert(m_map.size() == m_list.size());
			return rtn;
		}
		else
		{
			return end();
		}
	}

	//��������ɾ���������Ƿ�ɾ���ɹ�
	bool erase(const Index & key)
	{
		bool rtn = false;
		map_iterator it = m_map.find(key);
		if(it != m_map.end())
		{
			rtn = true;			
			m_list.erase((*it).second);
			m_map.erase(it);
			assert(m_map.size() == m_list.size());
			return rtn;
		}
		else
		{
			return false;
		}		
	}

	//���LISTָ��
	entry_table * get_entry_table_ptr(){return &m_list;}
	//���������ָ��
	index_table * get_index_table_ptr(){return &m_map;}


#ifndef gmindexlist_TEST
	protected:
#endif
	entry_table m_list;
	index_table m_map;
};


//template <class Index, class Item
//		, template<class>  class GMMemAllocMethod = GMListMemAllocMethod>
//class GMIndexlist 
//{
//
//public:
//	struct list_item//ʵ����е�ÿ������
//	{
//	typedef std::multimap<Index								// key
//			, typename GMBlist<list_item, GMMemAllocMethod>::iterator		// value
//			, std::less<Index>							// compare
//			, GMWidgetAlloctor<std::pair<Index, typename GMBlist<list_item, GMMemAllocMethod>::iterator>, GMMemAllocMethod> // allocator
//		> list_item_map;
//
//		list_item()
//		{}
//		list_item(
//			typename list_item_map::iterator &itm
//			, const Item& im)
//			:mapit(itm), item(im){}
//		Item					  item;//LIST��ŵ�Ԫ��
//		typename list_item_map::iterator mapit;//ָ��MAP�еĵ�����
//		/*list_item & operator=(const list_item &src)
//		{
//			item = src.item;
//			mapit = src.mapit;
//			return *this;            
//		}*/
//	};
//	
//	typedef typename GMBlist<list_item, GMMemAllocMethod>::iterator list_iterator;//list����������
//	typedef list_iterator iterator;//index_list �ĵ�����
//	typedef typename list_item::list_item_map::iterator map_iterator;//map iterator
//	typedef GMBlist<list_item, GMMemAllocMethod> entry_table;//ʵ������Ԫ�ص�LIST
//	typedef typename list_item::list_item_map index_table;// ��������MAPʵ��
//
//public:
//	//���ؿ�ʼλ�õĵ�����
//	iterator begin(){return m_list.begin();}
//	
//	//�������λ�õĵ�������ΪNULL
//	iterator end(){return m_list.end();}
//	
//	//��INDEX��ʽ �����ֵ��
//	void push_back(const Index& key, const Item &itm)
//	{
//
//		list_item(m_map.end(), itm);
//		/*m_list.push_back();
//		m_list.back().mapit
//			= m_map.insert(m_map.end()
//				,index_table::value_type(key,m_list.last()));
//		assert(m_map.size() == m_list.size());*/
//	}
//
//	//��INDEX ǰ����ֵ
//	void push_front(const Index& key, const Item &itm)	
//	{
//		m_list.push_front(list_item(m_map.end(), itm));
//		m_list.front().mapit
//			= m_map.insert(m_map.end(),
//				index_table::value_type(key,m_list.first()));
//		assert(m_map.size() == m_list.size());
//	}
//
//	//�ڿ�ʼλ�õ���ֵ
//	void pop_front()
//	{
//		m_map.erase(m_list.front().mapit);
//		m_list.pop_front();
//		assert(m_map.size() == m_list.size());
//	}
//
//	//�����λ�õ���ֵ
//	void pop_back()
//	{
//		m_map.erase(m_list.back().mapit);
//		m_list.pop_back();
//		assert(m_map.size() == m_list.size());
//	}
//	
//	//����Ԫ������
//	size_t size()
//	{
//		assert(m_map.size() == m_list.size());
//		return m_list.size();
//	}
//
//	//�Ƿ�Ϊ��
//	bool empty()
//	{
//		assert(m_map.size() == m_list.size());
//		return m_list.empty();
//	}
//
//	//�������Ԫ��
//	void clear()
//	{
//		assert(m_map.size() == m_list.size());
//		m_map.clear();
//		m_list.clear();
//	}
//
//	//������ǰ���ڵ�ĵ�����
//	iterator first()
//	{
//		return m_list.first();
//	}
//	//���������ڵ�ĵ�����
//	iterator last()
//	{
//		return m_list.last();
//	}
//
//	//������ǰ�ڵ��ֵ
//	Item & front()
//	{
//		return m_list.front().item;
//	}
//
//	//�������ڵ��ֵ
//	Item & back()
//	{
//		return m_list.back().item;
//	}
//
//	//��ָ���ĵ�����λ�ò����INDEX��Ԫ��
//	iterator insert(iterator& it, const Index& key,  const Item &itm)
//	{
//		iterator rtn =  m_list.insert(it, list_item(m_map.end(), itm));
//		(*rtn).mapit
//			= m_map.insert(m_map.end(),index_table::value_type(key,rtn));
//		assert(m_map.size() == m_list.size());
//		return rtn;
//	}
//
//	//�����������ٲ��ҵ�λ�ã����ص�����
//	iterator find(const Index &key)
//	{
//		assert(m_map.size() == m_list.size());
//		map_iterator it = m_map.find(key);
//		if(it != m_map.end())
//			return (*it).second;
//		else 
//			return end();
//	}
//
//	//ɾ��ָ��������λ�õ�Ԫ�أ����ص�����λ�õ���һ��λ�ã���������
//	iterator erase(iterator & lit)
//	{
//		iterator rtn = end();
//		map_iterator it = m_map.end();
//		if(lit != m_list.end())
//		{
//			it = (*lit).mapit;
//		}
//		if(it != m_map.end())
//		{
//			rtn = lit;
//			++rtn;
//			assert((*it).second == lit);
//			m_list.erase(lit);
//			m_map.erase(it);
//			assert(m_map.size() == m_list.size());
//			return rtn;
//		}
//		else
//		{
//			return end();
//		}
//	}
//
//	//��������ɾ���������Ƿ�ɾ���ɹ�
//	bool erase(const Index & key)
//	{
//		bool rtn = false;
//		map_iterator it = m_map.find(key);
//		if(it != m_map.end())
//		{
//			rtn = true;			
//			m_list.erase((*it).second);
//			m_map.erase(it);
//			assert(m_map.size() == m_list.size());
//			return rtn;
//		}
//		else
//		{
//			return false;
//		}		
//	}
//
//	//���LISTָ��
//	entry_table * get_entry_table_ptr(){return &m_list;}
//	//���������ָ��
//	index_table * get_index_table_ptr(){return &m_map;}
//
//
//#ifndef gmindexlist_TEST
//	protected:
//#endif
//	entry_table m_list;
//	index_table m_map;
//};


#endif
