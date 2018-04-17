/****************************************************************************                                                                          
* ISender.h                       
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 所有Sender都需继承的接口
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYN_ISENDER_H_GOUMANG_2008
#define ASYN_ISENDER_H_GOUMANG_2008

#include "GMEmbedSmartPtr.h"
namespace AsynModel
{
	/**
	*所谓Sender是一种通信能力的封装，系统默认提供3种Sender，TCPSender，UDPSender与LocalSender，
	*分别封装TCP发送，UDP发送与本地消息发送行为。用户可定义自己的Sender，继承基本的ISender接口
	*来封装自己的通信行为（邮件，无线传输等），通过Session的PostMsg来传入自定义Sender，异步模型
	*通过ISender接口来调用用户的通信行为与目的地实现通信
	*注：自定义Sender本次异步模型版本还没有支持
	*/
	class ISender
	{
	public:
		/**
		*定义支持的Sender的类型
		*/
		enum SenderType
		{
			ASYNLOCALSENDER = 0,///<本地Sender
			ASYNUDPSENDER,///<UDPSender
			ASYNTCPSENDER///<TCPSender
		};
		/**
		* 描述 异步发送接口，用户传入要发送的数据与长度，用户自定义Sender可重载实现该接口
		* @param buff char类型指针 用户数据首地址
		* @param len  unsigned int类型 用户数据长度
		* @return int类型 0：成功，负数失败
		*/
		virtual int AsynSend(char* buff,unsigned int len) = 0;
		/**
		* 描述 获取本地的IPPort，本机序 需要UDP，TCP的派生类实现
		* @param localIP unsigned long类型传出参数 
		* @param localPort unsigned short类型传出参数 
		* @return 无
		*/
		virtual void GetLocalIPPort(unsigned long &localIP,unsigned short &localPort)const = 0;
		/**
		* 描述 获取远程的IPPort，本机序 需要UDP，TCP的派生类实现
		* @param remoteIP unsigned long类型传出参数 
		* @param remotePort unsigned short类型传出参数 
		* @return 无
		*/
		virtual void GetRemoteIPPort(unsigned long &remoteIP,unsigned short &remotePort)const = 0;
		/**
		* 描述 引用计数器加一
		* @param 无
		* @return 无
		*/
		virtual void AddRef()  = 0;
		/**
		* 描述 引用计数器减一
		* @param 无
		* @return 无
		*/
		virtual void Release() = 0;
		/**
		* 描述 获取Sender的类型
		* @see SenderType
		* @param 无
		* @return int类型 值为类型代码
		*/
		virtual int  GetType()const  = 0;
		/**
		* 描述 判断Sender是否有效
		* @param 无
		* @return bool类型 true：成功，false：失败
		*/
		virtual bool IsValid()const = 0;
	};

typedef GMEmbedSmartPtr<ISender> SptrISender;
#define NULLSptrISender SptrISender(NULL) 
}
#endif //ASYN_ISENDER_H_GOUMANG_2008
