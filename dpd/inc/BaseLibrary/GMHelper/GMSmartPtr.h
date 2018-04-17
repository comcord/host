#pragma once

#ifdef WIN32
#include <Windows.h>
#else // linux
#include "GMLock.h"
#include "GMautolock.h"
#endif


template<class object>
class GMSmartPtr;
/*
*模板类GMObjectContainer,线程安全
*GMObjectContainer封装对象，管理封装对象指针并计数的类，
*是多个GMSmartPtr对象共同管理的一个私有对象，多个GMSmartPtr
*对象共有同一个GMObjectContainer对象来实现管理指针的计数
*与释放。
*/
template<class object>
class GMObjectContainer
{
/*
*GMSmartPtr的私有对象，
*只有GMSmartPtr可对其进行操作
*/
friend class GMSmartPtr<object>;

private:
	static GMObjectContainer<object> NullGMObjectContainer;
	/*
	*为了防止隐式转换，构造函数加explicit
	*例如:GMObjectContainer obi = &object;
	*/
	explicit GMObjectContainer(object* pObject):m_Ptr(pObject),m_nCount(1){}

	~GMObjectContainer()
	{
		if( m_Ptr != NULL )
			delete m_Ptr;
	}
  
	/*
	*引用计数加一
	*/
	inline void AddRef()
	{
#ifdef WIN32
		InterlockedIncrement((long *)&m_nCount);
#else
		GMAutoLock<GMLock> _lock(&m_lock);
		m_nCount++;
#endif
		
	}
	/*
	*引用计数减一
	*/
	inline void Release()
	{
#ifdef WIN32
		if ( !(InterlockedDecrement( (long *)&m_nCount)))
		{
			delete this;
		}
#else
		m_lock.lock();
		if ( --m_nCount == 0)
		{
			m_lock.unlock();
			delete this;
		}
		else
		{
			m_lock.unlock();
		}
#endif

	}
	/*
	*返回object类型指针
	*/
	inline object*  Get()
	{
		return m_Ptr;
	}
	/*
	*为了处理const对象
	*/
	inline const object*  Get() const
	{
		return m_Ptr;
	}

private:
	/*
	*管理的object类型指针
	*/
	object* m_Ptr;
	/*
	*计数器
	*/
	volatile int m_nCount;

#ifndef WIN32
	GMLock m_lock;
#endif
};
template<class object>
GMObjectContainer<object> GMObjectContainer<object>::NullGMObjectContainer
= GMObjectContainer<object>(NULL);


/**
 *	智能指针
 */
template<class object>
class GMSmartPtr
{
public:
	//
	explicit GMSmartPtr(object* pObject)
	{
		m_pGMObjectContainer = new GMObjectContainer<object>(pObject);
	}
	//
	explicit GMSmartPtr()
	{
		m_pGMObjectContainer = NULL;
	}
	GMSmartPtr(const GMSmartPtr<object> &sp)
	{
		m_pGMObjectContainer = sp.GetGMObjectContainer();

		if (m_pGMObjectContainer != NULL)
		{
			m_pGMObjectContainer->AddRef();
		}
	}
	//
	~GMSmartPtr()
	{
		if( m_pGMObjectContainer != NULL )
		{
			m_pGMObjectContainer->Release(); 
		}
	}

	GMSmartPtr& operator = (const GMSmartPtr<object>& sp)
	{
		GMObjectContainer<object>* pTmpGMObjectContainer
			= sp.GetGMObjectContainer();
		if (pTmpGMObjectContainer)
		{
			pTmpGMObjectContainer->AddRef();
		}
		if (m_pGMObjectContainer != NULL) 
		{
			m_pGMObjectContainer->Release(); 
		}
		m_pGMObjectContainer = pTmpGMObjectContainer;
		return *this;
	}
	//
	object& operator* ()
	{
		if( m_pGMObjectContainer == NULL )
		{
			return *(GMObjectContainer<object>::NullGMObjectContainer.Get()); 
		}
		return *(m_pGMObjectContainer->Get());
	}
	//
	const object& operator* () const
	{
		if( m_pGMObjectContainer == NULL )
		{
			return *(GMObjectContainer<object>::NullGMObjectContainer.Get()); 
		}
		return *(m_pGMObjectContainer->Get());
	}
	//
	object* operator-> ()
	{
		if( m_pGMObjectContainer == NULL )
		{
			return NULL; 
		}
		return m_pGMObjectContainer->Get();
	}
	//
	const object* operator-> () const 
	{
		if( m_pGMObjectContainer == NULL )
		{
			return NULL; 
		}
		return m_pGMObjectContainer->Get();
	}
	//
	bool operator== (const GMSmartPtr& sp) const
	{
		return Get() == sp.Get();
	}
	//
	bool operator== (const object* pObject) const
	{
		return Get() == pObject;
	}
	//
	bool operator!= (const GMSmartPtr& sp) const
	{
		return !(*this == sp);
	}
	bool operator!= (const object* pObject) const
	{
		return !((*this).Get() == pObject);
	}
	//
	object* Get()
	{
		if( m_pGMObjectContainer == NULL )
		{
			return NULL; 
		}
		return m_pGMObjectContainer->Get();
		
	}
	//
	const object* Get() const
	{
		if( m_pGMObjectContainer == NULL )
		{
			return NULL; 
		}
		return m_pGMObjectContainer->Get();
	}
	//
	//void attach(object* pObject)
	//void Detach()
	//
	void Reset(object* pObject)
	{
		if (m_pGMObjectContainer != NULL) 
		{
			m_pGMObjectContainer->Release();
			m_pGMObjectContainer = NULL;
		}
		m_pGMObjectContainer = new GMObjectContainer<object>(pObject);
		//m_pGMObjectContainer->AddRef();
	}
private:
	GMObjectContainer<object>* GetGMObjectContainer() const
	{
		return m_pGMObjectContainer;
	}
private:
	GMObjectContainer<object>* m_pGMObjectContainer;
}; // GMSmartPtr
