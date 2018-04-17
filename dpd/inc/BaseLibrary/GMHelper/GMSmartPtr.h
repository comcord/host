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
*ģ����GMObjectContainer,�̰߳�ȫ
*GMObjectContainer��װ���󣬹����װ����ָ�벢�������࣬
*�Ƕ��GMSmartPtr����ͬ�����һ��˽�ж��󣬶��GMSmartPtr
*������ͬһ��GMObjectContainer������ʵ�ֹ���ָ��ļ���
*���ͷš�
*/
template<class object>
class GMObjectContainer
{
/*
*GMSmartPtr��˽�ж���
*ֻ��GMSmartPtr�ɶ�����в���
*/
friend class GMSmartPtr<object>;

private:
	static GMObjectContainer<object> NullGMObjectContainer;
	/*
	*Ϊ�˷�ֹ��ʽת�������캯����explicit
	*����:GMObjectContainer obi = &object;
	*/
	explicit GMObjectContainer(object* pObject):m_Ptr(pObject),m_nCount(1){}

	~GMObjectContainer()
	{
		if( m_Ptr != NULL )
			delete m_Ptr;
	}
  
	/*
	*���ü�����һ
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
	*���ü�����һ
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
	*����object����ָ��
	*/
	inline object*  Get()
	{
		return m_Ptr;
	}
	/*
	*Ϊ�˴���const����
	*/
	inline const object*  Get() const
	{
		return m_Ptr;
	}

private:
	/*
	*�����object����ָ��
	*/
	object* m_Ptr;
	/*
	*������
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
 *	����ָ��
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
