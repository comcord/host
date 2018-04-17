#pragma once
//#include "stdafx.h"
//#include "commonPub.h"
#include "GMMemAlloc.h"


/**
 *	STL MAP MULITMAP的内存分配是适配器，不支持VECTOR 和LIST 
 使用方法：GMWidgetAlloctor<std::pair<key, value>, GMMemAllocMethod>
 */
template <class T, template<class> class GMMemAllocMethod=GMListMemAllocMethod>
class GMWidgetAlloctor 
{

private:
	static GMMemAllocMethod<T> _GMMemAlloc; //内存分配器

public:	
	typedef size_t size_type;
	typedef size_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef T value_type;
	typedef T& reference;

	template<typename U>
	struct rebind
	{
		typedef GMWidgetAlloctor<U, GMMemAllocMethod> other;
	};

	template<typename U>
	GMWidgetAlloctor(const GMWidgetAlloctor<U, GMMemAllocMethod>& )
	{
		//printf("T=%d,U=%d\n",sizeof(T),sizeof(U));
	}

	GMWidgetAlloctor()
	{}

	GMWidgetAlloctor(const GMWidgetAlloctor&)
	{}

	~GMWidgetAlloctor()
	{}

	pointer address(reference x) const
	{ 
		return &x; 
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	pointer allocate(size_type n, const_pointer = 0)
	{
		assert(n == 1);
		void* p=_GMMemAlloc.Allocate(n*sizeof(T));
		if (!p)
		{
			throw std::bad_alloc();
		}

		return static_cast<pointer>(p);
	}

	void deallocate(pointer p, size_type n)
	{
		_GMMemAlloc.DeAllocate(static_cast<void*>(p),n*sizeof(T));
	}

	size_type max_size() const 
	{
		return static_cast<size_type>(-1) / sizeof(value_type);
	}

	void construct(pointer p, const value_type& x)
	{
		new(p) value_type(x);
	}

	void destroy(pointer p) 
	{
		p->~value_type();
	}
};

template<class T,template<class> class GMMemAllocMethod>
	GMMemAllocMethod<T> GMWidgetAlloctor<T,GMMemAllocMethod >::_GMMemAlloc;
