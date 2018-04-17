#pragma once
#include <malloc.h>

/**
*	����FIFO����, �Ƚ��ȳ�, û�������������
*	û�л�ȡ������Ԫ�ظ����ķ���, ���Ը���: ��count�ڳ�һ��ռ����Ÿ���, ��������ABA�ķ���
*	64λwindowsϵͳ����ʱ����Ҫ��һ��CAS��ʵ��
*/
template<class TYPE>
class GMLockFreeFIFOQueue
{
public:
	enum INIT_RESULT
	{
		INIT_OK = 0, // �ɹ�
		INIT_CASFAIL =	0x80000001L, //��֧��˫ָ���CAS
		INIT_HEAPFAIL =	0x80000002L, //��������ڵ�Ķ�ʧ��
		INIT_MEMFAIL =	0x80000003L  //�ڴ����ʧ��
	};

private:
	struct Node_t;

	/// ָ����һ���ڵ��ָ��, ���װ��һ������������
	struct Pointer_t
	{
		Node_t * ptr;  ///< ָ����һ���ڵ��ָ��
		UINT_PTR count; ///< ���Ӽ�����Ϊ�˷�ֹABA����

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

	/// �ڵ�
	struct Node_t
	{
		Pointer_t next; //ָ����������һ���ڵ�, ( ˳��Ҫ��: ��Ҫ���ڽṹ���һ��, (�ڴ�ı߽����Ҫ��) )
		TYPE data;
		Node_t() {
		}
		Node_t( const TYPE & d ): data( d ) {
		}
	};

	Pointer_t m_head; ///<ָ�����ͷ
	Pointer_t m_tail; ///<ָ�����β
	HANDLE m_hHeap; ///<����ڵ��ڴ�ĶѾ��

private:

	/// ԭ�ӵı�֤Pointer_t( ˫ָ��ռ� )��compare_and_change
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
		//0501��2003server
		return InterlockedCompareExchange64( (volatile LONGLONG*)pDest, \
			*(LONGLONG*)&newValue, *(LONGLONG*)&cmpValue );
#else
		return *pDest; ///��ִ��,ֱ�ӷ���ԭ����ֵ
#endif
	}

	/// ����CAS�Ƿ����
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

	/// ����ڵ��ڴ�
	Node_t * AllocNode()
	{
		//CASҪ��MEMORY_ALLOCATION_ALIGNMENT�߽����

	//	return (Node_t*)_aligned_malloc( sizeof(Node_t), MEMORY_ALLOCATION_ALIGNMENT );
		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
	}

	/// �ͷŽڵ��ڴ�
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
		//����ʱӦ��Ҫ��queue��Ϊ��
		assert( m_head.ptr == m_tail.ptr );
		assert( m_head.ptr->next.ptr == NULL );

		//��������ʼ��ʱ�����dummy Node,���δ��ʼ����,��m_head.ptr==NULL,����Ҳû����
		FreeNode( m_head.ptr );

		HeapDestroy( m_hHeap );
	}

	/**
	 *	��ʼ��
	 *	��֧��CASʱ, ����INIT_CASFAIL
	 *	������ʧ��, ����INIT_HEAPFAIL
	 *	����dummy�ڵ��ڴ�ʧ��, ����INIT_MEMFAIL
	 *	�ɹ�, ����INIT_OK
	 */
	INIT_RESULT Initialize()
	{
		//����CAS�Ƿ�֧��
		if( !TestCASSupported() ) return INIT_CASFAIL;

		//��������Node�ڴ�Ķ�,��Ҫ���ڷ���dummy Node֮ǰ
		m_hHeap = HeapCreate( 0, 0, 0 );
		if( m_hHeap == NULL ) return INIT_HEAPFAIL;

		//����һ��dummy Node
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

	/// ���ݽ�����, ��Ҫ����ڵ��ڴ�, ����ʧ��ʱ����false,���򷵻�true
	bool EnQueue( const TYPE & data )
	{
		Node_t * pNode = AllocNode();
		if( pNode == NULL ) return false;

		new ( pNode ) Node_t( data ); //placement new, ��pNode�ڴ�λ����dataΪ��������Node_t
		Pointer_t tail, next;
		for( ; ; )
		{
			tail = m_tail;
			next = m_tail.ptr->next;
			if( tail == m_tail )
			{
				if( next.ptr == NULL ) // tailȷʵΪ���һ��
				{
					// ��pNode����tail����һ��, ע: ����û�и���m_tailΪ���һ��,�������ط�����m_tail����ʱ,�ٳ��Ը���
					if( next == CAS( (volatile Pointer_t*)&(tail.ptr->next), \
						Pointer_t(pNode, next.count+1), next ) )
						break;
				}
				else // tail��Ϊ���һ��
				{
					// m_tail����,���Խ�m_tail����Ϊnext.ptr
					CAS((volatile Pointer_t*)&m_tail, \
						Pointer_t(next.ptr,tail.count+1), tail );
				}
			}
		} // end for(;;)
		// �������Ӽ���count
		CAS( (volatile Pointer_t*)&m_tail, Pointer_t(pNode,tail.count+1), tail );
		return true;
	}

	/// ���ݳ�����, �ڶ���Ϊ��ʱ, dataû�и�ֵ, ����false, ����data��ֵΪ����ͷ��ֵ, ����true
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

					//m_tail����,���Խ�m_tail����Ϊnext.ptr
					CAS( (volatile Pointer_t*)&m_tail, \
						Pointer_t(next.ptr, tail.count+1), tail );
				}
				else
				{
					//��CASǰ���dataֵ,����CAS�������߳�DeQueue���ܻ��ͷ�next.ptr
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
