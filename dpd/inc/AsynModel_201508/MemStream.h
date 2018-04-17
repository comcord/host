/****************************************************************************
* MemStream.h                       
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 封装Session 发送消息的逻辑
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
#define ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
#include "GMEmbedSmartPtr.h"
#include "ListMemAlloc.h"

namespace AsynModel
{
	#define MEM_STREAM_SECTION_NUMBER 16///<定义MemStream内部使用的内存块级别

	/**
	* 继承RefCounterBase模板，具有引用计数能力
	* memStream的产生原因：
	* 在提交给完成端口使用,需要为其传入一个buffer,该buffer里面包含了用户在本次IO里面希望完成端口
	* 最终发送到网络上的数据,该buffer必须为一块连续的内存,必须在堆上..并且每次提交IO的时候都需要
	* 提供该buffer,这样我们就可以对其进行池化.由于用户要发送的数据大小不定长,所以不能用统一的固定
	* 内存大小对象的管理来管理buffer.在该框架里面我们采用了分段管理的方式.对用户需要发送的数据为6段。
	*   64			128			256			512			1024		2048 （字节）
	* 
	* MemStream中buff包含3部分数据：
	* 1：IO控制增加数据；
	* 2：框架增加数据 Msg头内容；
	* 3：用户数据
	* 由于IO层在需要在传输的时候在用户传输数据前面加上一些控制信息,并且这些信息是不需要回调给用户的，
	* 但是在给完成端口使用的时候是一个连续的内存块,所以我们必须将用户数据的偏移位置记录下来才能使用
	* 该内存块回调给用户（m_userDataOffset）
	*/
	class MemStream : public GMRefCounterBase<MemStream>,
							   public GMMemAlloc<MemStream,GMPrivateHeapMemAllocMethod>
	{
	public:
		/**
		* 描述 构造函数 将根据传入的wantSize，判断需要需要放在哪一端上，构造该段规定大小的内存块，放入段容器中
		* @param wantSize U32类型 期望构造的内存大小
		* @return 无
		*/
		MemStream(U32 wantSize);
		/**
		* 描述 析构函数
		* @param 无
		* @return 无
		*/
		~MemStream();
	
		U32 m_reallen;///<真正用到的内存长度（需要外部填写）
		void * m_buff;///< 用户申请的buff指针
		U32 m_wantSize;///<期望的内存大小
		U32 m_userDataOffset;///<用户数据的相对于内存块首地址的偏移位置

	public:
		static bool clearAllTheListMemAlloc();///<清空memStream所占用的内存块
	private:
		void alloc();
		void deAlloc();
		U32 m_realSection;//改名
		static int getSectionNumber(U32 size);
		static ListMemAlloc m_ListMemAlloc[MEM_STREAM_SECTION_NUMBER];
		static volatile long counter;
	};
	typedef GMEmbedSmartPtr<MemStream> SptrMemStream;
}
#endif //ASYNMODEL_MEMSTREAM_H_GOUMANG_2009
