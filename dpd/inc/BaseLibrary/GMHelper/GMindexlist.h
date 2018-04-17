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
 *	功能描述： 带索引的LIST，能够快速根据索引定位到具体的数据
    ，key允许重复，索引表采用multimap实现,实现了STL list的部分接口
	iterator是指向list_item的跌代器，
 * 
 */



template <class Index, class Item
		, template<class>  class GMMemAllocMethod = GMListMemAllocMethod>
class GMIndexlist 
{

public:
	struct list_item//实体表中的每个表项
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
		Item					  item;//LIST存放的元素
		typename list_item_map::iterator mapit;//指向MAP中的跌代器
		/*list_item & operator=(const list_item &src)
		{
			item = src.item;
			mapit = src.mapit;
			return *this;            
		}*/
	};
	
	typedef typename GMBlist<list_item, GMMemAllocMethod>::iterator list_iterator;//list跌代器声明
	typedef list_iterator iterator;//index_list 的迭代器
	typedef typename list_item::list_item_map::iterator map_iterator;//map iterator
	typedef GMBlist<list_item, GMMemAllocMethod> entry_table;//实体表，存放元素的LIST
	typedef typename list_item::list_item_map index_table;// 索引表，用MAP实现

public:
	//返回开始位置的迭代器
	iterator begin(){return m_list.begin();}
	
	//返回最后位置的迭代器，为NULL
	iterator end(){return m_list.end();}
	
	//带INDEX方式 后插入值，
	void push_back(const Index& key, const Item &itm)
	{
		m_list.push_back(list_item(m_map.end(), itm));
		m_list.back().mapit
			= m_map.insert(m_map.end()
				,index_table::value_type(key,m_list.last()));
		assert(m_map.size() == m_list.size());
	}

	//带INDEX 前插入值
	void push_front(const Index& key, const Item &itm)	
	{
		m_list.push_front(list_item(m_map.end(), itm));
		m_list.front().mapit
			= m_map.insert(m_map.end(),
				index_table::value_type(key,m_list.first()));
		assert(m_map.size() == m_list.size());
	}

	//在开始位置弹出值
	void pop_front()
	{
		m_map.erase(m_list.front().mapit);
		m_list.pop_front();
		assert(m_map.size() == m_list.size());
	}

	//在最后位置弹出值
	void pop_back()
	{
		m_map.erase(m_list.back().mapit);
		m_list.pop_back();
		assert(m_map.size() == m_list.size());
	}
	
	//返回元素数量
	size_t size()
	{
		assert(m_map.size() == m_list.size());
		return m_list.size();
	}

	//是否为空
	bool empty()
	{
		assert(m_map.size() == m_list.size());
		return m_list.empty();
	}

	//清除所有元素
	void clear()
	{
		assert(m_map.size() == m_list.size());
		m_map.clear();
		m_list.clear();
	}

	//返回最前个节点的迭代器
	iterator first()
	{
		return m_list.first();
	}
	//返回最后个节点的迭代器
	iterator last()
	{
		return m_list.last();
	}

	//返回最前节点的值
	Item & front()
	{
		return m_list.front().item;
	}

	//返回最后节点的值
	Item & back()
	{
		return m_list.back().item;
	}

	//在指定的迭代器位置插入带INDEX的元素
	iterator insert(iterator& it, const Index& key,  const Item &itm)
	{
		iterator rtn =  m_list.insert(it, list_item(m_map.end(), itm));
		(*rtn).mapit
			= m_map.insert(m_map.end(),index_table::value_type(key,rtn));
		assert(m_map.size() == m_list.size());
		return rtn;
	}

	//根据索引快速查找到位置，返回迭代器
	iterator find(const Index &key)
	{
		assert(m_map.size() == m_list.size());
		map_iterator it = m_map.find(key);
		if(it != m_map.end())
			return (*it).second;
		else 
			return end();
	}

	//删除指定跌代器位置的元素，返回跌代器位置的下一个位置（跌代器）
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

	//根据索引删除，返回是否删除成功
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

	//获得LIST指针
	entry_table * get_entry_table_ptr(){return &m_list;}
	//获得索引表指针
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
//	struct list_item//实体表中的每个表项
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
//		Item					  item;//LIST存放的元素
//		typename list_item_map::iterator mapit;//指向MAP中的跌代器
//		/*list_item & operator=(const list_item &src)
//		{
//			item = src.item;
//			mapit = src.mapit;
//			return *this;            
//		}*/
//	};
//	
//	typedef typename GMBlist<list_item, GMMemAllocMethod>::iterator list_iterator;//list跌代器声明
//	typedef list_iterator iterator;//index_list 的迭代器
//	typedef typename list_item::list_item_map::iterator map_iterator;//map iterator
//	typedef GMBlist<list_item, GMMemAllocMethod> entry_table;//实体表，存放元素的LIST
//	typedef typename list_item::list_item_map index_table;// 索引表，用MAP实现
//
//public:
//	//返回开始位置的迭代器
//	iterator begin(){return m_list.begin();}
//	
//	//返回最后位置的迭代器，为NULL
//	iterator end(){return m_list.end();}
//	
//	//带INDEX方式 后插入值，
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
//	//带INDEX 前插入值
//	void push_front(const Index& key, const Item &itm)	
//	{
//		m_list.push_front(list_item(m_map.end(), itm));
//		m_list.front().mapit
//			= m_map.insert(m_map.end(),
//				index_table::value_type(key,m_list.first()));
//		assert(m_map.size() == m_list.size());
//	}
//
//	//在开始位置弹出值
//	void pop_front()
//	{
//		m_map.erase(m_list.front().mapit);
//		m_list.pop_front();
//		assert(m_map.size() == m_list.size());
//	}
//
//	//在最后位置弹出值
//	void pop_back()
//	{
//		m_map.erase(m_list.back().mapit);
//		m_list.pop_back();
//		assert(m_map.size() == m_list.size());
//	}
//	
//	//返回元素数量
//	size_t size()
//	{
//		assert(m_map.size() == m_list.size());
//		return m_list.size();
//	}
//
//	//是否为空
//	bool empty()
//	{
//		assert(m_map.size() == m_list.size());
//		return m_list.empty();
//	}
//
//	//清除所有元素
//	void clear()
//	{
//		assert(m_map.size() == m_list.size());
//		m_map.clear();
//		m_list.clear();
//	}
//
//	//返回最前个节点的迭代器
//	iterator first()
//	{
//		return m_list.first();
//	}
//	//返回最后个节点的迭代器
//	iterator last()
//	{
//		return m_list.last();
//	}
//
//	//返回最前节点的值
//	Item & front()
//	{
//		return m_list.front().item;
//	}
//
//	//返回最后节点的值
//	Item & back()
//	{
//		return m_list.back().item;
//	}
//
//	//在指定的迭代器位置插入带INDEX的元素
//	iterator insert(iterator& it, const Index& key,  const Item &itm)
//	{
//		iterator rtn =  m_list.insert(it, list_item(m_map.end(), itm));
//		(*rtn).mapit
//			= m_map.insert(m_map.end(),index_table::value_type(key,rtn));
//		assert(m_map.size() == m_list.size());
//		return rtn;
//	}
//
//	//根据索引快速查找到位置，返回迭代器
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
//	//删除指定跌代器位置的元素，返回跌代器位置的下一个位置（跌代器）
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
//	//根据索引删除，返回是否删除成功
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
//	//获得LIST指针
//	entry_table * get_entry_table_ptr(){return &m_list;}
//	//获得索引表指针
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
