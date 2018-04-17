#pragma once
#ifdef WIN32
#include <windows.h>
#include <new.h>
#else // linux
#include <cstdlib>
#endif

#include <assert.h>
#include "GMMemAlloc.h"

namespace AsynModel
{
	/*************************************SLIST**********************************/
	//¡¥±Ìƒ⁄¥Ê∑÷≈‰
	class ListMemAlloc
	{
	public:
		 ListMemAlloc();
		 void* Allocate(U32 );
		 void DeAllocate(void *p, U32);
		 void Destroy(U32 s);
		 void SetMaxAllocUnitNum(I32 num = MAXLONG );
//	private:
		//static  SLIST_HEADER  _slistheader;
#ifdef WIN32
		 void*	_slistheader;
		 bool _hasinit;
		//static SLIST_HEADER *  getListHeader();
		 void*  getListHeader();
		 volatile long m_counter;
		 bool m_isInit;
		 bool m_isSupportSLIST;
		 GMPrivateHeapMemAllocMethod<char> m_allocator;
#else
		 GMFixedAllocator* m_pFixAlloc;
		 GMFixedAllocator* getFixedMemAllocMethodInst(U32 s);
		 pthread_mutex_t m_mutex;
#endif

		 I32 m_maxAllocUnitNum;
		 GMLock			m_lock;
	};


}
