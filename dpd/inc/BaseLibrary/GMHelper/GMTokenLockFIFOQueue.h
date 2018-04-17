#ifndef GM_GF_TOKENLOCKFIFOQUEUE_20090430
#define GM_GF_TOKENLOCKFIFOQUEUE_20090430

#include "GMToken.h"

template<class T>

class GMTokenLockFIFOQueue
{
private:
	struct Node_t;

	/// 指向下一个节点的指针, 多包装了一个记数器变量
	struct Pointer_t
	{
		Node_t* ptr;   ///< 指向下一个节点的指针
		Pointer_t():ptr(NULL)
		{}

		Pointer_t( Node_t * pN): ptr(pN)
		{}

		Pointer_t & operator=( const Pointer_t & pt ) 
		{
			if( this == &pt ) return *this;
			ptr = pt.ptr;
			return *this;
		}

		bool operator==( const Pointer_t & pt )
		{
			return ptr == pt.ptr;
		}
	};

	/// 节点
	struct Node_t
	{
		Pointer_t next; //指向链表里下一个节点, ( 顺序要求: 需要放在结构体第一个, (内存的边界对齐要求) )
		T data;

		Node_t()
		{}

		Node_t( const T & d ): data( d )
		{}
	};

	/// 分配节点内存
	Node_t * AllocNode()
	{
#ifdef WIN32
		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
#else // linux
		return (Node_t*)malloc(sizeof(Node_t));
#endif
	}

	/// 释放节点内存
	void FreeNode( void * pNode )
	{
#ifdef WIN32
		HeapFree( m_hHeap, 0, pNode );
#else // linux
		free(pNode);
#endif
	}

private:
	Pointer_t m_head; ///<指向队列头
	Pointer_t m_tail; ///<指向队列尾
#ifdef WIN32
	HANDLE m_hHeap; ///<分配节点内存的堆句柄
#endif
	unsigned int m_counter;
	GMToken m_token;

public:
#ifdef WIN32
	GMTokenLockFIFOQueue():m_hHeap(NULL),m_counter(0)
	{
	}

	~GMTokenLockFIFOQueue() 
	{
		//析构时应该要求queue已为空
		assert(m_counter == 0);
		HeapDestroy( m_hHeap );
	}
#else // linux
	GMTokenLockFIFOQueue():m_counter(0)
	{
	}

	~GMTokenLockFIFOQueue() 
	{
		//析构时应该要求queue已为空
		assert(m_counter == 0);
	}
#endif

	/**
	*	初始化
	*	不支持CAS时, 返回INIT_CASFAIL
	*	创建堆失败, 返回INIT_HEAPFAIL
	*	分配dummy节点内存失败, 返回INIT_MEMFAIL
	*	成功, 返回INIT_OK
	*/
	bool Initialize()
	{
		//创建分配Node内存的堆,需要放在分配dummy Node之前
#ifdef WIN32
		m_hHeap = HeapCreate( 0, 0, 0 );
		if( m_hHeap == NULL ) 
			return false;
#endif
		/*Node_t* pNode = AllocNode();
		if( pNode == NULL )
		{
			HeapDestroy(m_hHeap);
			m_hHeap = NULL;
			return false;
		}

		ZeroMemory( &(pNode->next), sizeof(Pointer_t) );
		m_head.ptr = m_tail.ptr = pNode;*/
		return true;
	}

	/// 数据进队列, 需要分配节点内存, 分配失败时返回false,否则返回true
	bool EnQueue( const T & data )
	{
		Node_t* pNode = AllocNode();
		if( pNode == NULL )
			return false;

		new (pNode) Node_t( data ); //placement new, 在pNode内存位置以data为参数构造Node_t

		m_token.Borrow();
		if( (m_tail == m_head) && (m_tail.ptr == NULL) ) ///<表示现在队列是一个空表
		{
			m_tail.ptr = pNode;
			m_head = m_tail;
		}
		else
		{
			m_tail.ptr->next = Pointer_t(pNode);
			m_tail = m_tail.ptr->next;
		}

		m_counter++;
		m_token.Repay();
		return true;
	}

	bool DeQueue( T & data )
	{
		m_token.Borrow();
		if(m_counter == 0)
		{
			m_token.Repay();
			return false;
		}
		
		m_counter--;
		Pointer_t head = m_head;
		data = head.ptr->data;
		m_head = m_head.ptr->next;
		FreeNode( head.ptr );

		if(m_counter == 0)
		{
			m_head.ptr = NULL;
			m_tail = m_head;
		}

		m_token.Repay();
		return true;
	}
}; // GMTokenLockFIFOQueue

//template<class T>
//class GMTokenLockFIFOQueue
//{
//private:
//	struct Node_t;
//
//	/// 指向下一个节点的指针, 多包装了一个记数器变量
//	struct Pointer_t
//	{
//		Node_t* ptr;   ///< 指向下一个节点的指针
//		Pointer_t():ptr(NULL)
//		{}
//
//		Pointer_t( Node_t * pN): ptr(pN)
//		{}
//
//		Pointer_t & operator=( const Pointer_t & pt ) 
//		{
//			if( this == &pt ) return *this;
//			ptr = pt.ptr;
//			return *this;
//		}
//
//		bool operator==( const Pointer_t & pt )
//		{
//			return ptr == pt.ptr;
//		}
//	};
//
//	/// 节点
//	struct Node_t
//	{
//		Pointer_t next; //指向链表里下一个节点, ( 顺序要求: 需要放在结构体第一个, (内存的边界对齐要求) )
//		T data;
//
//		Node_t()
//		{}
//
//		Node_t( const T & d ): data( d )
//		{}
//	};
//
//	/// 分配节点内存
//	Node_t * AllocNode()
//	{
//		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
//	}
//
//	/// 释放节点内存
//	void FreeNode( void * pNode )
//	{
//		HeapFree( m_hHeap, 0, pNode );
//	}
//
//private:
//	Pointer_t m_head; ///<指向队列头
//	Pointer_t m_tail; ///<指向队列尾
//	HANDLE m_hHeap; ///<分配节点内存的堆句柄
//	unsigned int m_counter;
//	GMToken m_token;
//
//public:
//	GMTokenLockFIFOQueue():m_hHeap(NULL),m_counter(0)
//	{
//	}
//
//	~GMTokenLockFIFOQueue() 
//	{
//		//析构时应该要求queue已为空
//		assert(m_counter == 0);
//
//		HeapDestroy( m_hHeap );
//	}
//
//	/**
//	*	初始化
//	*	不支持CAS时, 返回INIT_CASFAIL
//	*	创建堆失败, 返回INIT_HEAPFAIL
//	*	分配dummy节点内存失败, 返回INIT_MEMFAIL
//	*	成功, 返回INIT_OK
//	*/
//	bool Initialize()
//	{
//		//创建分配Node内存的堆,需要放在分配dummy Node之前
//		m_hHeap = HeapCreate( 0, 0, 0 );
//		if( m_hHeap == NULL ) 
//			return false;
//
//		/*Node_t* pNode = AllocNode();
//		if( pNode == NULL )
//		{
//			HeapDestroy(m_hHeap);
//			m_hHeap = NULL;
//			return false;
//		}
//
//		ZeroMemory( &(pNode->next), sizeof(Pointer_t) );
//		m_head.ptr = m_tail.ptr = pNode;*/
//		return true;
//	}
//
//	/// 数据进队列, 需要分配节点内存, 分配失败时返回false,否则返回true
//	bool EnQueue( const T & data )
//	{
//		Node_t* pNode = AllocNode();
//		if( pNode == NULL )
//			return false;
//
//		new (pNode) Node_t( data ); //placement new, 在pNode内存位置以data为参数构造Node_t
//
//		m_token.Borrow();
//		if( (m_tail == m_head) && (m_tail.ptr == NULL) ) ///<表示现在队列是一个空表
//		{
//			m_tail.ptr = pNode;
//			m_head = m_tail;
//		}
//		else
//		{
//			m_tail.ptr->next = Pointer_t(pNode);
//			m_tail = m_tail.ptr->next;
//		}
//
//		m_counter++;
//		m_token.Repay();
//		return true;
//	}
//
//	bool DeQueue( T & data )
//	{
//		m_token.Borrow();
//		if(m_counter == 0)
//		{
//			m_token.Repay();
//			return false;
//		}
//		
//		m_counter--;
//		Pointer_t head = m_head;
//		data = head.ptr->data;
//		m_head = m_head.ptr->next;
//		FreeNode( head.ptr );
//
//		if(m_counter == 0)
//		{
//			m_head.ptr = NULL;
//			m_tail = m_head;
//		}
//
//		m_token.Repay();
//		return true;
//	}
//};

#endif //_GF_TOKENLOCKFIFOQUEUE_20090430
