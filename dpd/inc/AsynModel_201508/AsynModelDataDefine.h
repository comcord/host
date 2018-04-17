/****************************************************************************                                                                          
* AsynModelDataDefine.h                       
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 定义异步模型的Event，EventItem， 协议，回执类型与结果，Msg映射等用户
*          需要感知的结构
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYNMODEL_ASYNMODELDATADEFINE_H_GOUMANG_2009
#define ASYNMODEL_ASYNMODELDATADEFINE_H_GOUMANG_2009
//#include "stdafx.h"
#include "GMUInt.h"
#include "GMEmbedSmartPtr.h"
#include "ISender.h"
#include "GMCustomTimer.h"

namespace AsynModel
{
	/**
	* 异步模型内部流转事件定义
	*/
	struct Event
	{
		U32	destSid;	///<目标放的会话SID
		U32	eventid;    ///<EventID中定义的类型均为框架定义类型，其它为用户自定义类型
#ifdef X64
		U64	wPrama;
#else
		U32	wPrama;     ///<一般放入Msg指针
#endif
#ifdef X64
		U64		lPrama;
#else
		U32		lPrama;     ///<扩展参数
#endif
		uint64	startPostTime;
		uint64	inArrayTime;
		uint64	startPopTime;
		uint64	outArrayTime;
		//U32	postThreadID;
		//long	ID;
		//std::string  postLog;
		//U32	       socketID;
		/**
		* 描述 构造函数 1
		* @param dsid U32类型 目标Session ID号
		* @param eid  U32类型 事件ID
		* @param wp   wPrama参数
		* @param lp   lPrama参数
		* @return 无
		*/
#ifdef X64
		Event(U32 dsid,U32 eid,U64 wp,U64 lp):
#else
		Event(U32 dsid,U32 eid,U32 wp,U32 lp):
#endif
			destSid(dsid),eventid(eid),wPrama(wp),
			lPrama(lp),startPostTime(GMTimerAssistant::GetSysCurrentTime())
		{}
		/**
		* 描述 构造函数 2
		* @param 无
		* @return 无
		*/
		Event()
			:destSid(0),eventid(0),wPrama(0),lPrama(0),startPostTime(GMTimerAssistant::GetSysCurrentTime())
		{}

	};
	/**
	*协议类型定义 针对TCP，是否需要通过Session为用户提供Listen与accept接口
	*/
	enum Protocol
	{
		UDP = 1,///<UDP协议
		TCP ,   ///<TCP协议
		ANYONE  ///<任意一种协议
	};
	
	/**
	*回执类型定义
	*/
	enum ReceptType
	{
		TCP_SEND = 0,///<TCP发送
		UDP_SEND ///<UDP发送
	};
	/**
	*	回执结果定义
	*/
	enum ReceptResult
	{
		RECEPT_SUCCESS = 0,///<用户操作成功
		RECEPT_COMPLETE_PORT_FAILURE,///<完成端口发生错误，操作失败
		RECEPT_SOCKET_ERROR,///<Socket错误
		RECEPT_TCP_CONNECT_FAILURE,///<TCP连接失败
		RECEPT_TIMEOUT,///<超时
		RECEPT_TIMEOUT_SENDING,///<正处在发送中数据已超时，系统将会在发送成功或失败后再次回执用户
		RECEPT_SET_TIMER_ERROR,///<设置timer失败
		RECEPT_TCP_PACKAGE_ERROR,///<TCP打包错误，可能传入的包大小过大或者包内容校验出错
		RECEPT_ASYNMODEL_DESTROYED,///<异步模型已经被销毁
		RECEPT_UNKNOW_ERROR, ///<未知错误
		RECEPT_MEM_ALLOC_FAILED ///内存分配错误
	};
	/**
	* 用户回执消息订阅类型
	*/
	enum SubScribeMsgDefine
	{
		POSTMSG_NO_SUBSCRIBE = 0,///<不订阅
		POSTMSG_SUCCESS = 1,///<订阅发送成功
		POSTMSG_FAILURE   = 2,///<订阅发送失败
		POSTMSG_ALL     = 3 ///<订阅所有回执信息
	};
	/**
	*Session 等待处理消息队列中的元素结构体定义
	*/
	struct EventItem
	{
		Event       m_event;///<发给Session的事件
		SptrISender m_sender;///<回复消息给目的Session的快捷通道（Sender）
		/**
		* 描述 构造函数 1
		* @param event Event类型
		* @see Event定义
		* @param sender SptrISender智能指针类型
		* @see  SptrISender定义
		* @return 无
		*/
		EventItem(Event event,SptrISender sender):
				 m_event(event),m_sender(sender)
		{}
		/**
		* 描述 构造函数 2
		* @param 无
		* @return 无
		*/
		EventItem()
			:m_sender(NULL)
		{}
	};

	/**
	* Session 用户参数数据结构基类定义，提供虚析构由派生类重写加入内部资源的释放代码
	*/
	struct UserDataInfo:GMRefCounterBase<UserDataInfo>
	{
	public:
		UserDataInfo()
		{
			AddRef();
		}
		/**
		* 描述 虚析构由派生类重写加入内部资源的释放代码
		* @param 无
		* @return 无
		*/
		virtual ~UserDataInfo(){}
	};

typedef GMEmbedSmartPtr<UserDataInfo> Sptr_UserDataInfo;
	/**
	* Session 消息映射定义，将MsgID与派生Session中定义的处理消息函数建立关联，根据不同的MsgID调用不同的处理函数
	*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
	#define BEGIN_MSG_MAP(theClass)\
		public:\
		virtual int OnMsgArrived(U32 msgid,   \
	                         const char* buff,     \
						     U32 len,     \
							 U32 srcsid,  \
							 const AsynModel::SptrISender& sender)\
	{\

	#define MSG_HANDLER(msgID, func)\
		if(msgid == msgID)\
	{\
		return func(buff,len,srcsid,sender);\
	}

	#define END_MSG_MAP()\
		return 0;\
	}

#else

	#define BEGIN_MSG_MAP(theClass)\
		public:\
		virtual int OnMsgArrived(U32 msgid,   \
		const char* buff,     \
		U32 len,     \
		U32 srcsid,  \
		const AsynModel::ISender* sender)\
	{\

	#define MSG_HANDLER(msgID, func)\
		if(msgid == msgID)\
	{\
		return func(buff,len,srcsid,sender);\
	}

	#define END_MSG_MAP()\
		return 0;\
	}

#endif

	/**
	* Session timer映射定义，将timerID与派生Session中定义的timer处理函数建立关联，根据不同的timerID调用不同的处理函数
	*/
	#define BEGIN_TIMER_MAP(theClass)\
		public:\
		virtual int OnTimer(GMCustomTimerCallBackType onTimerType,U16 timerid,AsynModel::UserDataInfo* pUserTimeDataInfo)\
	{\

	#define TIMER_HANDLER(timerID,func)\
		if(timerid == timerID)\
	{\
		return func(onTimerType,pUserTimeDataInfo);\
	}
	#define  END_TIMER_MAP()\
		return 0;\
	}
  
#ifdef __IOS__
  enum SocketType
  {
    ASynTCP,
    ASynUDP,
  };
#endif
}
#endif //ASYNMODEL_ASYNMODELDATADEFINE_H_GOUMANG_2009
