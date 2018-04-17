/****************************************************************************
* TCPSender.h                       
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 封装Session 发送消息的逻辑
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef GF_POSTMSG_BASESESSION_H_2008_GOUMANG
#define GF_POSTMSG_BASESESSION_H_2008_GOUMANG

#include "AsynResultDefine.h"
#include "AsynModelDataDefine.h"
#include "LinkBuff.h"
#include "ConnID.h"
#include "ISender.h"

#define SENMSGDEFAULTTIMROUT	0xFFFFFFFF

namespace AsynModel
{
	class PostMsgContext;
	class AsynManager;
	class Session;

	/**
	* Session 继承次类，封装发送消息的逻辑
	*/
	class PostMsgBaseSession
	{
	public:
		/**
		* 描述 获取子类Session指针，在发送消息时需要知道子类Session的指针以封装Session智能指针
		*      由子类Session实现
		* @param 无
		* @return Session指针类型 
		*/
		virtual Session* GetSession() = 0;
		/**
		* 描述 检查AsynManager是否处于运行状态，是则返回其指针
		* @param result AsynResultRet类型传出参数 告知成功或失败原因
		* @return AsynManager类型指针
		*/
		AsynManager* CheckAsynModelStatus(AsynResultRet& result);

		/**
		* 描述 不使用Sender的本地消息发送 
		* @param destSid U32类型 目标Session的ID号
		* @param msgid  U32类型 发送的msg ID号
		* @param msgData char指针类型 发送的消息内容首地址
		* @param dataLen U32类型 发送消息的内容长度
		* @param isShareMsgData bool类型 默认为false 指明是否需要共享数据
		* （共享则直接使用用户传入的msgData数据 用户在传输期间不能删除msgData，不共享则将用户msgData复制使用 用户在调用该函数后可以自由删除msgData）
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
		AsynResultRet PostMsg(U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isShareMsgData = false);

		AsynResultRet PostMsg(U32 destSid,U32 msgid,
							   UserDataInfo* msgData);
		#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender, U32 destSid, U32 msgid,
								UserDataInfo* msgData);
		#else
		AsynResultRet PostMsg(ISender* sender, U32 destSid, U32 msgid,
								UserDataInfo* msgData);	
		#endif
		/**
		* 描述 使用Sender的本地消息发送
		* @param sender SptrISender智能指针类型 
		* @param destSid U32类型 目标Session的ID号
		* @param msgid  U32类型 发送的msg ID号
		* @param isShareMsgData bool类型 默认为false 指明是否需要共享数据
		* @param msgData char指针类型 发送的消息内容首地址
		* @param dataLen U32类型 发送消息的内容长度
		* （共享则直接使用用户传入的msgData数据 用户在传输期间不能删除msgData，不共享则将用户msgData复制使用 用户在调用该函数后可以自由删除msgData） 
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender, U32 destSid, U32 msgid,bool isShareMsgData,
							  char* msgData,U32 dataLen);
#else
		AsynResultRet PostMsg(ISender* sender, U32 destSid, U32 msgid,bool isShareMsgData,
							  char* msgData, U32 dataLen);
#endif


		/**
		* 描述 远程消息发送 使用ConnID方式，并且用户传入的msgData是char*类型
		* @param connID ConnID类型 连接ID
		* @param destSid U32类型 目标的Session ID
		* @param msgid U32类型 消息ID
		* @param msgData char类型指针 消息内容首地址
		* @param dataLen U32类型 消息内容长度
		* @param isReliable bool类型 是否可靠
		* @param timeout U32类型 超时时间，发送超出该时间将会产生超时事件回调用户（用户订阅了发送失败）.如果采用默认的SENMSGDEFAULTTIMROUT时间对于UDP可靠发送，系统会最多重发5次，大约的时间为3秒
		* @param subscribeMsg SubScribeMsgDefine类型 回调事件订阅，默认不订阅
		* @see SubScribeMsgDefine类型定义
		* @param protocal Protocol类型 发送的协议类型
		* @see Protocol类型定义
		* @param userPara UserDataInfo类型指针 用户自定义的回调参数（如果Session销毁将会涉及到释放问题）
		* @param localIP unsigned long类型 指定发送的本地IP（本机序）
		* @param localPort unsigned short类型 指定发送的本地Port（本机序）
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
		AsynResultRet PostMsg(ConnID& connID,U32 destSid,U32 msgid, 
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  Protocol protocal = ANYONE,
							  UserDataInfo* userPara = NULL,
							  unsigned long localIP = 0,
							  unsigned short localPort = 0);
		/**
		* 描述 远程消息发送 使用ConnID方式，并且用户传入的msgData是LinkBuff_SmartPtr类型
		* @param connID ConnID类型 连接ID
		* @param destSid U32类型 目标的Session ID
		* @param msgid U32类型 消息ID
		* @param msgData LinkBuff_SmartPtr类型智能指针 消息内容使用Linkbuff封装
		* @param isReliable bool类型 是否可靠
		* @param timeout U32类型 超时时间，发送超出该时间将会产生超时事件回调用户（用户订阅了发送失败）..如果采用默认的SENMSGDEFAULTTIMROUT时间对于UDP可靠发送，系统会最多重发5次，大约的时间为3秒
		* @param subscribeMsg SubScribeMsgDefine类型 回调事件订阅，默认不订阅
		* @see SubScribeMsgDefine类型定义
		* @param protocal Protocol类型 发送的协议类型
		* @see Protocol类型定义
		* @param userPara UserDataInfo类型指针 用户自定义的回调参数（如果Session销毁将会涉及到释放问题）
		* @param localIP unsigned long类型 指定发送的本地IP（本机序）
		* @param localPort unsigned short类型 指定发送的本地Port（本机序）
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
		AsynResultRet PostMsg(ConnID& connID, U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData, 
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								Protocol protocal = ANYONE,
								UserDataInfo* userPara = NULL,
								unsigned long localIP = 0,
								unsigned short localPort = 0);

	
		/**
		* 描述 远程消息发送 使用Sender方式，并且用户传入的msgData是char*类型
		* @param sender SptrISender智能指针 
		* @param destSid U32类型 目标的Session ID
		* @param msgid U32类型 消息ID
		* @param msgData char类型指针 消息内容首地址
		* @param dataLen U32类型 消息内容长度
		* @param isReliable bool类型 是否可靠
		* @param timeout U32类型 超时时间，发送超出该时间将会产生超时事件回调用户（用户订阅了发送失败）..如果采用默认的SENMSGDEFAULTTIMROUT时间对于UDP可靠发送，系统会最多重发5次，大约的时间为3秒
		* @param subscribeMsg SubScribeMsgDefine类型 回调事件订阅，默认不订阅
		* @see SubScribeMsgDefine类型定义
		* @see Protocol类型定义
		* @param userPara UserDataInfo类型指针 用户自定义的回调参数（如果Session销毁将会涉及到释放问题）
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender,U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  UserDataInfo* userPara = NULL);
#else
		AsynResultRet PostMsg(ISender* sender,U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  UserDataInfo* userPara = NULL);
#endif
		/**
		* 描述 远程消息发送 使用sender方式，并且用户传入的msgData是LinkBuff_SmartPtr类型
		* @param sender SptrISender智能指针 
		* @param destSid U32类型 目标的Session ID
		* @param msgid U32类型 消息ID
		* @param msgData LinkBuff_SmartPtr类型智能指针 消息内容使用Linkbuff封装
		* @param isReliable bool类型 是否可靠
		* @param timeout U32类型 超时时间，发送超出该时间将会产生超时事件回调用户（用户订阅了发送失败）..如果采用默认的SENMSGDEFAULTTIMROUT时间对于UDP可靠发送，系统会最多重发5次，大约的时间为3秒
		* @param subscribeMsg SubScribeMsgDefine类型 回调事件订阅，默认不订阅
		* @see SubScribeMsgDefine类型定义
		* @see Protocol类型定义
		* @param userPara UserDataInfo类型指针 用户自定义的回调参数（如果Session销毁将会涉及到释放问题）
		* @return AsynResultRet类型 表明成功或则失败原因
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender,U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData,
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								UserDataInfo* userPara = NULL);
#else
		AsynResultRet PostMsg(ISender* sender,U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData,
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								UserDataInfo* userPara = NULL);
#endif
		AsynResultRet SendDataStream(ConnID& connID,
									 uint32 destSid,
									 uint32 msgid,
									 LinkBuff_SmartPtr& msgData,
									 unsigned long localIP,
									 unsigned short localPort,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//新增接口，只支持TCP发送

//#ifdef WIN32
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet SendDataStream(SptrISender& sender,
									 uint32 destSid,
									 uint32 msgid,
									 LinkBuff_SmartPtr& msgData,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//新增接口，只支持TCP发送
#else
		AsynResultRet SendDataStream(ISender* sender,
									 uint32 destSid,
									 uint32 msgid,
								     LinkBuff_SmartPtr& msgData,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//新增接口，只支持TCP发送
#endif
//#endif
	
	private:

		AsynResultRet PostMsg(SptrISender& destSession,PostMsgContext* context);

	};
}

#endif //GF_POSTMSG_BASESESSION_H_2008_GOUMANG
