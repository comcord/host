#pragma once
#include <malloc.h>

/**
*	无锁FIFO队列, 先进先出, 没有最大容量限制
*	没有获取队列中元素个数的方法, 可以改造: 将count腾出一半空间来放个数, 但将增加ABA的风险
*	64位windows系统开发时仅需要改一下CAS的实现
*/
template<class TYPE>
class GMLockFreeFIFOQueue
{
public:
	enum INIT_RESULT
	{
		INIT_OK = 0, // 成功
		INIT_CASFAIL =	0x80000001L, //不支持双指针的CAS
		INIT_HEAPFAIL =	0x80000002L, //创建分配节点的堆失败
		INIT_MEMFAIL =	0x80000003L  //内存分配失败
	};

private:
	struct Node_t;

	/// 指向下一个节点的指针, 多包装了一个记数器变量
	struct Pointer_t
	{
		Node_t * ptr;  ///< 指向下一个节点的指针
		UINT_PTR count; ///< 增加记数是为了防止ABA问题

		Pointer_t():ptr(NULL), count(0) {
		}

		Pointer_t( Node_t * pN, UINT_PTR c ): ptr(pN), count(c) {
		}

		Pointer_t & operator=( const Pointer_t & pt ) 
		{
			if( this == &pt ) return *this;
			ptr = pt.ptr;
			count = pt.count;
			return *this;
		}

		bool operator==( const Pointer_t & pt )
		{
			return ptr == pt.ptr && count == pt.count;
		}
	};

	/// 节点
	struct Node_t
	{
		Pointer_t next; //指向链表里下一个节点, ( 顺序要求: 需要放在结构体第一个, (内存的边界对齐要求) )
		TYPE data;
		Node_t() {
		}
		Node_t( const TYPE & d ): data( d ) {
		}
	};

	Pointer_t m_head; ///<指向队列头
	Pointer_t m_tail; ///<指向队列尾
	HANDLE m_hHeap; ///<分配节点内存的堆句柄

private:

	/// 原子的保证Pointer_t( 双指针空间 )的compare_and_change
	inline Pointer_t CAS( volatile Pointer_t* pDest, Pointer_t newValue, Pointer_t cmpValue )
	{
#if !defined (_WIN64) && defined (_M_IX86) && (_M_IX86 >= 500)
		Pointer_t rtn;
		__asm
		{
			mov eax, dword ptr [cmpValue]
			mov edx, dword ptr [cmpValue+4]
			mov ebx, dword ptr [newValue]
			mov ecx, dword ptr [newValue+4]
			mov esi, dword ptr [pDest]
			lock cmpxchg8b qword ptr [esi]
			mov dword ptr [rtn], eax;
			mov dword ptr [rtn+4], edx;
		}
		return rtn;
#elif ( !defined (WINVER) || (WINVER >= 0x0501) ) && \
	( !defined (_WIN32_WINNT) || (_WIN32_WINNT >= 0x0501) )
		//0501是2003server
		return InterlockedCompareExchange64( (volatile LONGLONG*)pDest, \
			*(LONGLONG*)&newValue, *(LONGLONG*)&cmpValue );
#else
		return *pDest; ///不执行,直接返回原来的值
#endif
	}

	/// 测试CAS是否可用
	inline bool TestCASSupported()
	{
		Pointer_t * pDest = (Pointer_t*)_aligned_malloc( sizeof(Pointer_t), MEMORY_ALLOCATION_ALIGNMENT );
		if( pDest == NULL ) return false;
		new( pDest ) Pointer_t( (Node_t*)8, (UINT_PTR)9 );
		Pointer_t cmp( (Node_t*)8, (UINT_PTR)9 );
		Pointer_t exchange( (Node_t*)0xA, (UINT_PTR)0xB );
		return ( cmp == CAS((volatile Pointer_t*)pDest, exchange, cmp) \
			&& *pDest == exchange );
	}

	/// 分配节点内存
	Node_t * AllocNode()
	{
		//CAS要求MEMORY_ALLOCATION_ALIGNMENT边界对齐

	//	return (Node_t*)_aligned_malloc( sizeof(Node_t), MEMORY_ALLOCATION_ALIGNMENT );
		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
	}

	/// 释放节点内存
	void FreeNode( void * pNode )
	{
		//_aligned_free( pNode );
		HeapFree( m_hHeap, 0, pNode );
	}

public:
	GMLockFreeFIFOQueue() : m_hHeap(NULL) {
	}

	~GMLockFreeFIFOQueue() 
	{
		//析构时应该要求queue已为空
		assert( m_head.ptr == m_tail.ptr );
		assert( m_head.ptr->next.ptr == NULL );

		//析构掉初始化时分配的dummy Node,如果未初始化过,则m_head.ptr==NULL,调用也没问题
		FreeNode( m_head.ptr );

		HeapDestroy( m_hHeap );
	}

	/**
	 *	初始化
	 *	不支持CAS时, 返回INIT_CASFAIL
	 *	创建堆失败, 返回INIT_HEAPFAIL
	 *	分配dummy节点内存失败, 返回INIT_MEMFAIL
	 *	成功, 返回INIT_OK
	 */
	INIT_RESULT Initialize()
	{
		//测试CAS是否支持
		if( !TestCASSupported() ) return INIT_CASFAIL;

		//创建分配Node内存的堆,需要放在分配dummy Node之前
		m_hHeap = HeapCreate( 0, 0, 0 );
		if( m_hHeap == NULL ) return INIT_HEAPFAIL;

		//分配一个dummy Node
		Node_t * pNode = AllocNode();
		if( pNode == NULL )
		{
			HeapDestroy( m_hHeap );
			m_hHeap = NULL;
			return INIT_MEMFAIL;
		}

		ZeroMemory( &(pNode->next), sizeof(Pointer_t) );
		m_head.ptr = m_tail.ptr = pNode;
		return INIT_OK;
	}

	/// 数据进队列, 需要分配节点内存, 分配失败时返回false,否则返回true
	bool EnQueue( const TYPE & data )
	{
		Node_t * pNode = AllocNode();
		if( pNode == NULL ) return false;

		new ( pNode ) Node_t( data ); //placement new, 在pNode内存位置以data为参数构造Node_t
		Pointer_t tail, next;
		for( ; ; )
		{
			tail = m_tail;
			next = m_tail.ptr->next;
			if( tail == m_tail )
			{
				if( next.ptr == NULL ) // tail确实为最后一个
				{
					// 将pNode插在tail的下一个, 注: 这里没有更新m_tail为最后一个,在其他地方发现m_tail不新时,再尝试更新
					if( next == CAS( (volatile Pointer_t*)&(tail.ptr->next), \
						Pointer_t(pNode, next.count+1), next ) )
						break;
				}
				else // tail不为最后一个
				{
					// m_tail不新,尝试将m_tail更新为next.ptr
					CAS((volatile Pointer_t*)&m_tail, \
						Pointer_t(next.ptr,tail.count+1), tail );
				}
			}
		} // end for(;;)
		// 尝试增加记数count
		CAS( (volatile Pointer_t*)&m_tail, Pointer_t(pNode,tail.count+1), tail );
		return true;
	}

	/// 数据出队列, 在队列为空时, data没有赋值, 返回false, 否则data赋值为队列头的值, 返回true
	bool DeQueue( TYPE & data )
	{
		Pointer_t head, tail, next;
		for( ; ; )
		{
			head = m_head;
			tail = m_tail;
			next = head.ptr->next;
			if( head == m_head )
			{
				if( head.ptr == tail.ptr )
				{
					if( next.ptr == NULL ) 
						return false;

					//m_tail不新,尝试将m_tail更新为next.ptr
					CAS( (volatile Pointer_t*)&m_tail, \
						Pointer_t(next.ptr, tail.count+1), tail );
				}
				else
				{
					//在CAS前获得data值,否则CAS后其他线程DeQueue可能会释放next.ptr
					data = next.ptr->data;
					if( head == CAS( (volatile Pointer_t*)&m_head, \
						Pointer_t(next.ptr, head.count+1), head ) )
						break;
				}
			}
		} // end for(;;)
		FreeNode( head.ptr );
		return true;
	}
};
