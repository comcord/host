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
	 *	����LINKBUFF��pbuff���ڴ涼��AllocateBuff����
	 �����ٶ���DeAllocateBuff����,��������ʱ�򣬻���pbuff�ڴ�
	 */

namespace AsynModel
{
	struct LinkBuff 
		: public GMRefCounterBase<LinkBuff>
		//����ڴ�й¶����ע��, public GMMemAlloc<LinkBuff, GMPrivateHeapMemAllocMethod>
	{
	public:
		LinkBuff();
		~LinkBuff();

		char* const AllocateBuff(U32 len);
		void DeAllocateBuff();

		/*  ���л�������LINKBUFF����ָ����buff��
		*	����-1 ����buff����ΪNULL��
		*	-2����Ļ��峤�Ȳ���
		*	>=0 ���л�������ʵ�ʳ���
		*/
		int SeralizeTotalLinkBuff(void* buff, U32 leninput);
		U32 GetTotalLinkBuffLen(); //���������LINKBUFF���е�����Ч���ݳ���

		char* pbuff;//�ڴ���AllocateBuff���룬��DeAllocateBuff�ͷ�
		U32 bufflen;//pbuff������ڴ��С,���ɸı�

		U32 reallen;//pbuff��������Ч���ݵĴ�С
		GMEmbedSmartPtr<LinkBuff> nextLBSP;

	private:
		static GMPoolMemAllocMethod<LinkBuff> _memAlloc;//�ڴ������
		/*static volatile long counter;
		static volatile long remainCounter;*/
	};

	typedef GMEmbedSmartPtr<LinkBuff> LinkBuff_SmartPtr;
	#define NullLinkBuffSmartPtr LinkBuff_SmartPtr()
}
#endif
