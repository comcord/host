#ifndef GMBinDirectList_H_2006_CHANNELSOFT
#define GMBinDirectList_H_2006_CHANNELSOFT

#include <stdio.h>
template <class Item>
class Node
{

public:
	Node()
		:pPrior(NULL), pNext(NULL)
	{
	}
	Node<Item> * pPrior;
	Item value;
	Node<Item> * pNext;
};


/*
 *	NodeITERATOR
 */
template<class Item>
class NodeIterator
{
public:
	NodeIterator()
		:m_pNode(NULL)
	{
	}
	NodeIterator(Node<Item> *pNode)
	{
		m_pNode = pNode;
	}
	~NodeIterator()
	{

	}

	operator Node<Item>*()//����ת��
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
	Node<Item> *operator->()
	{
		return m_pNode;
	}

	/*
	 *	��ֵ
	 */
	NodeIterator & operator=(Node<Item> *pNode)
	{
		m_pNode = pNode;
		return *this;
	}
	/*
	 *	���������
	 */
	Node<Item> * operator++()
	{
		if(m_pNode)
			m_pNode = m_pNode->pNext;

		return m_pNode;
	}
	const Node<Item> * operator++(int)
	{
		Node<Item> * old = m_pNode;
		++(*this);
		return old;
	}
		
	Node<Item> * operator--()
	{
		if (m_pNode) 
			m_pNode = m_pNode->pPrior;
		return m_pNode;
	}
	const Node<Item> * operator--(int)
	{
		Node<Item> * old = m_pNode;
		--(*this);
		return old;
	}

	/*
	 *	�ȽϷ���
	 */
	bool operator==(Node<Item> *pNode)
	{
		return (m_pNode == pNode);
	}
	bool operator!=(Node<Item> *pNode)
	{
		return (m_pNode != pNode);
	}


private:
	
	Node<Item> * m_pNode;
};

/*
 *	LIST�нڵ���ڴ������,�����Ҫ�����ظ���Ϳ�����
 */
template <class Item>
class NodeMemAlloc
{
public:
	virtual Node<Item> *allocNode()
	{
		return ::new(std::nothrow) Node<Item>;
	}
	virtual void freeNode(Node<Item> *p)
	{
		::delete p;
	}
};

template <class Item>
class NodeMemAllocByHeap : public NodeMemAlloc<Item>
{
	HANDLE m_hHeap;
public:
	NodeMemAllocByHeap()
	{
		m_hHeap = ::HeapCreate(0,0,0);
	}
	virtual ~NodeMemAllocByHeap()
	{
		if(m_hHeap)
			::HeapDestroy(m_hHeap);
	}
	virtual Node<Item> *allocNode()
	{
		if(m_hHeap)
		{ 
			return (Node<Item>*)::HeapAlloc(m_hHeap, 0, sizeof(Node<Item>));
		}
		else 
		{
			return  NodeMemAlloc<Item>::allocNode();
		}
	}
	virtual void freeNode(Node<Item> *p)
	{
		if(p)
		{
			if(m_hHeap)
			{
				::HeapFree(m_hHeap, 0, p);
			}
			else
			{
				NodeMemAlloc<Item>::freeNode(p);
			}
		}
	}

};


/*
 *	˫������
 */
//list	
template <class Item>
class GMBinDirectList  
{
public:
	typedef NodeIterator<Item> iterator;

	GMBinDirectList(NodeMemAlloc<Item> *pNodealloc = NULL)
		:m_pFirstNode(NULL), m_pLastNode(NULL), m_nSize(0),m_pNodeAlloc(pNodealloc)
	{
	}
	virtual ~GMBinDirectList()
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

	//void erase( NodeIterator<Item> it);

	Node<Item>* erase(Node<Item>* it);

	/*
	 *	��ô�С
	 */
    int size();
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
	Node<Item>* first();

	/*
	 *	���һ��NODE��,������������ʹ��
	 */
	Node<Item>* last();


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
	Node<Item> *isolate(Node<Item> *);

	/*
	* ��ָ��������ǰ����ڵ�,�����²���ڵ������
	*/
	Node<Item> * insert(Node<Item> *, const Item &);
	/**
	 *	��һ���ڵ���뵽���β
	 */
	void push_back(Node<Item> *);
	/**
	*	��һ���ڵ���뵽����׸�
	*/
	void push_front(Node<Item> *);
	/*
	 *	����SLT
	 */
//********************************************

	/*
	 *	�׸�NODE
	 */
	Node<Item>* begin()
	{
		return first();
	}

	/*
	 *	����Ԫ��
	 */
	Node<Item>* end()
	{
		return NULL;
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
//**********************************************

protected:
	
	void _remove( Node<Item> *pNode);

	Node<Item>* find(Item value);

protected:

	int m_nSize;
	NodeMemAlloc<Item> * m_pNodeAlloc;
	Node<Item>* m_pFirstNode;
	Node<Item>* m_pLastNode;
};

template<class Item>
bool GMBinDirectList<Item>::push_back( const Item &itm)
{
	Node<Item> *pNode = NULL;
	if(m_pNodeAlloc == NULL)
	{
		pNode = new(std::nothrow) Node<Item>;
		if (!pNode) return false;
	}
	else
	{
		pNode = m_pNodeAlloc->allocNode(); 
		if (!pNode) return false;
		new(pNode) Node<Item>();
	}
	

	if (m_pLastNode)
		m_pLastNode->pNext = pNode;
	pNode->pPrior= m_pLastNode;
	pNode->value = itm;
	pNode->pNext = NULL;
	m_pLastNode = pNode;

	if( !m_pFirstNode)
		m_pFirstNode = pNode;

	m_nSize++;
	return true;
}

template<class Item>
bool GMBinDirectList<Item>::push_front(const Item &itm)
{
	Node<Item> *pNode = NULL;
	if(m_pNodeAlloc == NULL)
	{
		pNode = new(std::nothrow) Node<Item>;
		if (!pNode) return false;
	}
	else
	{
		pNode = m_pNodeAlloc->allocNode(); 
		if (!pNode) return false;
		new(pNode) Node<Item>();
	}

	if (m_pFirstNode)
		m_pFirstNode->pPrior = pNode;
	pNode->pPrior = NULL;
	pNode->pNext = m_pFirstNode;
	pNode->value = itm;
	m_pFirstNode = pNode;

	if (!m_pLastNode)
		m_pLastNode = pNode;

	m_nSize++;
	return true;

}


template<class Item>
Node<Item> *GMBinDirectList<Item>::isolate(Node<Item> *pNode)
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
		m_nSize--;
	}
	return pNode;
}
template<class Item>
Node<Item> * GMBinDirectList<Item>::insert(Node<Item> *pos, const Item & item)
{

	Node<Item> *pNode = NULL;
	if(m_pNodeAlloc == NULL)
	{
		pNode = new(std::nothrow) Node<Item>;
		if (!pNode) return NULL;
	}
	else
	{
		pNode = m_pNodeAlloc->allocNode(); 
		if (!pNode) return NULL;
		new(pNode) Node<Item>();
	}

	pNode->value = item;
	
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
	return pNode;
}

template<class Item>
void GMBinDirectList<Item>::push_back(Node<Item> *pNode)
{
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
template<class Item>
void GMBinDirectList<Item>::push_front(Node<Item> *pNode)
{
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

template<class Item>
void GMBinDirectList<Item>::remove(const Item &itm)
{
	Node<Item>* iter = begin();
	while(iter != end())
	{
		if(iter->value == itm)
		  iter = erase(iter);
		else
			iter = iter->pNext;
	}
}

template<class Item>
Node<Item>* GMBinDirectList<Item>::erase( Node<Item>* it)
{
	if(it == end())
		return end();

	Node<Item>* next = it->pNext;
	_remove(it);
	return next;
}

template<class Item>
int GMBinDirectList<Item>::size()
{
	return m_nSize;
}

template<class Item>
bool GMBinDirectList<Item>::empty()
{
	return (m_nSize <= 0);
}

template<class Item>
Node<Item>* GMBinDirectList<Item>::find(Item value)
{
	Node<Item>* pRtn = NULL;
	Node<Item>* pNode = m_pFirstNode;
	
	while (pNode)
	{
		if (pNode->value == value)
		{
			pRtn = pNode;
			break;
		}
		pNode = pNode->pNext;
	}
	return pRtn;
}

template<class Item>
void GMBinDirectList<Item>::clear()
{
	Node<Item>* pNode = m_pFirstNode;
	
	while (m_pFirstNode)
	{	
		pop_front();
	}
	
	
}
template<class Item>
Node<Item>* GMBinDirectList<Item>::first()
{
	return m_pFirstNode;
}

template<class Item>
Node<Item>* GMBinDirectList<Item>::last()
{
	return m_pLastNode;
}

template<class Item>
bool GMBinDirectList<Item>::isExist(const Item& itm)
{
	return (find(itm) != NULL);
}

template<class Item>
Item & GMBinDirectList<Item>::front()
{
	return m_pFirstNode->value;
}
template<class Item>
Item & GMBinDirectList<Item>::back()
{
	return m_pLastNode->value;
}

template<class Item>
void GMBinDirectList<Item>::_remove( Node<Item> *pNode)
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
		
		if(m_pNodeAlloc == NULL)
			delete pNode;
		else
			m_pNodeAlloc->freeNode(pNode);
		//delete pNode;
		
		m_nSize--;
	}

}

#endif // !defined(AFX_BIDIRECTLIST_H__9A890B75_280E_42FB_A92C_A572FFB3BA0B__INCLUDED_)
