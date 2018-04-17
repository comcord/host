#ifndef GM_GF_TOKENLOCKFIFOQUEUE_20090430
#define GM_GF_TOKENLOCKFIFOQUEUE_20090430

#include "GMToken.h"

template<class T>

class GMTokenLockFIFOQueue
{
private:
	struct Node_t;

	/// ָ����һ���ڵ��ָ��, ���װ��һ������������
	struct Pointer_t
	{
		Node_t* ptr;   ///< ָ����һ���ڵ��ָ��
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

	/// �ڵ�
	struct Node_t
	{
		Pointer_t next; //ָ����������һ���ڵ�, ( ˳��Ҫ��: ��Ҫ���ڽṹ���һ��, (�ڴ�ı߽����Ҫ��) )
		T data;

		Node_t()
		{}

		Node_t( const T & d ): data( d )
		{}
	};

	/// ����ڵ��ڴ�
	Node_t * AllocNode()
	{
#ifdef WIN32
		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
#else // linux
		return (Node_t*)malloc(sizeof(Node_t));
#endif
	}

	/// �ͷŽڵ��ڴ�
	void FreeNode( void * pNode )
	{
#ifdef WIN32
		HeapFree( m_hHeap, 0, pNode );
#else // linux
		free(pNode);
#endif
	}

private:
	Pointer_t m_head; ///<ָ�����ͷ
	Pointer_t m_tail; ///<ָ�����β
#ifdef WIN32
	HANDLE m_hHeap; ///<����ڵ��ڴ�ĶѾ��
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
		//����ʱӦ��Ҫ��queue��Ϊ��
		assert(m_counter == 0);
		HeapDestroy( m_hHeap );
	}
#else // linux
	GMTokenLockFIFOQueue():m_counter(0)
	{
	}

	~GMTokenLockFIFOQueue() 
	{
		//����ʱӦ��Ҫ��queue��Ϊ��
		assert(m_counter == 0);
	}
#endif

	/**
	*	��ʼ��
	*	��֧��CASʱ, ����INIT_CASFAIL
	*	������ʧ��, ����INIT_HEAPFAIL
	*	����dummy�ڵ��ڴ�ʧ��, ����INIT_MEMFAIL
	*	�ɹ�, ����INIT_OK
	*/
	bool Initialize()
	{
		//��������Node�ڴ�Ķ�,��Ҫ���ڷ���dummy Node֮ǰ
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

	/// ���ݽ�����, ��Ҫ����ڵ��ڴ�, ����ʧ��ʱ����false,���򷵻�true
	bool EnQueue( const T & data )
	{
		Node_t* pNode = AllocNode();
		if( pNode == NULL )
			return false;

		new (pNode) Node_t( data ); //placement new, ��pNode�ڴ�λ����dataΪ��������Node_t

		m_token.Borrow();
		if( (m_tail == m_head) && (m_tail.ptr == NULL) ) ///<��ʾ���ڶ�����һ���ձ�
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
//	/// ָ����һ���ڵ��ָ��, ���װ��һ������������
//	struct Pointer_t
//	{
//		Node_t* ptr;   ///< ָ����һ���ڵ��ָ��
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
//	/// �ڵ�
//	struct Node_t
//	{
//		Pointer_t next; //ָ����������һ���ڵ�, ( ˳��Ҫ��: ��Ҫ���ڽṹ���һ��, (�ڴ�ı߽����Ҫ��) )
//		T data;
//
//		Node_t()
//		{}
//
//		Node_t( const T & d ): data( d )
//		{}
//	};
//
//	/// ����ڵ��ڴ�
//	Node_t * AllocNode()
//	{
//		return (Node_t*)HeapAlloc( m_hHeap, 0, sizeof(Node_t) );
//	}
//
//	/// �ͷŽڵ��ڴ�
//	void FreeNode( void * pNode )
//	{
//		HeapFree( m_hHeap, 0, pNode );
//	}
//
//private:
//	Pointer_t m_head; ///<ָ�����ͷ
//	Pointer_t m_tail; ///<ָ�����β
//	HANDLE m_hHeap; ///<����ڵ��ڴ�ĶѾ��
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
//		//����ʱӦ��Ҫ��queue��Ϊ��
//		assert(m_counter == 0);
//
//		HeapDestroy( m_hHeap );
//	}
//
//	/**
//	*	��ʼ��
//	*	��֧��CASʱ, ����INIT_CASFAIL
//	*	������ʧ��, ����INIT_HEAPFAIL
//	*	����dummy�ڵ��ڴ�ʧ��, ����INIT_MEMFAIL
//	*	�ɹ�, ����INIT_OK
//	*/
//	bool Initialize()
//	{
//		//��������Node�ڴ�Ķ�,��Ҫ���ڷ���dummy Node֮ǰ
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
//	/// ���ݽ�����, ��Ҫ����ڵ��ڴ�, ����ʧ��ʱ����false,���򷵻�true
//	bool EnQueue( const T & data )
//	{
//		Node_t* pNode = AllocNode();
//		if( pNode == NULL )
//			return false;
//
//		new (pNode) Node_t( data ); //placement new, ��pNode�ڴ�λ����dataΪ��������Node_t
//
//		m_token.Borrow();
//		if( (m_tail == m_head) && (m_tail.ptr == NULL) ) ///<��ʾ���ڶ�����һ���ձ�
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
