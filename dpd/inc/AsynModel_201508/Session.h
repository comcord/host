/****************************************************************************                                                                          
* SESSION.h --	constant and macros definitions	                        
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 用户Session的基类           
* Copyright (c) GOUMANG Corporation. All rights reserved.                                                                             
****************************************************************************/
#ifndef GF_SESSION_H_2008_GOUMANG
#define GF_SESSION_H_2008_GOUMANG

#include "AsynModelDataDefine.h"
#include "ISender.h"
#include "PostMsgBaseSession.h"
#include "GMTimerAssistant.h"
#include "GMQuickTimer.h"
#include "GMLockPool.h"
#include "GMLockPoolAgent.h"
#include <map>
#include <string>
class AsynModelTest;

namespace AsynModel
{
	/**
	*Session是提供给2次开发人员的所有Session的基类，
	*封装了Session基本控制行为以及消息流转能力，
	*派生类可以重载Session中的方法来完成自己的消息发送与接收的业务要求 
	*/
	class Session: public PostMsgBaseSession
				  ,public GMRefCounterBase<Session>
	{

	public:
		///>由于不对外公开Event结构，OnEventArrived这个方法只在异步框架中被调用
		///>为了能够直接的调用OnEventArrived方法，需要设置 ComputeWorker，LocalSender，AsynManager为友元
		friend class LocalSender;
		friend class AsynManager;
		friend class AsynModelTest;
		friend class IOProcessor;
		friend class TCPIOProcessor;
#ifdef WIN32
		friend class ComputeWorker;
#else
		friend class EpollControler;
#endif
		///>定义SessionTimer回调函数指针格式
		///typedef  int (Session::*T_SessonTimerCallbackFun)(U32);
		
		/**
		*提供2种线程执行模式：
		*当Session工作在串行方式时，Session串行处理消息，当某一个ComputeWorker线程在处理Session消息时，他将独占该Session，其他ComputeWorker线程只能投递消息，无法处理消息；
		*当Session处于并行方式时，则可允许多个ComputeWorker线程同时处理Session上淤积的不同消息。
		*注：无论何种线程模式，在Session OnStart之前，都不允许执行非Start消息，只能将消息投递到Session 消息队列中。
		*
		*提供2种Session的驻留方式：
		*永不失效模式：表示该Session是常驻Session，用户将无法销毁它，也不会超时，它的生命周期与异步模型的生命周期一样；
		*超时失效模式：表示该Session超时时，会销毁该Session或外部用户可以指定销毁该Session（当该Session可见时）
		*
		*提供2种消息抵达模式，顺序与非顺序方式
		*
		*定义该Session是否可被找寻：
		*canLookFor是bool值，如果为true则可找寻，将会将该Session的ID作为Key，Session智能指针作为value加入到SessionMap中，
		*需要与其通信的对象（本地Session，外部应用，其他异步模型中的Session）如果知道其Session ID，就可与其通信
		*（可通过Session ID从SessionMap中找寻到该Session的智能指针）；
		*如果为false，则不会加入到SessionMap中，知道其Session ID的其它对象将没有办法找寻到该Session与其通信。
		*/
		struct BehaviorAttribute 
		{
		public:
			/**
			*ThreadMode 线程模式
			*/
			enum ThreadMode
			{
				ASYN_SESSION_SERIALER_EXECUTE,	///<串行工作方式
				ASYN_SESSION_PARALLEL_EXECUTE	///<并行工作方式
			};
			/**
			*ResidenceModel Session驻留方式
			*/
			enum ResidenceModel
			{
				ASYN_SESSION_RESIDENT,			///<永不失效方式		
				ASYN_SESSION_GENERAL,			///<超时失效方式
			};
			/**
			*MsgArrivedModel 消息抵达方式
			*/
			enum MsgArrivedModel
			{
				ASYN_MSG_OUTOFORDER,			///<不要求消息严格按照顺序到达
				ASYN_MSG_ORDINAL				///<要求消息顺序到达,此属性仅对ThreadMode为串行的模式有效
			};
			/**
			*BehaviorAttribute 构造函数
			* @param 无
			* @return 无
			*/
			BehaviorAttribute(): m_threadMode(ASYN_SESSION_SERIALER_EXECUTE),
										m_residenceModel(ASYN_SESSION_GENERAL),
										m_msgArrivedModel(ASYN_MSG_ORDINAL),
										m_canLookFor(true)
			{}
			/**
			*BehaviorAttribute 构造函数
			* @param threadMode ThreadMode类型 定义Session的线程模式（串，并型）
			* @param residenceModel ResidenceModel类型 定义Session的驻留方式（常驻，超时失效）
			* @param msgArrivedModel MsgArrivedModel类型 定义Session的消息抵达方式（顺序，非顺序）
			* @param canLookFor bool类型 定义Session是否可见（true：可见，false：不可见）
			* @return 无
			*/
			BehaviorAttribute(ThreadMode        threadMode,
									  ResidenceModel  residenceModel,
									  MsgArrivedModel msgArrivedModel,  
									  bool					  canLookFor)
									  :m_threadMode(threadMode),
									   m_residenceModel(residenceModel),
									   m_msgArrivedModel(msgArrivedModel),
									   m_canLookFor(canLookFor)
			{
				
			}
			/**
			*BehaviorAttribute 析构函数
			* @param  无
			* @return 无
			*/
			~BehaviorAttribute(){}
			/**
			*重载等号，实现BehaviorAttribute赋值操作
			* @param  right 
			* @return BehaviorAttribute
			*/
			BehaviorAttribute& operator= (const BehaviorAttribute& right)
			{
				if(&right != this)
				{
					m_threadMode = right.m_threadMode;
					m_residenceModel = right.m_residenceModel;
					m_msgArrivedModel = right.m_msgArrivedModel;
					m_canLookFor = right.m_canLookFor;
				}				
				return *this;
			}
			
			ThreadMode		m_threadMode;///<线程模式
			ResidenceModel  m_residenceModel;///<驻留方式
			MsgArrivedModel m_msgArrivedModel;///<消息抵达方式
			bool			m_canLookFor;///<是否可见
		};
		/**
		*定义Session会话的状态
		*/
		enum Status							
		{
			ASYN_SESSION_SUSPEND = 0,		///<挂起状态，等待MSG
			ASYN_SESSION_RUNNING,			///<正在运行状态
			ASYN_SESSION_TERMINATING,		///<处于销毁过程中
		};
		/**
		*定义Session销毁的原因
		*/
		enum TerminateType
		{
			ASYN_SESSION_EXIT = 6000,		///<Session正常结束
			ASYN_SESSION_HASBEENCANCELED,	///<被外界应用强行终止
			ASYN_SESSION_TIMEOUT,			///<Session超时
			ASYN_SESSION_SESSIONSTARTERROR,	///<session启动时失败	
			ASYN_SESSION_CALLBACKERROR,		///<Session回调错误
			ASYN_MODEL_ERROR				///<异步模型错误
		};
		/**
		*定义 Session发送消息的连接方式 ConnID还是Sender
		*/
		enum ContactType
		{
			ASYN_CONTACTTYPE_CONNID = 0,
			ASYN_CONTACTTYPE_SENDER,
		};
		/**
		* 定义BehaviorAttribute 默认构造宏
		*/
		#define  DEFAULTATTRIBUTE BehaviorAttribute() 
        
		/**
		* Session 构造函数
		* @param  attribute BehaviorAttribute类型 默认参数是DEFAULTATTRIBUTE，即
		*         Session可见，超时失效，串行运行以及消息抵达不要求顺序
		* @return 无
		*/
		Session(BehaviorAttribute attribute = DEFAULTATTRIBUTE);
		/**
		* Session 析构函数，虚方法确保删除一个指向派生类的基类指针的时候派生类的析构函数会被调用
		* @param 无
		* @return 无
		*/
		virtual ~Session(void);
		/**
		* 描述 获取Session ID
		* @param 无
		* @return uin32类型 值为SessionID 不会为0
		*/
		U32 GetSessionID();
		/**
		* 描述 获取Session 当前会话状态
		* @param 无
		* @return Status类型（挂起状态，运行以及销毁），注：挂起状态没有被起用
		*/
		Status GetStatus();
		/**
		* 描述 获取当前Session的属性
		* @param 无
		* @return BehaviorAttribute结构体类型指针 加const修饰避免外界修改指针指向的值
		*/
		const BehaviorAttribute* GetAttribute();
		/**
		* 描述 启动Session，根据Session的不同属性有不同的行为
		* 1.根据是否可见来决定是否加入SessionMap
		* 2.根据当前执行线程是否是ComputeWorker，是则直接执行OnStart方法，否则向完成端口投递Start消息
		* @see OnStart方法
		* @param timeOut U32类型 默认0xFFFFFFFF即不设置超时，单位为毫秒
		* @return int类型 0成功，小于0失败
		*/
		virtual int Start(U32 timeOut = 0xFFFFFFFF,bool is_Switch_thread = false);//timeout 单位为毫秒	
		
		/**
		* 描述 设置Session Timer
		* @param TimerID U16类型 需要设置的TimerID
		* @param escapetime U32类型 值代表timer触发时间
		* @param pUserTimeDataInfo UserDataInfo类型指针 默认为NULL，即不指定传输的参数
		* @see UserDataInfo结构体
		* @return bool类型 true：成功，fasle：失败
		*/
		bool SetTimer(U16 TimerID,U32 escapetime,UserDataInfo* pUserTimeDataInfo = NULL);

		/**
		* 描述 获取Session实例指针，在Session的基类PostMsgBaseSession中投递消息时调用
		* @param 无
		* @return Session类型指针
		*/
		Session* GetSession();
		/**
		* 描述 静态方法 封装Session timer桶获取逻辑，供Session Timer调用（timer支持自定义桶获取逻辑）
		* @param 无
		* @return unsigned int类型，值为计算获得的桶标号
		*/
		static unsigned int SpecifyTimerBucketIndex(unsigned int time);
		/**
		* 描述 静态方法供Session Timer调用，获取自定义的Timer桶个数
		* @param 无
		* @return unsigned int类型,值为桶个数
		*/
		static unsigned int GetTimerBucketNum();

	protected:
		/**
		* 描述 Start消息的响应函数，可由用户派生写入用户实现
		* @param 无
		* @return int类型 0：成功，负数：不成功
		*/
		virtual int	 OnStart();
		/**
		* 描述 销毁消息Terminate的响应函数，可由用户派生写入用户实现
		* @param type U32 类型 Session销毁的原因（由TerminateType定义）
		* @see enum TerminiateType
		* @return
		*/
		virtual void OnTerminate(U32 type);
		
		/**
		* 描述 Session超时消息的响应函数，Session在该函数中直接调用Terminate（ASYN_SESSION_TIMEOUT）
		* 用户也可重写该方法
		* @param type CustomTimerCallBackType类型 表明是外部Cancel回调还是超时到期回调
		* @see CustomTimerCallBackType
		* @param lprama U32类型
		* @return int类型 0：成功，负数：不成功
		*/
#ifdef X64
		virtual int	 OnTimeout(GMCustomTimerCallBackType type, 
							   void* lprama);
#else
		virtual int	 OnTimeout(GMCustomTimerCallBackType type, 
							   U32 lprama);
#endif


		/**
		* 描述	回执Event处理 ,用户发送消息可以指定需要回执信息，当消息发送成功或失败根据用户指定的回执
		* 要求回执给用户，触发OnPostMsgRecept函数执行，用户派生重写该函数定义自己接收到回执的后续行为
		* @param receptType ReceptType类型 receptType代表回执的类型(TCP发送,UDP发送,TCP打洞)
		* @see ReceptType
		* @param receptResult ReceptResult类型 receptResult表示回执Event的结果（成功，失败）
		* @see ReceptResult
		* @param msgId msgId类型 发送消息的ID号
		* @param data char指针类型 发送的消息内容
		* @param len U32类型 发送消息的长度
		* @param destSid U32类型 目标Session ID号
		* @param contactType ContactType类型 指名消息发送的渠道（connID抑或Sender）如果是connID那么sender也是有效的（内部通过connID也会构造Sender）
		* @see ContactType
		* @param connid ConnID类型 消息如果使用connID发送，则connID的信息将在此处
		* @param sender SptrISender类型 消息如果使用Sender，则该Sender的ISender智能指针将在此处
		* @param userPara void指针类型 用户发送消息可以附加回执的信息，属于扩展接口
		* @param localIP U32类型 发送消息的本地IP
		* @param localPort U16类型 发送消息的本地Port
		* @param isReliable  bool类型 是否是采用可靠的方式发送消息（仅对UDP有效）
		* @return int类型 0：成功，负数：不成功
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		virtual int OnPostMsgRecept(AsynModel::ReceptType receptType,//TCP_SEND UDP_SEND
									AsynModel::ReceptResult receptResult,
									U32 msgId,
								    const char *data, 
									U32 len, 
									U32 destSid,
									AsynModel::Session::ContactType contactType, //ASYN_CONTACTTYPE_CONNID,ASYN_CONTACTTYPE_SENDER
									AsynModel::ConnID& connid,
									const AsynModel::SptrISender& sender,
									AsynModel::UserDataInfo* userPara,
									U32 localIP,
									U16 localPort,
									bool isReliable);
#else
		virtual int OnPostMsgRecept(AsynModel::ReceptType receptType,
									AsynModel::ReceptResult receptResult,
									U32 msgId,
									const char *data, 
									U32 len, 
									U32 destSid,
									AsynModel::Session::ContactType contactType, 
									AsynModel::ConnID& connid,
									const AsynModel::ISender* sender,
									AsynModel::UserDataInfo* userPara,
									U32 localIP,
									U16 localPort,
									bool isReliable);
#endif
		/**
		* 描述 消息抵达Session后，消息处理函数，用户可派生重写该函数
		* @param msgid U32类型 消息ID号
		* @param data char指针类型 Const修饰 表明不能修改指向的消息内容
		* @param len U32类型 消息长度
		* @param srcsid U32类型 发送消息的Session ID号
		* @param sender SptrISender类型 直接回复消息的便捷通道
		* @return int类型 0：成功，负数：不成功
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		virtual int	 OnMsgArrived(U32 msgid,
								  const char* data,
								  U32 len,
								  U32 srcsid,
								  const AsynModel::SptrISender& sender);
#else
		virtual int	 OnMsgArrived(U32 msgid,
								  const char* data,
								  U32 len,
								  U32 srcsid,
								  const AsynModel::ISender* sender);
#endif
		/**
		* 描述 用户设置Timer的响应函数
		* @param onTimerType CustomTimerCallBackType类型
		* @param timerid U16类型 timer id号
		* @param pUserTimeDataInfo UserDataInfo类型 设置Timer所携带的参数
		* @return int类型 0：成功，负数：不成功
		*/
		virtual int OnTimer( GMCustomTimerCallBackType onTimerType,
							 U16 timerid,
							 AsynModel::UserDataInfo* pUserTimeDataInfo);
		
		/**
		* 描述 销毁Session函数，内部调用OnTerminiate，并且如果Session可见则将Session从SessionMap中删除
		* @param type U32类型 销毁的原因（由TerminateType定义）
		* @return int类型 0：成功，负数：不成功
		*/
		int	 Terminate(U32 type);
		
	private:
		bool PopEvent(EventItem& event);
		bool SafePopEvent(EventItem& eventItem);
		void PushEvent(EventItem& event);
		int	 DispatchEvent(EventItem& event);

#ifdef X64
int PostMsgReceptFilter(U64 wPrama, U64 lPrama);
#else
int PostMsgReceptFilter(U32 wPrama, U32 lPrama);
#endif

		static GMLockPool_SmartPtr GetLockPool();
		static U32 GenerateSessionID(BehaviorAttribute::ResidenceModel residencemodel);
		

		//框架可以识别的数据类型有LinkBuffer、char和Stream指针
		//不对外公开Event结构，这个方法只在异步框架中被调用
		int OnEventArrived(Event& pEvent,const SptrISender& sender);
		int CheckIsStartedProcess(Event& pEvent,const SptrISender& sender,GMThreadID threadid,bool msgHasdispatch,bool &isStartEvent);
		int ParallelExecuteProcess(Event& pEvent,const SptrISender& sender,GMThreadID threadid,bool msgHasdispatch,bool isStartEvent);
		int SerialerExecuteProcess(Event& pEvent,const SptrISender& sender,GMThreadID threadid,bool &msgHasdispatch,bool isStartEvent);

		int TransmitTimerEvent(GMQuickTimerCallBackType type,void* prama);

		bool SetTimeoutTimer(U32 escapetime);
		static void ReclaimEvent(Event& event);
	
#ifdef TEST
	public:
#else
	private:
#endif
		/**
		* 描述 保存Session的属性信息
		*/
		BehaviorAttribute m_attribute;
		typedef GMBlist<EventItem, GMFixedMemAllocMethod> EventList;
		//typedef GMAutoLock<GMLockPoolAgent>   autolock;
		typedef GMAutoLock<GMLock>   autolock;
		static U32 m_residentSessionIDBase;
		static U32 m_staySessionIDBase;
		static GMLock  m_sessionIDLock;

		static GMLock m_lockPoolLock;
		static bool  m_lockPoolInitFlag;
		static GMLockPool_SmartPtr m_lockPool;

		//GMLockPoolAgent m_lock;
		GMLock		m_lock;
		EventList m_eventList;
		
		GMThreadID m_threadid;			 //串行模式执行需要的当前执行的线程ID
		volatile long m_executingThreadCount; //并行模式下的执行该SESSION的线程数量
		
#ifndef WIN32
		GMLockPoolAgent m_executingThreadCountLock;
#endif
		bool m_beTerminating;
		U32 m_sessionID;
		
		bool m_isStart;
		bool m_hasPostStartEvent;
	};

	inline bool Session::PopEvent(EventItem& eventItem)
	{
		if(m_eventList.empty())
			return false;

		eventItem = m_eventList.front();
		m_eventList.pop_front();
		return true;
	}

	inline bool Session::SafePopEvent(EventItem& eventItem)
	{
		autolock al(&m_lock);
		return PopEvent(eventItem);
	}

	inline void Session::PushEvent(EventItem& eventItem)
	{
		autolock al(&m_lock);
		m_eventList.push_back(eventItem);	
	}	


	typedef GMEmbedSmartPtr<Session> SptrSession;
	#define NullSptrSession SptrSession()
}


#endif //GF_SESSION_H_2008_GOUMANG
