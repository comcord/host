#ifndef GM_SINGLE_TON_H_20085_KYG_GM
#define GM_SINGLE_TON_H_20085_KYG_GM

#include "GMLock.h"
#include "GMautolock.h"

/*
 *	单件类
	使用方法:
	class A : public SignleTon<A>
	{
		...
	};
	A * a= A::GetInst();
 */
template <class T>
class GMSingleTon
{
public:
	static T * GetInst() //会抛内存分配异常
	{
		if(!_inst)
		{
			GMAutoLock<GMLock> ao( &_lock );
			if(!_inst)
				_inst = new T; 
		}
		return _inst;
	}
	static void DestroyInst()
	{
		GMAutoLock<GMLock> ao( &_lock );
		if(_inst)
		{
			delete	_inst; 
			_inst = NULL;
		}
		
	}
	
	static T * Obj() //会抛内存分配异常
	{
		if(!_inst)
		{
			GMAutoLock<GMLock> ao( &_lock );
			if(!_inst)
				_inst = new T; 
		}
		return _inst;
	}

protected: 
	static GMLock _lock;
	static T * _inst;
};
template<class T>
	GMLock GMSingleTon<T>::_lock = GMLock();
template<class T>
	T * GMSingleTon<T>::_inst = NULL;
#endif
