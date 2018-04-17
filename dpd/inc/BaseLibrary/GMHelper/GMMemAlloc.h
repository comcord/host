#ifndef GM_MEM_ALLOC_H 
#define GM_MEM_ALLOC_H
#ifdef MONITER_MEM_ALLOC
#include "MemoryAllocWatcher.h"
#endif
#include "GMTimerAssistant.h"

#ifdef WIN32
#include <windows.h>
#include <new.h>
#else // linux
#include <cstdlib>
#endif

#include <assert.h>
#include <list>
#include "GMautolock.h"
#include "GMLock.h"
#include "GMTokenLockFIFOQueue.h"


#ifndef ENABLE_LOKI_POOL_ALLOC
#define ENABLE_LOKI_POOL_ALLOC
#endif

#ifdef ENABLE_LOKI_POOL_ALLOC
#include "GMSmallObj.h"
#endif


//#define TEST_OUT(X) std::cout << X<< std::endl;
#define TEST_OUT(X)
#define MEM_PROTECT_FLAG_SEG1 0xc7d3a4b5
#define MEM_PROTECT_FLAG_SEG2 0xe2f968aa

#ifndef WIN32
#define MAXLONG     0x7fffffff  
#endif


/*
*	内存分配的基础类用法是继承该类，就可以实现重载NEW DELETE的方法
，如 class A :public MemAlloc<A, PrivateHeapMemAllocMethod> 就可以了
*/
template<class T, template<class> class GMMemAllocMethod >
class  GMMemAlloc : private GMMemAllocMethod<T>
{
public:
	static void * operator new(size_t s)
	{
		void* p = GMMemAllocMethod<T>::Allocate(s);
#ifdef MONITER_MEM_ALLOC
		if(NULL != p)
			MemAllocWatcher::inst()->recored_new(typeid(T).name(),static_cast<int>(s));
#endif	
		return p;
	}

	static void * operator new( size_t s,const std::nothrow_t& t) throw()
	{
		void* p = GMMemAllocMethod<T>::Allocate(s);
#ifdef MONITER_MEM_ALLOC
		if(NULL != p)
			MemAllocWatcher::inst()->recored_new(typeid(T).name(),static_cast<int>(s));
#endif	
		return p;
	}

	static void operator delete( void *p,size_t s)
	{
		if(NULL == p)
			return;

#ifdef MONITER_MEM_ALLOC	
		MemAllocWatcher::inst()->recored_del(typeid(T).name());
#endif
		GMMemAllocMethod<T>::DeAllocate(p,s);
	}

	static void operator delete(void* p,const std::nothrow_t& t) throw()
	{
		if(NULL == p)
			return;

#ifdef MONITER_MEM_ALLOC	
		MemAllocWatcher::inst()->recored_del(typeid(T).name());
#endif
		GMMemAllocMethod<T>::DeAllocate(p,sizeof(T));
	}

	static void DestroyMemAllocMethod()//销毁内存分配方法
	{
		GMMemAllocMethod<T>::Destroy();
	}
};

//全局堆内存分配
template<class T>
class GMGlobalHeapMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
};

//私有堆内存分配
template<class T>
class GMPrivateHeapMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
private:
#ifdef WIN32
	static GMLock m_init_lock;
	static HANDLE _privateHeap;
	static HANDLE getPrivateHeapInst();
#endif
};

//链表内存分配
template<class T>
class GMListMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
	static void SetMaxAllocUnitNum( int num = MAXLONG );
private:
	//static  SLIST_HEADER  _slistheader;
#ifdef WIN32
	static GMLock m_init_lock;
	static void*	_slistheader;
	volatile static bool _hasinit;
	//static SLIST_HEADER *  getListHeader();
	static void*  getListHeader();
	static volatile long m_counter;
	static bool m_isInit;
	static bool m_isSupportSLIST;
	static GMPrivateHeapMemAllocMethod<T> m_allocator;
#else
	static GMFixedAllocator* m_pFixAlloc;
	static GMFixedAllocator* getFixedMemAllocMethodInst();
	static pthread_mutex_t m_mutex;
#endif

	static int m_maxAllocUnitNum;
};

//无锁先入先出队列内存分配
template<class T>
class GMFIFOMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
private:
	static GMTokenLockFIFOQueue<unsigned int> m_FIFOAllocator;
	static bool m_isInit;
	static GMLock m_lock;
	static bool m_isValid;
	static GMPrivateHeapMemAllocMethod<T> m_privateHeapAllocator;
};

#ifdef ENABLE_LOKI_POOL_ALLOC//使用LOKI库的内存分配方法
//固定对象内存分配
template<class T>
class GMFixedMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
private:
	static GMFixedAllocator * _pSmallObjAlloc;
	static GMLock _lock;
};

//可变对象池内存分配
template<class T>
class GMPoolMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
private:
	static GMSmallObjAllocator * _pSmallObjAlloc;
	//static GMSmallObjAllocator * getPoolMemAllocMethodInst();
	static GMLock _lock;

};
#endif

/** 内存溢出检查器
1 内存经过delete后,然后再访问该块内存,会弹出错误
2 内存读写溢出后会弹出错误
注意：由于使用virtualalloc,申请的内存空间以页（4k）为单位，仅供调试中查找内存问题所用
不要用在其它方面（性能测试，正式版本）
liuju 20081205
*/
/*
template<class T>
class MemCheckAllocMethod
{
public:
static void* Allocate(size_t );
static void DeAllocate(void *p, size_t);
static void Destroy();
};
*/
/**
*	只做了前溢出和后溢出时候，在DELETE时候检查的内存校验 BY KEYG
*/
/*
template<class T>
class MemCheckAllocMethod2
{
public:
static void* Allocate(size_t );
static void DeAllocate(void *p, size_t);
static void Destroy();
};
*/

//**********************************************************************
//						定义
//						定义
//**********************************************************************

/*template<class T>
SLIST_HEADER ListMemAllocMethod<T>::_slistheader;*/

#ifdef WIN32
template<class T>
GMLock GMPrivateHeapMemAllocMethod<T>::m_init_lock;

template<class T>
HANDLE GMPrivateHeapMemAllocMethod<T>::_privateHeap = INVALID_HANDLE_VALUE;
#endif

#ifdef ENABLE_LOKI_POOL_ALLOC //LOKI库的功能
template<class T>
GMFixedAllocator* GMFixedMemAllocMethod<T>::_pSmallObjAlloc = NULL;
template<class T>
GMLock  GMFixedMemAllocMethod<T>::_lock;

template<class T>
GMSmallObjAllocator* GMPoolMemAllocMethod<T>::_pSmallObjAlloc = NULL;

template<class T>
GMLock GMPoolMemAllocMethod<T>::_lock;
#endif

//*************************************************************************
//							IMP
//							IMP
//*************************************************************************

///***************** MemCheck for Debug Region ****************************
/*
template<class T>
void *MemCheckAllocMethod<T>::Allocate(size_t s)
{
assert( s % 4 == 0);
//创建的页面个数,单位页 4096字节
int page_num = 0;
if ( s % 4096 == 0)
page_num = s / 4096;
else
page_num = (int) ( s / 4096 + 1 );
//偏移量
size_t offset = page_num * 4096 - s;
//在内存块的后面创建一个额外的保护页面,并且将页面的属性设置为不可读写
void *p = VirtualAlloc(NULL,page_num*4096+1,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
//定位最后一个保护页面的地址
void *pProtectPage = (char *)p + page_num*4096;
//设置最后一页为不可读写
DWORD outOldValue;
VirtualProtect(pProtectPage,4096,PAGE_NOACCESS,&outOldValue);
return (char *)p + offset;
}
template<class T>
void MemCheckAllocMethod<T>::DeAllocate(void *p,size_t s)
{
//找到VirtualAlloc时候的起始地址
void *startaddr = (char *)p - (size_t)p % 4096;  
VirtualFree(startaddr,0,MEM_RELEASE);
}
template<class T>
void MemCheckAllocMethod<T>::Destroy()
{
TEST_OUT("MemCheckAllocMethod::Destroy()")
}
template<class T>
void MemCheckAllocMethod2<T>::Destroy()
{
TEST_OUT("MemCheckAllocMethod2::Destroy()")
}
template<class T>
void * MemCheckAllocMethod2<T>::Allocate(size_t s)
{
PVOID  r = NULL;
TEST_OUT("MemCheckAllocMethod2::Allocate()")
if(s > 0)
{
r = ::VirtualAlloc(NULL,
s + sizeof(MEM_PROTECT_FLAG_SEG1) + sizeof(MEM_PROTECT_FLAG_SEG2)
,MEM_COMMIT,PAGE_READWRITE);//std::malloc(s);
if(r)
{
*(int * )((char*)r + s) = MEM_PROTECT_FLAG_SEG1;
*(int * )((char*)r + s + sizeof(MEM_PROTECT_FLAG_SEG1))
= MEM_PROTECT_FLAG_SEG2;
return r;
}
}
return NULL;
}
template<class T>
void MemCheckAllocMethod2<T>::DeAllocate(void *p, size_t s)
{
TEST_OUT("MemCheckAllocMethod2::DeAllocate()")
if(p)
{
if((*(int * )((char *)p + s) == MEM_PROTECT_FLAG_SEG1)
&& (*(int * )((char *)p + s + sizeof(MEM_PROTECT_FLAG_SEG1))
== MEM_PROTECT_FLAG_SEG2 ))
{
if(::VirtualFree(p, 0, MEM_RELEASE) != 0)
return ;
}
#ifdef _DEBUG
assert(false && "error in GlobalHeapMemAllocMethod::DeAllocate()");
#else
::DebugBreak();
#endif
}
}
*/
///************************** MemCheck for Debug Region ******************** 


template<class T>
void * GMGlobalHeapMemAllocMethod<T>::Allocate(size_t s)
{
	TEST_OUT("GlobalHeapMemAllocMethod::Allocate()")
		if(s > 0)
			return std::malloc(s);
		else
			return NULL;
}
template<class T>
void GMGlobalHeapMemAllocMethod<T>::DeAllocate(void *p, size_t)
{
	TEST_OUT("GlobalHeapMemAllocMethod::DeAllocate()")
		if(p)
			std::free(p);
}

template<class T>
void GMGlobalHeapMemAllocMethod<T>::Destroy()
{
	TEST_OUT("GlobalHeapMemAllocMethod::Destroy()")
}


template<class T>
void * GMPrivateHeapMemAllocMethod<T>::Allocate(size_t s)
{
#ifdef WIN32
	HANDLE ph = INVALID_HANDLE_VALUE;
	TEST_OUT("PrivateHeapMemAllocMethod::Allocate()");
	if((ph = getPrivateHeapInst(), ph != INVALID_HANDLE_VALUE) && s > 0 && ph != 0)
		return ::HeapAlloc(ph, 0, s);
	else
		return NULL;
#else
	return malloc(s);
#endif
}
template<class T>
void GMPrivateHeapMemAllocMethod<T>::DeAllocate(void *p, size_t s)
{
#ifdef WIN32
	HANDLE ph = INVALID_HANDLE_VALUE;
	TEST_OUT("PrivateHeapMemAllocMethod::DeAllocate()");
	if((ph = getPrivateHeapInst(), ph != INVALID_HANDLE_VALUE) && p != NULL)
		::HeapFree(ph, 0, p);
#else
	free(p);
#endif
}
template<class T>
void GMPrivateHeapMemAllocMethod<T>::Destroy()
{
	TEST_OUT("PrivateHeapMemAllocMethod::Destroy()")
#ifdef WIN32
		GMAutoLock<GMLock> a(&m_init_lock);
	if(_privateHeap != INVALID_HANDLE_VALUE)
	{
		::HeapDestroy(_privateHeap);
		_privateHeap = INVALID_HANDLE_VALUE;
	}
#endif
}

#ifdef WIN32
template<class T>
HANDLE GMPrivateHeapMemAllocMethod<T>::getPrivateHeapInst()
{
	if(_privateHeap == INVALID_HANDLE_VALUE)
	{
		GMAutoLock<GMLock> a(&m_init_lock);
		if(_privateHeap == INVALID_HANDLE_VALUE)
		{
			_privateHeap =  ::HeapCreate(0,0,0);
		}
	}
	return _privateHeap;
}
#endif
/*************************************SLIST**********************************/



template<class T>
int GMListMemAllocMethod<T>::m_maxAllocUnitNum = MAXLONG;

#ifdef WIN32

template<class T>
GMLock GMListMemAllocMethod<T>::m_init_lock;

template<class T>
volatile bool GMListMemAllocMethod<T>::_hasinit = false;

template<class T>
GMPrivateHeapMemAllocMethod<T> GMListMemAllocMethod<T>::m_allocator;

template<class T>
volatile long GMListMemAllocMethod<T>::m_counter = 0;

template<class T>
bool GMListMemAllocMethod<T>::m_isInit = false;

template<class T>
bool GMListMemAllocMethod<T>::m_isSupportSLIST = false;

template<class T>
void* GMListMemAllocMethod<T>::_slistheader = NULL;

template<class T>
void* GMListMemAllocMethod<T>::getListHeader()
{
	if(!_hasinit)
	{
		GMAutoLock<GMLock> a(&m_init_lock);
		if(!_hasinit)
		{
			_slistheader = (void*)new SLIST_HEADER();
			::InitializeSListHead((SLIST_HEADER*)_slistheader);
			_hasinit = true;
		}
	}
	return (void*)_slistheader;
}
#else
template<class T>
pthread_mutex_t GMListMemAllocMethod<T>::m_mutex = PTHREAD_MUTEX_INITIALIZER;


template<class T>
GMFixedAllocator* GMListMemAllocMethod<T>::m_pFixAlloc = NULL;

#endif




template<class T>
void * GMListMemAllocMethod<T>::Allocate(size_t s)
{
#ifdef WIN32
	if(!m_isInit)
	{
		GMAutoLock<GMLock> a(&m_init_lock);
		if(!m_isInit)
		{
			OSVERSIONINFO info;
			ZeroMemory(&info, sizeof(OSVERSIONINFO));
			info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

			GetVersionEx(&info);
			if((info.dwMajorVersion >= 5) && (info.dwMinorVersion > 0) )
				m_isSupportSLIST = true;

			m_isInit = true;
		}
	}

	char* pe = NULL;
	//assert(s == sizeof(T));
	assert(s % 4 == 0); //堆4字节对齐
	TEST_OUT("ListMemAllocMethod::Allocate()");

	if(s > 0)
	{
		if(m_isSupportSLIST)
		{
			SLIST_HEADER *pl = (SLIST_HEADER *)getListHeader();
			if(pl == NULL)
				return NULL;

			pe = (char*)InterlockedPopEntrySList(pl);
			if(pe == NULL)
				pe = (char*)m_allocator.Allocate(s + sizeof(SLIST_ENTRY));
			else
				::InterlockedDecrement((long *)&m_counter);

			return pe != NULL ? (pe+sizeof(SLIST_ENTRY)) : NULL;
		}
		else
		{
			pe = (char*)m_allocator.Allocate(s);
			return pe != NULL ? pe:NULL;
		}	
	}
#else
	assert(s == sizeof(T));
	assert(s % 4 == 0); //堆4字节对齐
	TEST_OUT("ListMemAllocMethod::Allocate()");

	void* pe = NULL;
	pthread_mutex_lock(&m_mutex);
	if (s > DEFAULT_CHUNK_SIZE / 2)
	{
		pe = malloc(s);
	}
	else
	{
		GMFixedAllocator *pi = NULL;
		if((pi = getFixedMemAllocMethodInst(),pi) && s > 0)
			pe = pi->Allocate();
	}
	pthread_mutex_unlock(&m_mutex);
	return pe != NULL ? pe: NULL;
#endif
	return NULL;
}

template<class T>
void GMListMemAllocMethod<T>::DeAllocate(void *p, size_t s)
{
	assert(sizeof(T) == s);
	assert(s % 4 == 0); //堆4字节对齐
	TEST_OUT("ListMemAllocMethod::DeAllocate()");

	if(p == NULL)
		return;
#ifdef WIN32

	if(m_isSupportSLIST)
	{
		SLIST_HEADER *pl = (SLIST_HEADER *)getListHeader();
		if(pl == NULL)
			return;

		if(m_counter > m_maxAllocUnitNum)
		{
			m_allocator.DeAllocate((char*)p-sizeof(SLIST_ENTRY),s+sizeof(SLIST_ENTRY));
			return;
		}

		::InterlockedPushEntrySList(pl, 
			(SLIST_ENTRY*)((char*)p-sizeof(SLIST_ENTRY)));

		::InterlockedIncrement((long *)&m_counter);
	}
	else
	{
		m_allocator.DeAllocate((char*)p,sizeof(T));
	}
#else
	pthread_mutex_lock(&m_mutex);
	if (s > DEFAULT_CHUNK_SIZE / 2)
	{
		free(p);
	}
	else
	{
		GMFixedAllocator *pi = NULL;
		if((pi = getFixedMemAllocMethodInst(),pi) && s > 0)
			pi->Deallocate(p);
	}
	pthread_mutex_unlock(&m_mutex);
#endif
}

template<class T>
void GMListMemAllocMethod<T>::Destroy()
{
	TEST_OUT("ListMemAllocMethod::Destroy()");
#ifdef WIN32
	char * pe;

	GMAutoLock<GMLock> a(&m_init_lock);
	if(m_isInit && m_isSupportSLIST && _hasinit)
	{
		do{
			pe = (char*)InterlockedPopEntrySList((SLIST_HEADER*)_slistheader);
			if(pe == NULL) 
				break;
			m_allocator.DeAllocate(pe,sizeof(T)+sizeof(SLIST_ENTRY));
		}while(true);

		InterlockedFlushSList((SLIST_HEADER*)_slistheader);
		_hasinit = false;
	}	
#else
	if(m_pFixAlloc)
	{
		delete m_pFixAlloc;
		m_pFixAlloc = NULL;
	}
#endif
}

#ifndef WIN32
template<class T>
GMFixedAllocator* GMListMemAllocMethod<T>::getFixedMemAllocMethodInst()
{
	if(!m_pFixAlloc)
	{
		m_pFixAlloc =  new(std::nothrow) GMFixedAllocator(sizeof(T));
	}
	return m_pFixAlloc;
}
#endif

template<class T>
void GMListMemAllocMethod<T>::SetMaxAllocUnitNum(int num)
{
	m_maxAllocUnitNum = num;
}


#ifdef ENABLE_LOKI_POOL_ALLOC//使用LOKI库

template<class T>
void * GMFixedMemAllocMethod<T>::Allocate(size_t s)
{
	GMAutoLock<GMLock> al(&_lock);

	if(!_pSmallObjAlloc)
		 _pSmallObjAlloc =  new(std::nothrow) GMFixedAllocator(sizeof(T));
		
	if(_pSmallObjAlloc && s > 0)
		return _pSmallObjAlloc->Allocate();
	else
		return NULL;
}

template<class T>
void GMFixedMemAllocMethod<T>::DeAllocate(void *p, size_t s)
{
	GMAutoLock<GMLock> al(&_lock);

	if(_pSmallObjAlloc == NULL)
			return;

	if(_pSmallObjAlloc && p != NULL && s > 0)
		_pSmallObjAlloc->Deallocate(p);
	else
		assert(0);
}

template<class T>
void GMFixedMemAllocMethod<T>::Destroy()
{
	GMAutoLock<GMLock> a(&_lock);
	if(_pSmallObjAlloc)
	{
		delete _pSmallObjAlloc;
		_pSmallObjAlloc = NULL;
	}
}

template<class T>
void * GMPoolMemAllocMethod<T>::Allocate(size_t s)
{
	GMAutoLock<GMLock> al(&_lock);
	if(!_pSmallObjAlloc)
		_pSmallObjAlloc =  new(std::nothrow) GMSmallObjAllocator(UCHAR_MAX,MAX_SMALL_OBJECT_SIZE);
	
	if(_pSmallObjAlloc && s > 0)
		return _pSmallObjAlloc->Allocate(s);
	else
		return NULL;
}

template<class T>
void GMPoolMemAllocMethod<T>::DeAllocate(void *p, size_t s)
{
	GMAutoLock<GMLock> al(&_lock);
	if(!_pSmallObjAlloc)
		return;
		
	if(_pSmallObjAlloc && p != NULL && s > 0)
		_pSmallObjAlloc->Deallocate(p,s);
	else
		assert(0);
}

template<class T>
void GMPoolMemAllocMethod<T>::Destroy()
{
	GMAutoLock<GMLock> a(&_lock);
	if(_pSmallObjAlloc)
	{
		delete _pSmallObjAlloc;
		_pSmallObjAlloc = NULL;
	}
}


/////////////////////////////////////////////FIFOMemAllocMethod/////////////////////////////////////
template<class T>
GMTokenLockFIFOQueue<unsigned int> GMFIFOMemAllocMethod<T>::m_FIFOAllocator;

template<class T>
bool GMFIFOMemAllocMethod<T>::m_isInit = false;

template<class T>
GMLock GMFIFOMemAllocMethod<T>::m_lock;

template<class T>
bool GMFIFOMemAllocMethod<T>::m_isValid = false;

template<class T>
GMPrivateHeapMemAllocMethod<T> GMFIFOMemAllocMethod<T>::m_privateHeapAllocator;


template<class T>
void* GMFIFOMemAllocMethod<T>::Allocate(size_t s)
{
	if(!m_isInit)
	{
		GMAutoLock<GMLock> al(&m_lock);
		if(!m_isInit)
		{
			m_isValid = m_FIFOAllocator.Initialize();
			m_isInit  = true;
		}
	}

	unsigned int p = 0; 
	if(m_isValid)
		if(m_FIFOAllocator.DeQueue(p))
			return (void*)p;

	return m_privateHeapAllocator.Allocate(s);
}

template<class T>
void GMFIFOMemAllocMethod<T>::DeAllocate(void *p, size_t s)
{
	if(p == NULL)
		return;

	if(!m_isInit)
	{
		GMAutoLock<GMLock> al(&m_lock);
		if(m_isInit)
		{
			m_isValid = m_FIFOAllocator.Initialize();
			m_isInit  = true;
		}
	}

	if(m_isValid)
		m_FIFOAllocator.EnQueue((unsigned long)p);
	else
		m_privateHeapAllocator.DeAllocate(p,s);

}

template<class T>
void GMFIFOMemAllocMethod<T>::Destroy()
{
	TEST_OUT("FIFOMemAllocMethod::Destroy()");
	char * pe;

	GMAutoLock<GMLock> a(&m_lock);
	if(m_isValid)
	{
		unsigned int pointer =  0;
		while(m_FIFOAllocator.DeQueue(pointer))
		{
			void* p = (void*)pointer;
			if(p)
				m_privateHeapAllocator.DeAllocate(p,sizeof(T));
		}
		m_isValid = false;
		m_isInit  = false;
	}	
}

struct listItem
{
	listItem():perIO(0),lastRepayTime(GMTimerAssistant::GetSysCurrentTime()){}
	void*	perIO;
	U64	lastRepayTime;
};

//链表内存分配
template<class T>
class CheckMemAllocMethod
{
public:
	static void* Allocate(size_t );
	static void DeAllocate(void *p, size_t);
	static void Destroy();
	static void SetMaxAllocUnitNum( int num = MAXLONG );
private:
	static GMLock					m_lock;
	static std::list<listItem>		m_list;
	
};
template<class T>
GMLock CheckMemAllocMethod<T>::m_lock;

template<class T>
std::list<listItem> CheckMemAllocMethod<T>::m_list;

template<class T>
void* CheckMemAllocMethod<T>::Allocate(size_t s)
{
	GMAutoLock<GMLock> a(&m_lock);
	std::list<listItem>::iterator iter = m_list.begin();
	if(iter != m_list.end())
	{
		void* p = (*iter).perIO;
		if(GMTimerAssistant::GetSysCurrentTime() - (*iter).lastRepayTime > 3* 1000)
		{
			m_list.pop_front();
			return p;
		}
	}

	return std::malloc(s);
}

template<class T>
void CheckMemAllocMethod<T>::DeAllocate(void *p, size_t)
{
	GMAutoLock<GMLock> a(&m_lock);
	listItem item;
	item.perIO = p;
	m_list.push_back(item);

}

template<class T>
void CheckMemAllocMethod<T>::Destroy(){}

template<class T>
void CheckMemAllocMethod<T>::SetMaxAllocUnitNum( int num/* = MAXLONG*/ ){}


#endif // ENABLE_LOKI_POOL_ALLOC

#endif
