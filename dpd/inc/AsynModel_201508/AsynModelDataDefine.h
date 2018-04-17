/****************************************************************************                                                                          
* AsynModelDataDefine.h                       
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction �����첽ģ�͵�Event��EventItem�� Э�飬��ִ����������Msgӳ����û�
*          ��Ҫ��֪�Ľṹ
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
	* �첽ģ���ڲ���ת�¼�����
	*/
	struct Event
	{
		U32	destSid;	///<Ŀ��ŵĻỰSID
		U32	eventid;    ///<EventID�ж�������;�Ϊ��ܶ������ͣ�����Ϊ�û��Զ�������
#ifdef X64
		U64	wPrama;
#else
		U32	wPrama;     ///<һ�����Msgָ��
#endif
#ifdef X64
		U64		lPrama;
#else
		U32		lPrama;     ///<��չ����
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
		* ���� ���캯�� 1
		* @param dsid U32���� Ŀ��Session ID��
		* @param eid  U32���� �¼�ID
		* @param wp   wPrama����
		* @param lp   lPrama����
		* @return ��
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
		* ���� ���캯�� 2
		* @param ��
		* @return ��
		*/
		Event()
			:destSid(0),eventid(0),wPrama(0),lPrama(0),startPostTime(GMTimerAssistant::GetSysCurrentTime())
		{}

	};
	/**
	*Э�����Ͷ��� ���TCP���Ƿ���Ҫͨ��SessionΪ�û��ṩListen��accept�ӿ�
	*/
	enum Protocol
	{
		UDP = 1,///<UDPЭ��
		TCP ,   ///<TCPЭ��
		ANYONE  ///<����һ��Э��
	};
	
	/**
	*��ִ���Ͷ���
	*/
	enum ReceptType
	{
		TCP_SEND = 0,///<TCP����
		UDP_SEND ///<UDP����
	};
	/**
	*	��ִ�������
	*/
	enum ReceptResult
	{
		RECEPT_SUCCESS = 0,///<�û������ɹ�
		RECEPT_COMPLETE_PORT_FAILURE,///<��ɶ˿ڷ������󣬲���ʧ��
		RECEPT_SOCKET_ERROR,///<Socket����
		RECEPT_TCP_CONNECT_FAILURE,///<TCP����ʧ��
		RECEPT_TIMEOUT,///<��ʱ
		RECEPT_TIMEOUT_SENDING,///<�����ڷ����������ѳ�ʱ��ϵͳ�����ڷ��ͳɹ���ʧ�ܺ��ٴλ�ִ�û�
		RECEPT_SET_TIMER_ERROR,///<����timerʧ��
		RECEPT_TCP_PACKAGE_ERROR,///<TCP������󣬿��ܴ���İ���С������߰�����У�����
		RECEPT_ASYNMODEL_DESTROYED,///<�첽ģ���Ѿ�������
		RECEPT_UNKNOW_ERROR, ///<δ֪����
		RECEPT_MEM_ALLOC_FAILED ///�ڴ�������
	};
	/**
	* �û���ִ��Ϣ��������
	*/
	enum SubScribeMsgDefine
	{
		POSTMSG_NO_SUBSCRIBE = 0,///<������
		POSTMSG_SUCCESS = 1,///<���ķ��ͳɹ�
		POSTMSG_FAILURE   = 2,///<���ķ���ʧ��
		POSTMSG_ALL     = 3 ///<�������л�ִ��Ϣ
	};
	/**
	*Session �ȴ�������Ϣ�����е�Ԫ�ؽṹ�嶨��
	*/
	struct EventItem
	{
		Event       m_event;///<����Session���¼�
		SptrISender m_sender;///<�ظ���Ϣ��Ŀ��Session�Ŀ��ͨ����Sender��
		/**
		* ���� ���캯�� 1
		* @param event Event����
		* @see Event����
		* @param sender SptrISender����ָ������
		* @see  SptrISender����
		* @return ��
		*/
		EventItem(Event event,SptrISender sender):
				 m_event(event),m_sender(sender)
		{}
		/**
		* ���� ���캯�� 2
		* @param ��
		* @return ��
		*/
		EventItem()
			:m_sender(NULL)
		{}
	};

	/**
	* Session �û��������ݽṹ���ඨ�壬�ṩ����������������д�����ڲ���Դ���ͷŴ���
	*/
	struct UserDataInfo:GMRefCounterBase<UserDataInfo>
	{
	public:
		UserDataInfo()
		{
			AddRef();
		}
		/**
		* ���� ����������������д�����ڲ���Դ���ͷŴ���
		* @param ��
		* @return ��
		*/
		virtual ~UserDataInfo(){}
	};

typedef GMEmbedSmartPtr<UserDataInfo> Sptr_UserDataInfo;
	/**
	* Session ��Ϣӳ�䶨�壬��MsgID������Session�ж���Ĵ�����Ϣ�����������������ݲ�ͬ��MsgID���ò�ͬ�Ĵ�����
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
	* Session timerӳ�䶨�壬��timerID������Session�ж����timer�������������������ݲ�ͬ��timerID���ò�ͬ�Ĵ�����
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
