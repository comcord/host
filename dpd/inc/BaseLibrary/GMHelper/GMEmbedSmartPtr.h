#pragma once
#ifdef WIN32
#include <Windows.h>
#else
#include "GMLock.h"
#include "GMautolock.h"
#endif

#ifdef USE_ATOMIC
#include <cstdatomic>
#endif
/**使用方法
*  class A : public GMRefCounterBase<A>
{
}
*/
template <class T> 
class GMRefCounterBase
{
public:
	GMRefCounterBase():m_nCount(0){}

	~GMRefCounterBase(){}
	
	inline void AddRef()
	{
#ifdef WIN32
		InterlockedIncrement((long *)&m_nCount);
#else
	
	#ifdef USE_ATOMIC
		++m_nCount;
	#else
		GMAutoLock<GMLock> al(&m_lock);
		++m_nCount;
	#endif
		
#endif
	}

	inline void Release()
	{
#ifdef WIN32
		if ( !(InterlockedDecrement( (long *)&m_nCount)))
		{
			delete (T *)(this);
		}
#else
	#ifdef USE_ATOMIC
		if( 0 ==--m_nCount)
			delete (T*)(this);
	#else
		m_lock.lock();
		if( 0 ==--m_nCount)
		{
			m_lock.unlock();
			delete (T*)(this);
		}
		else
		{
			m_lock.unlock();
		}

	#endif
#endif
	}
	
#ifdef WIN32
	volatile long m_nCount;
#else
	#ifdef USE_ATOMIC
	std::atomic_long m_nCount;
	#else
	volatile int m_nCount;
	GMLock m_lock;
	#endif
#endif	
};

template<class object>
class GMEmbedSmartPtr
{
public:
	explicit GMEmbedSmartPtr(object* pObject)
	{
		m_pObject = pObject;
		if ( m_pObject != NULL )
		{
			pObject->AddRef();	
		}
	}
	
	explicit GMEmbedSmartPtr()
	{
		m_pObject = NULL;
	}

	GMEmbedSmartPtr(const GMEmbedSmartPtr<object> &GMSmartPtr)
	{
		m_pObject = GMSmartPtr.Get();

		if (m_pObject != NULL)
		{
			m_pObject->AddRef();
		}
	}
	//
	~GMEmbedSmartPtr()
	{
		if( m_pObject != NULL )
		{
			m_pObject->Release(); 
		}
	}

	GMEmbedSmartPtr& operator = (const GMEmbedSmartPtr<object>& GMSmartPtr)
	{
		object * pTmpObject	= GMSmartPtr.Get();
		
		if (pTmpObject != NULL)
		{
			pTmpObject->AddRef();
		}
		if (m_pObject != NULL) 
		{
			m_pObject->Release(); 
		}
		m_pObject = pTmpObject;
		return *this;
	}
	//
	object& operator* ()
	{
		return *(m_pObject);
	}
	//
	const object& operator* () const
	{
		return *(m_pObject);
	}
	//
	object* operator-> ()
	{
		return m_pObject;
	}
	//
	const object* operator-> () const 
	{
		return m_pObject;
	}
	//
	bool operator== (const GMEmbedSmartPtr& GMSmartPtr) const
	{
		return m_pObject == GMSmartPtr.Get();
	}
	//
	bool operator== (const object* pObject) const
	{
		return m_pObject == pObject;
	}
	//
	bool operator!= (const GMEmbedSmartPtr& GMSmartPtr) const
	{
		return !(*this == GMSmartPtr);
	}
	bool operator!= (const object* pObject) const
	{
		return !((*this).Get() == pObject);
	}
	//
	object* Get()
	{
		return m_pObject;
	}
	//
	object* Get() const
	{
		return m_pObject;
	}
	void Reset(object* pObject)
	{
		if(pObject != NULL)
		{
			pObject->AddRef();
		}
		if (m_pObject != NULL) 
		{
			m_pObject->Release();
			m_pObject = NULL;
		}	
		m_pObject = pObject;		
	}
private:
	object * m_pObject;
};
