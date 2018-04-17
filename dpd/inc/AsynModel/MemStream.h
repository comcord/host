/****************************************************************************
* MemStream.h                       
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction ��װSession ������Ϣ���߼�
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
#define ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
#include "GMEmbedSmartPtr.h"
#include "ListMemAlloc.h"

namespace AsynModel
{
	#define MEM_STREAM_SECTION_NUMBER 16///<����MemStream�ڲ�ʹ�õ��ڴ�鼶��

	/**
	* �̳�RefCounterBaseģ�壬�������ü�������
	* memStream�Ĳ���ԭ��
	* ���ύ����ɶ˿�ʹ��,��ҪΪ�䴫��һ��buffer,��buffer����������û��ڱ���IO����ϣ����ɶ˿�
	* ���շ��͵������ϵ�����,��buffer����Ϊһ���������ڴ�,�����ڶ���..����ÿ���ύIO��ʱ����Ҫ
	* �ṩ��buffer,�������ǾͿ��Զ�����гػ�.�����û�Ҫ���͵����ݴ�С������,���Բ�����ͳһ�Ĺ̶�
	* �ڴ��С����Ĺ���������buffer.�ڸÿ���������ǲ����˷ֶι���ķ�ʽ.���û���Ҫ���͵�����Ϊ6�Ρ�
	*   64			128			256			512			1024		2048 ���ֽڣ�
	* 
	* MemStream��buff����3�������ݣ�
	* 1��IO�����������ݣ�
	* 2������������� Msgͷ���ݣ�
	* 3���û�����
	* ����IO������Ҫ�ڴ����ʱ�����û���������ǰ�����һЩ������Ϣ,������Щ��Ϣ�ǲ���Ҫ�ص����û��ģ�
	* �����ڸ���ɶ˿�ʹ�õ�ʱ����һ���������ڴ��,�������Ǳ��뽫�û����ݵ�ƫ��λ�ü�¼��������ʹ��
	* ���ڴ��ص����û���m_userDataOffset��
	*/
	class MemStream : public GMRefCounterBase<MemStream>,
							   public GMMemAlloc<MemStream,GMPrivateHeapMemAllocMethod>
	{
	public:
		/**
		* ���� ���캯�� �����ݴ����wantSize���ж���Ҫ��Ҫ������һ���ϣ�����öι涨��С���ڴ�飬�����������
		* @param wantSize U32���� ����������ڴ��С
		* @return ��
		*/
		MemStream(U32 wantSize);
		/**
		* ���� ��������
		* @param ��
		* @return ��
		*/
		~MemStream();
	
		U32 m_reallen;///<�����õ����ڴ泤�ȣ���Ҫ�ⲿ��д��
		void * m_buff;///< �û������buffָ��
		U32 m_wantSize;///<�������ڴ��С
		U32 m_userDataOffset;///<�û����ݵ�������ڴ���׵�ַ��ƫ��λ��

	public:
		static bool clearAllTheListMemAlloc();///<���memStream��ռ�õ��ڴ��
	private:
		void alloc();
		void deAlloc();
		U32 m_realSection;//����
		static int getSectionNumber(U32 size);
		static ListMemAlloc m_ListMemAlloc[MEM_STREAM_SECTION_NUMBER];
		static volatile long counter;
	};
	typedef GMEmbedSmartPtr<MemStream> SptrMemStream;
}
#endif //ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
