#pragma once

#include "hostAgentSession.h"
#include "RecvUdpCmdInterfaceCtrl.h"
#include "MeetingHostAgent.h"

#define   FREE_PORT_START  (11000)
#define   FREE_PORT_END    (15000)

#define  CELL_PHONE_VERSION
#define  MCTRLAGENT_LOG  (1<<20)
#define  LOG_TYPE_ID     (2000)

struct asynPro
{
	asynPro()
	{
		init();
	}
	void init()
	{
		asyn_model_version	= 1;
		length				= 0;
		checkSum			= 0;
		reliable			= 0;
		pading				= 0;
		session_msg_version = 1;
		msgid				= 0;
		srcsid				= 0;
		destsid				= 0;
	}

	//字段	值	类型	大小（Byte）	含义
	unsigned short  asyn_model_version; //	1	short	2	版本
	unsigned short	length;             //		short	2	数据包的长度,包括头长度
	unsigned short	checkSum;           //	0	short	2	包校验值=0
	unsigned char 	reliable;  	        //  0	byte	1	是否可靠
	unsigned char	pading;             //	0	byte	1	包对齐补丁
	unsigned int  	session_msg_version;//	1	int	4	Session消息版本
	unsigned int	msgid;              //   	int	4	消息id,区分,1:原始视频数据，2:原始音频数据，3:补发视频数据，4:补发音频数据（数据包含了数据包和校验包）
	unsigned int	srcsid;             //  0	int	4	源id
	unsigned int	destsid;            //		int	4	目标id
};

/*安全异步模型头*/
struct asynPro_reliable
{
	//字段	值	类型	大小（Byte）	含义
	unsigned short  asyn_model_version; //	1	short	2	版本
	unsigned short	length;             //		short	2	数据包的长度,包括头长度
	unsigned short	checkSum;           //	0	short	2	包校验值=0
	unsigned char 	reliable;  	        //  0	byte	1	是否可靠
	unsigned char	pading;             //	0	byte	1	包对齐补丁
	unsigned int    sn;
	unsigned int  	session_msg_version;//	1	int	4	Session消息版本
	unsigned int	msgid;              //   	int	4	消息id,区分,1:原始视频数据，2:原始音频数据，3:补发视频数据，4:补发音频数据（数据包含了数据包和校验包）
	unsigned int	srcsid;             //  0	int	4	源id
	unsigned int	destsid;            //		int	4	目标id
};
/*非安全异步模型头*/
struct asynPro_unreliable
{
	//字段	值	类型	大小（Byte）	含义
	unsigned short  asyn_model_version; //	1	short	2	版本
	unsigned short	length;             //		short	2	数据包的长度,包括头长度
	unsigned short	checkSum;           //	0	short	2	包校验值=0
	unsigned char 	reliable;  	        //  0	byte	1	是否可靠
	unsigned char	pading;             //	0	byte	1	包对齐补丁
	unsigned int  	session_msg_version;//	1	int	4	Session消息版本
	unsigned int	msgid;              //   	int	4	消息id,区分,1:原始视频数据，2:原始音频数据，3:补发视频数据，4:补发音频数据（数据包含了数据包和校验包）
	unsigned int	srcsid;             //  0	int	4	源id
	unsigned int	destsid;            //		int	4	目标id
};

struct meeting_syncHead_ext
{
	unsigned int ssrcid;
	unsigned int remoteip;
	unsigned int localip;
	unsigned short remoteport;
	unsigned short localport;
};

/*本地异步模型头*/
struct meeting_syncHead
{
	unsigned int	msgid;              //   	int	4	消息id
	unsigned int	srcsid;             //  0	int	4	源id
	unsigned int	destsid;            //		int	4	目标id
};

static int asynHeadToLocal(const char *pasynPro,meeting_syncHead *plocalHead)
{
	int len = 0;
	asynPro_reliable *preliable = (asynPro_reliable*)pasynPro;
	if(1 == preliable->reliable)
	{
		plocalHead->msgid = preliable->msgid;
		plocalHead->destsid = preliable->destsid;
		plocalHead->srcsid = preliable->srcsid;
		len = sizeof(asynPro_reliable);
	}
	else
	{
		asynPro_unreliable *punReliable = (asynPro_unreliable*)pasynPro;
		plocalHead->msgid = punReliable->msgid;
		plocalHead->destsid = punReliable->destsid;
		plocalHead->srcsid = punReliable->srcsid;

		len = sizeof(asynPro_unreliable);
	}

	return len;
}

struct procCmdData
{
	unsigned int ssid;
	int          msgid;
};

class MeetingHostAgentManagerSession: public AsynModel::Session 
{
public:
	MeetingHostAgentManagerSession(void* pmanage):m_recvThread(&MeetingHostAgentManagerSession::recvWorkThreadFun,this,NULL)
	{
		memset(m_recvBuff,0,sizeof(m_recvBuff));
		memset(m_stpRCIP,0,sizeof(m_stpRCIP));
		m_curUdpPort   = FREE_PORT_START;
		m_curTcpPort   = FREE_PORT_START;
		m_resetUdpTime = false;
		m_resetTcpTime = false;
		
		m_creatSocketTime = 0;

		//线程控制
		m_recvActive      = false;
	}

	~MeetingHostAgentManagerSession();

	BEGIN_MSG_MAP(MeetingHostAgentManagerSession)
		MSG_HANDLER(MEETINGMANAGE_SEESION_RECREATE,onReCreate)
		MSG_HANDLER(MEETINGMANAGE_SEESION_RESETUDP,onResetUdpCmd)
		MSG_HANDLER(MEETINGMANAGE_SEESION_RESETTCP,onResetTcpCmd)
		MSG_HANDLER(MEETINGMANAGE_SEESION_RECVCMD_PROC,onRecvCmdMsg)
	END_MSG_MAP()

	BEGIN_TIMER_MAP(MeetingHostAgentManagerSession)
		TIMER_HANDLER(MEETINGMANAGE_SEESION_RESETUDP_TIMER,OnSetUdpTimer)
		TIMER_HANDLER(MEETINGMANAGE_SEESION_RESETTCP_TIMER,OnSetTcpTimer)
	END_TIMER_MAP()

	int onReCreate(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onRecvCmdMsg(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int onResetUdpCmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onResetTcpCmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int OnSetUdpTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	int OnSetTcpTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);

	int    init();
	void   recvWorkThreadFun(void *p);
	int    createRecvSocket(unsigned short startPort,unsigned short endPort);
	int    sendDataByUdp(U32 localssid,AsynModel::ConnID& connID,U32 destSid,U32 msgid, char* msgData,U32 dataLen);
	int    sendDataToAsynModel(U32 localssid,GMUdpSocket &sock,string &dstip,int dstPort,int dstSessionId,int msgId,char *pdata,int len);
	int    uninit();

	//接收线程
	GMThread<MeetingHostAgentManagerSession>  m_recvThread;
	//接收线程控制符
	bool                                      m_recvActive;
	int                                       m_recvThreadRun;
	GMUdpSocket                               m_recvSocket;
	//创建时间记录,保证重建socket时间差大于500ms
	unsigned int                              m_creatSocketTime; 
	//本地IP
	std::string                               m_localIp;
	
	//接收空间
	char                                      m_recvBuff[1500];
	//key为命令标号,value为对应的SeesionID
	map<unsigned int,procCmdData>             m_procCmd;
	int                                       m_bindSuccPort;

	//外面服务器地址，获取本地ip用
	char                               m_stpRCIP[16];
	unsigned short                     m_curUdpPort;
	unsigned short                     m_curTcpPort;
	bool                               m_resetUdpTime;
	bool                               m_resetTcpTime;
};

class MeetingHostAgentManager : public AsynModel::SubScribeEventCallBack
{
public:

	MeetingHostAgentManager();
	~MeetingHostAgentManager();

	void TCPDefaultListenPortFailureCallBack( char errorIP[16],unsigned short errorPort,AsynModel::AsynResultRet errorType);
	void UDPDefaultPortFailureCallBack(char errorIP[16],unsigned short errorPort,AsynModel::AsynResultRet errorType);

#ifdef X64
	int StatAddTcpTimerCallbackProc(GMCustomTimerCallBackType type,void* prama);
	int StatAddUdpTimerCallbackProc(GMCustomTimerCallBackType type,void* prama);
#else
	int StatAddUdpTimerCallbackProc(GMCustomTimerCallBackType type,unsigned int prama);
	int StatAddTcpTimerCallbackProc(GMCustomTimerCallBackType type,unsigned int prama);
#endif

	static int   init(std::string logfile,std::string logOutDir);
	
	bool Start2(std::string accounted,
		std::string localip,int localport,   
		string localCmdIp,int  localCmdPort,
		string UICmdIp,int  UICmdPort,
		string  rcAddr,string token,unsigned short& returnPort);

	bool  Start(std::string accounted,
		std::string localip,int localport,   
		string localCmdIp,int  localCmdPort,
		string UICmdIp,int  UICmdPort,
		string  rcAddr,string token,unsigned short& returnPort);

	int getRecommendMsgAbstractMange(std::string contactlistid,char* outbuf,int &outlen);
	int getRecommendMsgMange(std::string contactlistid,char* outbuf);
	int deleteRecommendMsgMange(std::string contactlistid);
	int deleteRecommendMsgMangeRecord(std::string contactlistid);

	void  Stop();
	
	bool  SetUdpPort(std::string localIp,unsigned short & usePort);
	bool  SetTcpPort(std::string localIp,unsigned short & usePort);

	bool  SetUdpPort_Uniform(std::string localIp,unsigned short & usePort);
	bool  SetTcpPort_Uniform(std::string localIp,unsigned short & usePort);

	bool  StartRecvCtrl(std::string localIP,unsigned short & localPort,std::string targetIP,unsigned short targetPort);

	void  resetUdpLocalipandPort(std::string localIp,int port,int &udpUsePort);
	void  resetTcpLocalipandPort(std::string localIp,int port,int &tcpUsePort);

	//Mange封装一层,用于发送命令
	int   sendDataByUdpManage(U32 localssid,AsynModel::ConnID& connID,U32 destSid,U32 msgid, char* msgData,U32 dataLen);

	bool   GetServerAddrFromUrl( const std::string & routeUrl, 
										std::string & meetingCtrlServerIp, 
										unsigned short & meetingCtrlServerPort );

	void	manager_UpdateIPPortMap( std::map<std::string,std::string> IPPortmap );

	int	   manager_SendTo(std::string destAccountId,const char* buf,const int len,const long long userData);
	int	   manager_SetTransDataCB(OnRecvTransDataCB cbFunc);
	int	   manager_SetTransDataFromUserCB(const char* userId,OnRecvTransDataCB cbFunc);
	int    manager_SetTransDataResultCB(OnTransDataResultCB cbFunc);

	void   OnRecvTransData(const char* fromUserId,const char* buf,const unsigned short len);
	void   OnTransDataResult(const long long userData,const int result);

public:
	int                       m_start;
	std::string               m_localip;
	RecvUdpCmdInterfaceCtrl   m_recvCtrl;
	hostAgentSession          *m_pclientSession;

#ifdef USE_RAW_SOCKET
	MeetingHostAgentManagerSession *m_manageSession;
	GMLock							m_manageSessionLock;
	int                             m_manageSid;
#endif

	//透传数据
	OnRecvTransDataCB						m_commonTransDataCB;
	std::map<std::string,OnRecvTransDataCB> m_userTransDataCB;
	GMLock									m_transCBLock;
	OnTransDataResultCB						m_transResultCB;
};