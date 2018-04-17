/****************************************************************************                                                                          
* SESSION.h --	constant and macros definitions	                        
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction �û�Session�Ļ���           
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
	*Session���ṩ��2�ο�����Ա������Session�Ļ��࣬
	*��װ��Session����������Ϊ�Լ���Ϣ��ת������
	*�������������Session�еķ���������Լ�����Ϣ��������յ�ҵ��Ҫ�� 
	*/
	class Session: public PostMsgBaseSession
				  ,public GMRefCounterBase<Session>
	{

	public:
		///>���ڲ����⹫��Event�ṹ��OnEventArrived�������ֻ���첽����б�����
		///>Ϊ���ܹ�ֱ�ӵĵ���OnEventArrived��������Ҫ���� ComputeWorker��LocalSender��AsynManagerΪ��Ԫ
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
		///>����SessionTimer�ص�����ָ���ʽ
		///typedef  int (Session::*T_SessonTimerCallbackFun)(U32);
		
		/**
		*�ṩ2���߳�ִ��ģʽ��
		*��Session�����ڴ��з�ʽʱ��Session���д�����Ϣ����ĳһ��ComputeWorker�߳��ڴ���Session��Ϣʱ��������ռ��Session������ComputeWorker�߳�ֻ��Ͷ����Ϣ���޷�������Ϣ��
		*��Session���ڲ��з�ʽʱ�����������ComputeWorker�߳�ͬʱ����Session���ٻ��Ĳ�ͬ��Ϣ��
		*ע�����ۺ����߳�ģʽ����Session OnStart֮ǰ����������ִ�з�Start��Ϣ��ֻ�ܽ���ϢͶ�ݵ�Session ��Ϣ�����С�
		*
		*�ṩ2��Session��פ����ʽ��
		*����ʧЧģʽ����ʾ��Session�ǳ�פSession���û����޷���������Ҳ���ᳬʱ�����������������첽ģ�͵���������һ����
		*��ʱʧЧģʽ����ʾ��Session��ʱʱ�������ٸ�Session���ⲿ�û�����ָ�����ٸ�Session������Session�ɼ�ʱ��
		*
		*�ṩ2����Ϣ�ִ�ģʽ��˳�����˳��ʽ
		*
		*�����Session�Ƿ�ɱ���Ѱ��
		*canLookFor��boolֵ�����Ϊtrue�����Ѱ�����Ὣ��Session��ID��ΪKey��Session����ָ����Ϊvalue���뵽SessionMap�У�
		*��Ҫ����ͨ�ŵĶ��󣨱���Session���ⲿӦ�ã������첽ģ���е�Session�����֪����Session ID���Ϳ�����ͨ��
		*����ͨ��Session ID��SessionMap����Ѱ����Session������ָ�룩��
		*���Ϊfalse���򲻻���뵽SessionMap�У�֪����Session ID����������û�а취��Ѱ����Session����ͨ�š�
		*/
		struct BehaviorAttribute 
		{
		public:
			/**
			*ThreadMode �߳�ģʽ
			*/
			enum ThreadMode
			{
				ASYN_SESSION_SERIALER_EXECUTE,	///<���й�����ʽ
				ASYN_SESSION_PARALLEL_EXECUTE	///<���й�����ʽ
			};
			/**
			*ResidenceModel Sessionפ����ʽ
			*/
			enum ResidenceModel
			{
				ASYN_SESSION_RESIDENT,			///<����ʧЧ��ʽ		
				ASYN_SESSION_GENERAL,			///<��ʱʧЧ��ʽ
			};
			/**
			*MsgArrivedModel ��Ϣ�ִ﷽ʽ
			*/
			enum MsgArrivedModel
			{
				ASYN_MSG_OUTOFORDER,			///<��Ҫ����Ϣ�ϸ���˳�򵽴�
				ASYN_MSG_ORDINAL				///<Ҫ����Ϣ˳�򵽴�,�����Խ���ThreadModeΪ���е�ģʽ��Ч
			};
			/**
			*BehaviorAttribute ���캯��
			* @param ��
			* @return ��
			*/
			BehaviorAttribute(): m_threadMode(ASYN_SESSION_SERIALER_EXECUTE),
										m_residenceModel(ASYN_SESSION_GENERAL),
										m_msgArrivedModel(ASYN_MSG_ORDINAL),
										m_canLookFor(true)
			{}
			/**
			*BehaviorAttribute ���캯��
			* @param threadMode ThreadMode���� ����Session���߳�ģʽ���������ͣ�
			* @param residenceModel ResidenceModel���� ����Session��פ����ʽ����פ����ʱʧЧ��
			* @param msgArrivedModel MsgArrivedModel���� ����Session����Ϣ�ִ﷽ʽ��˳�򣬷�˳��
			* @param canLookFor bool���� ����Session�Ƿ�ɼ���true���ɼ���false�����ɼ���
			* @return ��
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
			*BehaviorAttribute ��������
			* @param  ��
			* @return ��
			*/
			~BehaviorAttribute(){}
			/**
			*���صȺţ�ʵ��BehaviorAttribute��ֵ����
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
			
			ThreadMode		m_threadMode;///<�߳�ģʽ
			ResidenceModel  m_residenceModel;///<פ����ʽ
			MsgArrivedModel m_msgArrivedModel;///<��Ϣ�ִ﷽ʽ
			bool			m_canLookFor;///<�Ƿ�ɼ�
		};
		/**
		*����Session�Ự��״̬
		*/
		enum Status							
		{
			ASYN_SESSION_SUSPEND = 0,		///<����״̬���ȴ�MSG
			ASYN_SESSION_RUNNING,			///<��������״̬
			ASYN_SESSION_TERMINATING,		///<�������ٹ�����
		};
		/**
		*����Session���ٵ�ԭ��
		*/
		enum TerminateType
		{
			ASYN_SESSION_EXIT = 6000,		///<Session��������
			ASYN_SESSION_HASBEENCANCELED,	///<�����Ӧ��ǿ����ֹ
			ASYN_SESSION_TIMEOUT,			///<Session��ʱ
			ASYN_SESSION_SESSIONSTARTERROR,	///<session����ʱʧ��	
			ASYN_SESSION_CALLBACKERROR,		///<Session�ص�����
			ASYN_MODEL_ERROR				///<�첽ģ�ʹ���
		};
		/**
		*���� Session������Ϣ�����ӷ�ʽ ConnID����Sender
		*/
		enum ContactType
		{
			ASYN_CONTACTTYPE_CONNID = 0,
			ASYN_CONTACTTYPE_SENDER,
		};
		/**
		* ����BehaviorAttribute Ĭ�Ϲ����
		*/
		#define  DEFAULTATTRIBUTE BehaviorAttribute() 
        
		/**
		* Session ���캯��
		* @param  attribute BehaviorAttribute���� Ĭ�ϲ�����DEFAULTATTRIBUTE����
		*         Session�ɼ�����ʱʧЧ�����������Լ���Ϣ�ִﲻҪ��˳��
		* @return ��
		*/
		Session(BehaviorAttribute attribute = DEFAULTATTRIBUTE);
		/**
		* Session �����������鷽��ȷ��ɾ��һ��ָ��������Ļ���ָ���ʱ������������������ᱻ����
		* @param ��
		* @return ��
		*/
		virtual ~Session(void);
		/**
		* ���� ��ȡSession ID
		* @param ��
		* @return uin32���� ֵΪSessionID ����Ϊ0
		*/
		U32 GetSessionID();
		/**
		* ���� ��ȡSession ��ǰ�Ự״̬
		* @param ��
		* @return Status���ͣ�����״̬�������Լ����٣���ע������״̬û�б�����
		*/
		Status GetStatus();
		/**
		* ���� ��ȡ��ǰSession������
		* @param ��
		* @return BehaviorAttribute�ṹ������ָ�� ��const���α�������޸�ָ��ָ���ֵ
		*/
		const BehaviorAttribute* GetAttribute();
		/**
		* ���� ����Session������Session�Ĳ�ͬ�����в�ͬ����Ϊ
		* 1.�����Ƿ�ɼ��������Ƿ����SessionMap
		* 2.���ݵ�ǰִ���߳��Ƿ���ComputeWorker������ֱ��ִ��OnStart��������������ɶ˿�Ͷ��Start��Ϣ
		* @see OnStart����
		* @param timeOut U32���� Ĭ��0xFFFFFFFF�������ó�ʱ����λΪ����
		* @return int���� 0�ɹ���С��0ʧ��
		*/
		virtual int Start(U32 timeOut = 0xFFFFFFFF,bool is_Switch_thread = false);//timeout ��λΪ����	
		
		/**
		* ���� ����Session Timer
		* @param TimerID U16���� ��Ҫ���õ�TimerID
		* @param escapetime U32���� ֵ����timer����ʱ��
		* @param pUserTimeDataInfo UserDataInfo����ָ�� Ĭ��ΪNULL������ָ������Ĳ���
		* @see UserDataInfo�ṹ��
		* @return bool���� true���ɹ���fasle��ʧ��
		*/
		bool SetTimer(U16 TimerID,U32 escapetime,UserDataInfo* pUserTimeDataInfo = NULL);

		/**
		* ���� ��ȡSessionʵ��ָ�룬��Session�Ļ���PostMsgBaseSession��Ͷ����Ϣʱ����
		* @param ��
		* @return Session����ָ��
		*/
		Session* GetSession();
		/**
		* ���� ��̬���� ��װSession timerͰ��ȡ�߼�����Session Timer���ã�timer֧���Զ���Ͱ��ȡ�߼���
		* @param ��
		* @return unsigned int���ͣ�ֵΪ�����õ�Ͱ���
		*/
		static unsigned int SpecifyTimerBucketIndex(unsigned int time);
		/**
		* ���� ��̬������Session Timer���ã���ȡ�Զ����TimerͰ����
		* @param ��
		* @return unsigned int����,ֵΪͰ����
		*/
		static unsigned int GetTimerBucketNum();

	protected:
		/**
		* ���� Start��Ϣ����Ӧ�����������û�����д���û�ʵ��
		* @param ��
		* @return int���� 0���ɹ������������ɹ�
		*/
		virtual int	 OnStart();
		/**
		* ���� ������ϢTerminate����Ӧ�����������û�����д���û�ʵ��
		* @param type U32 ���� Session���ٵ�ԭ����TerminateType���壩
		* @see enum TerminiateType
		* @return
		*/
		virtual void OnTerminate(U32 type);
		
		/**
		* ���� Session��ʱ��Ϣ����Ӧ������Session�ڸú�����ֱ�ӵ���Terminate��ASYN_SESSION_TIMEOUT��
		* �û�Ҳ����д�÷���
		* @param type CustomTimerCallBackType���� �������ⲿCancel�ص����ǳ�ʱ���ڻص�
		* @see CustomTimerCallBackType
		* @param lprama U32����
		* @return int���� 0���ɹ������������ɹ�
		*/
#ifdef X64
		virtual int	 OnTimeout(GMCustomTimerCallBackType type, 
							   void* lprama);
#else
		virtual int	 OnTimeout(GMCustomTimerCallBackType type, 
							   U32 lprama);
#endif


		/**
		* ����	��ִEvent���� ,�û�������Ϣ����ָ����Ҫ��ִ��Ϣ������Ϣ���ͳɹ���ʧ�ܸ����û�ָ���Ļ�ִ
		* Ҫ���ִ���û�������OnPostMsgRecept����ִ�У��û�������д�ú��������Լ����յ���ִ�ĺ�����Ϊ
		* @param receptType ReceptType���� receptType�����ִ������(TCP����,UDP����,TCP��)
		* @see ReceptType
		* @param receptResult ReceptResult���� receptResult��ʾ��ִEvent�Ľ�����ɹ���ʧ�ܣ�
		* @see ReceptResult
		* @param msgId msgId���� ������Ϣ��ID��
		* @param data charָ������ ���͵���Ϣ����
		* @param len U32���� ������Ϣ�ĳ���
		* @param destSid U32���� Ŀ��Session ID��
		* @param contactType ContactType���� ָ����Ϣ���͵�������connID�ֻ�Sender�������connID��ôsenderҲ����Ч�ģ��ڲ�ͨ��connIDҲ�ṹ��Sender��
		* @see ContactType
		* @param connid ConnID���� ��Ϣ���ʹ��connID���ͣ���connID����Ϣ���ڴ˴�
		* @param sender SptrISender���� ��Ϣ���ʹ��Sender�����Sender��ISender����ָ�뽫�ڴ˴�
		* @param userPara voidָ������ �û�������Ϣ���Ը��ӻ�ִ����Ϣ��������չ�ӿ�
		* @param localIP U32���� ������Ϣ�ı���IP
		* @param localPort U16���� ������Ϣ�ı���Port
		* @param isReliable  bool���� �Ƿ��ǲ��ÿɿ��ķ�ʽ������Ϣ������UDP��Ч��
		* @return int���� 0���ɹ������������ɹ�
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
		* ���� ��Ϣ�ִ�Session����Ϣ���������û���������д�ú���
		* @param msgid U32���� ��ϢID��
		* @param data charָ������ Const���� ���������޸�ָ�����Ϣ����
		* @param len U32���� ��Ϣ����
		* @param srcsid U32���� ������Ϣ��Session ID��
		* @param sender SptrISender���� ֱ�ӻظ���Ϣ�ı��ͨ��
		* @return int���� 0���ɹ������������ɹ�
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
		* ���� �û�����Timer����Ӧ����
		* @param onTimerType CustomTimerCallBackType����
		* @param timerid U16���� timer id��
		* @param pUserTimeDataInfo UserDataInfo���� ����Timer��Я���Ĳ���
		* @return int���� 0���ɹ������������ɹ�
		*/
		virtual int OnTimer( GMCustomTimerCallBackType onTimerType,
							 U16 timerid,
							 AsynModel::UserDataInfo* pUserTimeDataInfo);
		
		/**
		* ���� ����Session�������ڲ�����OnTerminiate���������Session�ɼ���Session��SessionMap��ɾ��
		* @param type U32���� ���ٵ�ԭ����TerminateType���壩
		* @return int���� 0���ɹ������������ɹ�
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
		

		//��ܿ���ʶ�������������LinkBuffer��char��Streamָ��
		//�����⹫��Event�ṹ���������ֻ���첽����б�����
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
		* ���� ����Session��������Ϣ
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
		
		GMThreadID m_threadid;			 //����ģʽִ����Ҫ�ĵ�ǰִ�е��߳�ID
		volatile long m_executingThreadCount; //����ģʽ�µ�ִ�и�SESSION���߳�����
		
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
