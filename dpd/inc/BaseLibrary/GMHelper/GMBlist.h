#ifndef B_LIST_H_2009_9_1_10_41
#define B_LIST_H_2009_9_1_10_41
#include "GMMemAlloc.h"

template <class Item>
class GMBNode
{

protected:
	GMBNode()
		:pPrior(NULL), pNext(NULL){}
	~GMBNode(){}
public:
	GMBNode<Item> * pPrior;
	Item value;
	GMBNode<Item> * pNext;
};


/*
*	NodeITERATOR
*/
template<class Item>
class GMBNodeIterator
{
public:
	GMBNodeIterator()
		:m_pNode(NULL)
	{
	}
	GMBNodeIterator(GMBNode<Item> *pNode)
	{
		m_pNode = pNode;
	}
	~GMBNodeIterator()
	{
	}

	operator GMBNode<Item>*()//����ת��
	{
		return m_pNode;
	}
	/*
	*	ָ��
	*/
	Item & operator*()
	{
		return m_pNode->value;
	}
	GMBNode<Item> *operator->()
	{
		return m_pNode;
	}

	/*
	*	��ֵ
	*/
	GMBNodeIterator & operator=(GMBNode<Item> *pNode)
	{
		m_pNode = pNode;
		return *this;
	}
	GMBNodeIterator & operator=(GMBNodeIterator &src)
	{
		m_pNode = src.m_pNode;
		return *this;
	}
	/*
	*	���������
	*/

	GMBNodeIterator<Item> operator++()
	{
		if(m_pNode)
			m_pNode = m_pNode->pNext;

		return GMBNodeIterator<Item>(m_pNode);
	}

	const GMBNodeIterator<Item> operator++(int)
	{
		GMBNodeIterator<Item> old = *this;
		++(*this);
		return old;
	}


	GMBNodeIterator<Item> operator--()
	{
		if (m_pNode) 
			m_pNode = m_pNode->pPrior;
		return GMBNodeIterator<Item>(m_pNode);
	}
	const GMBNodeIterator<Item> operator--(int)
	{
		GMBNodeIterator<Item> old = *this;
		--(*this);
		return old;
	}

	/*
	*	�ȽϷ���
	*/
	bool operator==(GMBNode<Item> *pNode)
	{
		return (m_pNode == pNode);
	}
	bool operator==(GMBNodeIterator<Item> src)
	{
		return (m_pNode == src.m_pNode);
	}
	bool operator!=(GMBNode<Item> *pNode)
	{
		return (m_pNode != pNode);
	}
	bool operator!=(GMBNodeIterator<Item> src)
	{
		return (m_pNode != src.m_pNode);
	}
	


private:
	GMBNode<Item> * m_pNode;
};

/*
*	˫������
*/
//list	
template <class Item
	, template<class> class GMMemAllocMethod = GMGlobalHeapMemAllocMethod>
class GMBlist  
{
public:
	typedef GMBNodeIterator<Item> iterator;
	class SNode :public GMBNode<Item>
						,public GMMemAlloc<SNode, GMMemAllocMethod>
	{
	} ;


	GMBlist()
		:m_pFirstNode(NULL), m_pLastNode(NULL), m_nSize(0)
	{
	}
	~GMBlist()
	{
		if(!empty()) 
			clear();
	}

	/*
	*	���
	*/
	void add( const Item &itm)
	{
		push_back(itm);
	}
	/*
	*	ɾ��
	*/
	void remove(const Item &itm);
	GMBNodeIterator<Item> erase(GMBNodeIterator<Item>);

	/*
	*	��ô�С
	*/
	size_t size();
	/*
	*	�Ƿ�Ϊ��
	*/
	bool empty();
	/*
	*	�������Ԫ��
	*/
	void clear();
	/*
	*	�ж�Ԫ���Ƿ����
	*/
	bool isExist(const Item &itm);

	/*
	*	�׸�node,������������ʹ��
	*/
	GMBNodeIterator<Item> first();

	/*
	*	���һ��NODE��,������������ʹ��
	*/
	GMBNodeIterator<Item> last();


	/*
	*	�����׸�ITEM
	*/
	Item & front();

	/*
	*	��������Item
	*/
	Item & back();

	/**
	*	���б��н���һ���ڵ�
	*/
	GMBNodeIterator<Item> isolate(GMBNodeIterator<Item>);

	/*
	* ��ָ��������ǰ����ڵ�,�����²���ڵ������
	*/
	GMBNodeIterator<Item> insert(GMBNodeIterator<Item>, const Item &);
	/**
	*	��һ���ڵ���뵽���β
	*/
	void push_back(GMBNodeIterator<Item>);
	/**
	*	��һ���ڵ���뵽����׸�
	*/
	void push_front(GMBNodeIterator<Item>);
	/*
	*	����SLT
	*/
	//********************************************

	/*
	*	�׸�NODE
	*/
	GMBNodeIterator<Item> begin()
	{
		return first();
	}

	/*
	*	����Ԫ��
	*/
	GMBNodeIterator<Item> end()
	{
		return GMBNodeIterator<Item>(NULL);
	}
	bool push_back( const Item &itm);
	bool push_front(const Item &itm);
	void pop_front()
	{
		_remove(m_pFirstNode);
	}
	void pop_back()
	{
		_remove(m_pLastNode);
	}

	/**
	 *	��src���еĽڵ�ȫ�����뵽��ǰ�ı��У�ִ����Ϻ�src��sizeΪ0��
	 *	�÷�����������
	 */
	void merge_nosort(GMBlist<Item,GMMemAllocMethod> &src);

	/**
	 *	SRC�����ݸ��Ƶ���ǰ���У������ǰ���Ѿ������ݣ���ִ��clear()������ִ����Ϻ�src�뵱ǰ��������ͬ
	 */
	GMBlist<Item,GMMemAllocMethod> & operator=(GMBlist<Item,GMMemAllocMethod> &src);
	
	//**********************************************

protected:

	void _remove( GMBNode<Item> *pNode);
	GMBNodeIterator<Item> find(Item value);

protected:
	size_t m_nSize;
	GMBNode<Item>* m_pFirstNode;
	GMBNode<Item>* m_pLastNode;
};

template <class Item
, template<class> class GMMemAllocMethod>
bool GMBlist<Item, GMMemAllocMethod>::push_back( const Item &itm)
{
	GMBNode<Item> *pNode = new(std::nothrow) SNode; 
	if (!pNode) return false;

	if (m_pLastNode)
		m_pLastNode->pNext = pNode;
	pNode->pPrior= m_pLastNode;
	new (&(pNode->value)) Item(itm);
	pNode->pNext = NULL;
	m_pLastNode = pNode;

	if( !m_pFirstNode)
		m_pFirstNode = pNode;

	m_nSize++;
	return true;
}

template <class Item
, template<class> class GMMemAllocMethod>
bool GMBlist<Item, GMMemAllocMethod>::push_front(const Item &itm)
{
	GMBNode<Item> *pNode =  new(std::nothrow) SNode; 
	if (!pNode) return false;

	if (m_pFirstNode)
		m_pFirstNode->pPrior = pNode;
	pNode->pPrior = NULL;
	pNode->pNext = m_pFirstNode;
	new (&(pNode->value)) Item(itm);
	m_pFirstNode = pNode;

	if (!m_pLastNode)
		m_pLastNode = pNode;

	m_nSize++;
	return true;

}

template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::isolate(GMBNodeIterator<Item> pNodeIter)
{
	GMBNode<Item> * pNode = pNodeIter;
	if (pNode != NULL)
	{
		if (pNode->pPrior) 
			pNode->pPrior->pNext = pNode->pNext;
		else
			m_pFirstNode= pNode->pNext;

		if (pNode->pNext) 
			pNode->pNext->pPrior = pNode->pPrior;
		else
			m_pLastNode = pNode->pPrior;
		m_nSize--;
	}
	return pNodeIter;
}

template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::insert(GMBNodeIterator<Item> posIter, const Item & item)
{
	GMBNode<Item> * pos = posIter;
	GMBNode<Item> *pNode =  new(std::nothrow)  SNode; 
	if (!pNode) return GMBNodeIterator<Item>(NULL);
	new(&(pNode->value)) Item(item);

	if(!pos)
	{
		push_back(pNode);
		return pNode;
	}
	if(!pos->pPrior)
	{
		push_front(pNode);
		return pNode;
	}

	pos->pPrior->pNext = pNode;
	pNode->pPrior = pos->pPrior;
	pos->pPrior = pNode;
	pNode->pNext = pos;
	m_nSize++;
	return GMBNodeIterator<Item>(pNode);
}

template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::push_back(GMBNodeIterator<Item> pNodeIt)
{
	GMBNode<Item> * pNode = pNodeIt;	
	if(!pNode) return;

	if (m_pLastNode)
		m_pLastNode->pNext = pNode;
	pNode->pPrior= m_pLastNode;
	pNode->pNext = NULL;
	m_pLastNode = pNode;

	if( !m_pFirstNode)
		m_pFirstNode = pNode;

	m_nSize++;
}
template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::push_front(GMBNodeIterator<Item> pNodeIt)
{
	GMBNode<Item> * pNode = pNodeIt;	
	if(!pNode) return;

	if (m_pFirstNode)
		m_pFirstNode->pPrior = pNode;
	pNode->pPrior = NULL;
	pNode->pNext = m_pFirstNode;
	m_pFirstNode = pNode;
	if (!m_pLastNode)
		m_pLastNode = pNode;

	m_nSize++;
}

template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::remove(const Item &itm)
{
	GMBNode<Item>* iter = begin();
	while(iter != end())
	{
		if(iter->value == itm)
			iter = erase(iter);
		else
			iter = iter->pNext;
	}
}

template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::erase( GMBNodeIterator<Item> it)
{
	if(it == end())
		return end();

	GMBNode<Item>* next = it->pNext;
	_remove((GMBNode<Item>*)it);
	return GMBNodeIterator<Item>(next);
}

template <class Item
, template<class> class GMMemAllocMethod>
size_t GMBlist<Item, GMMemAllocMethod>::size()
{
	return m_nSize;
}

template <class Item
, template<class> class GMMemAllocMethod>
bool GMBlist<Item, GMMemAllocMethod>::empty()
{
	return (m_nSize == 0);
}

template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::find(Item value)
{
	GMBNode<Item>* pRtn = NULL;
	GMBNode<Item>* pNode = m_pFirstNode;

	while (pNode)
	{
		if (pNode->value == value)
		{
			pRtn = pNode;
			break;
		}
		pNode = pNode->pNext;
	}
	return GMBNodeIterator<Item>(pRtn);
}

template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::clear()
{
	GMBNode<Item>* pNode = m_pFirstNode;

	while (m_pFirstNode)
	{	
		pop_front();
	}


}
template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::first()
{
	return GMBNodeIterator<Item>(m_pFirstNode);
}

template <class Item
, template<class> class GMMemAllocMethod>
GMBNodeIterator<Item> GMBlist<Item, GMMemAllocMethod>::last()
{
	return GMBNodeIterator<Item>(m_pLastNode);
}

template <class Item
, template<class> class GMMemAllocMethod>
bool GMBlist<Item, GMMemAllocMethod>::isExist(const Item& itm)
{
	return (find(itm) != end());
}

template <class Item
, template<class> class GMMemAllocMethod>
Item & GMBlist<Item, GMMemAllocMethod>::front()
{
	return m_pFirstNode->value;
}
template <class Item
, template<class> class GMMemAllocMethod>
Item & GMBlist<Item, GMMemAllocMethod>::back()
{
	return m_pLastNode->value;
}

template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::_remove( GMBNode<Item> *pNode)
{
	if (pNode)
	{
		if (pNode->pPrior) 
			pNode->pPrior->pNext = pNode->pNext;
		else
			m_pFirstNode= pNode->pNext;

		if (pNode->pNext) 
			pNode->pNext->pPrior = pNode->pPrior;
		else
			m_pLastNode = pNode->pPrior;

		delete static_cast<SNode*>(pNode);

		m_nSize--;
	}
}
template <class Item
, template<class> class GMMemAllocMethod>
void GMBlist<Item, GMMemAllocMethod>::merge_nosort(GMBlist<Item,GMMemAllocMethod> &src)
{
	while(!src.empty())
	{
		this->push_back(src.isolate(src.begin()));
	}
	assert(src.empty());
}
template <class Item
, template<class> class GMMemAllocMethod>
GMBlist<Item,GMMemAllocMethod> & GMBlist<Item, GMMemAllocMethod>::operator=(GMBlist<Item,GMMemAllocMethod> &src)
{
	this->clear();
	typename GMBlist<Item,GMMemAllocMethod>::iterator it = src.begin();
	for(; it != src.end(); ++it)
	{
		this->push_back(*it);
	}
	assert(this->size() == src.size());
	return *this;
}

#endif
