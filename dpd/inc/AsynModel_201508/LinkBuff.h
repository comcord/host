/****************************************************************************
*                                                                           *
* LinkBuff.h --	constant and macros definitions	                    *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/
#ifndef _GF_LINKBUFF_H_2008_H_GOUMANG
#define _GF_LINKBUFF_H_2008_H_GOUMANG

#include "GMSmartPtr.h"
#include "GMMemAlloc.h"
#include "GMEmbedSmartPtr.h"

	/*
	 *	凡是LINKBUFF中pbuff的内存都由AllocateBuff分配
	 ，销毁都由DeAllocateBuff销毁,在析构的时候，回收pbuff内存
	 */

namespace AsynModel
{
	struct LinkBuff 
		: public GMRefCounterBase<LinkBuff>
		//检测内存泄露问题注掉, public GMMemAlloc<LinkBuff, GMPrivateHeapMemAllocMethod>
	{
	public:
		LinkBuff();
		~LinkBuff();

		char* const AllocateBuff(U32 len);
		void DeAllocateBuff();

		/*  序列化得整个LINKBUFF串到指定的buff中
		*	返回-1 代表buff传入为NULL，
		*	-2传入的缓冲长度不够
		*	>=0 序列化出来的实际长度
		*/
		int SeralizeTotalLinkBuff(void* buff, U32 leninput);
		U32 GetTotalLinkBuffLen(); //获得整个的LINKBUFF串中的总有效数据长度

		char* pbuff;//内存由AllocateBuff申请，由DeAllocateBuff释放
		U32 bufflen;//pbuff缓冲的内存大小,不可改变

		U32 reallen;//pbuff缓冲内有效数据的大小
		GMEmbedSmartPtr<LinkBuff> nextLBSP;

	private:
		static GMPoolMemAllocMethod<LinkBuff> _memAlloc;//内存分配器
		/*static volatile long counter;
		static volatile long remainCounter;*/
	};

	typedef GMEmbedSmartPtr<LinkBuff> LinkBuff_SmartPtr;
	#define NullLinkBuffSmartPtr LinkBuff_SmartPtr()
}
#endif
