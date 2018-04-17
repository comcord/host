#pragma once
#include "stdafx.h"
#include "client_session.h"
#include "util.h"
#include "meetingHostCmd.h"

#include "RecvUdpCmdInterfaceCtrl.h"
#include "CommonData.h"
#include "tinyxml.h"
#include "Date.h"

//BUGENGIEN使用到的头文件
//#include "LPClientManager.h"
//#include "ClientHandleSession.h"
//using namespace LogPumpClient;
//#define WIN_TEST


#define  __MEETINGHOSTAGENT_SESSIONSTATE_NO       0 /*没有工作*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_HOST     1 /*获取host*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_REG      2 /*向host注册*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_HEART    3 /*向host心跳中*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_DOWNLINE 4 /*被迫下线状态*/

#define  __MEETINGHOSTAGENT_TIME_REG            1
#define  __MEETINGHOSTAGENT_TIME_HEART          2
#define  __MEETINGHOSTAGENT_TIME_SHAREIP        3
#define  __MEETINGHOSTAGENT_TIME_SETLOCAL       4
#define  __MEETINGHOSTAGENT_TIME_INVITEANDCHECK 5
#define  __MEETINGHOSTAGENT_TIME_SENDSHAREINFO  6
#define  __MEETINGHOSTAGENT_TIME_SENDSHAREINFOTOHOST        7
#define  __MEETINGHOSTAGENT_TIME_SENDTRANSDATA  8

#define  INVITE_SEND_GUID_TIMES                10
#define  INVITE_RECEIVE_GUID_TIME_OUT          30000
#define  INVITE_CHECK_TIME_STEP                2000

struct meetingHost_trans_data_ex : public meetingHost_trans_data
{
	unsigned char m_sendTimes;		//发送次数		
	U64    m_lastSendTime;			//上传发送时间
	meetingHost_trans_data_ex()
	{
		m_lastSendTime = 0;
		m_sendTimes = 0;
	}
};


class hostAgentSession: public AsynModel::Session
{
public:
	hostAgentSession();
	~hostAgentSession(void);
	
	BEGIN_MSG_MAP(hostAgentSession)
		MSG_HANDLER(MEETINGHOST_REGISTER,onRegister)
		MSG_HANDLER(MEETINGHOSTAGENT_STOP,onStop)
		MSG_HANDLER(MEETINGHOSTAGNET_UI_CMD,onUICmd)
		MSG_HANDLER(HOSTAGENT_GETHOSTADDR_RESP,onReqHostResult)
		MSG_HANDLER(MEETINGHOST_CMD_SHAREIP_RESP,onShareipResp)
		MSG_HANDLER(MEETINGHOST_CMD_SHAREPHONE_RESP,onSharePhoneInfoResp)
		MSG_HANDLER(MEETINGHOST_CMD_REGISTER_RESP,onRegisterResult)
		MSG_HANDLER(MEETINGHOST_CMD_INVITED,onInvited)
		MSG_HANDLER(MEETINGHOST_CMD_INVITED_SINGLE,onInvitedSingle)
		MSG_HANDLER(MEETINGHOST_CMD_INVITED_SINGLE_RESP_DEST,onInviteSingleResp)
		MSG_HANDLER(MEETINGHOST_CMD_HEART_RESP,onHeartResult)
		//BugEngien命令处理
		MSG_HANDLER(MEETINGHOST_CMD_BUGENGIEN,onBugEngien)
		//收到HOST发送的推荐信息
		MSG_HANDLER(MEETINGHOST_CMD_AGENT_RECOMMEND_REQUEST,onRecommendRequest)
		MSG_HANDLER(MEETINGHOST_CMD_GET_RECOMMENDMSG_RESP,onRecommendMsgArrive)
		//收到HOST发送的打洞信息
		MSG_HANDLER(MEETINGHOST_AGENT_CMD_DIGHOLE_REQ,onPuntchRequest)

		MSG_HANDLER(MEETINGHOST_UI_TRANSDATA,onTransData)
		MSG_HANDLER(MEETINGHOST_CMD_TRANSDATA_REQ,onRecvTransData)
		MSG_HANDLER(MEETINGHOST_CMD_TRANSDATA_RESP,onRecvTransDataResp)

	END_MSG_MAP()

	BEGIN_TIMER_MAP(hostAgentSession)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_REG,OnTimerRegister)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_HEART,OnTimerHeart)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_SHAREIP,OnTimerShareip)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_SETLOCAL,onTimerSetLocal)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_INVITEANDCHECK,onTimerInviteCheck)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_SENDSHAREINFO,onTimerSendShareInfo)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_SENDSHAREINFOTOHOST,onTimerShareInfoToHost)
		TIMER_HANDLER(__MEETINGHOSTAGENT_TIME_SENDTRANSDATA,onTimerSendTransData)
	END_TIMER_MAP()

	int	OnStart();

protected:
	/*收到UI的向host注册命令*/
	int onRegister(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*通过rc获得host回应*/
	int onReqHostResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*向host注册回应*/
	int onRegisterResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*邀请参会者*/
	int onInvite(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*收到邀请*/
	int onInvited(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int onInvitedSingle(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onInviteSingleResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*sessing退出*/
	int onStop(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*心跳响应*/
	int onHeartResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*收到UI发向Agent命令*/
	int onUICmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*收到ui,发布共享信息*/
	int onShareInfo(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*收到server发布共享信息应答*/
	int onShareipResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*被迫线下回复*/
	int onSharePhoneInfoResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*BugEngien命令处理*/
	int onBugEngien(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*收到推荐请求*/
	int onRecommendRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onRecommendMsgArrive(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int recommendResp(char* guid,char* userid,U32 srcsid,const AsynModel::SptrISender& sender);

	int onPuntchRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int onTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int	onRecvTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onRecvTransDataResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*定时器*/
	/*与hostServer心跳*/
	int OnTimerHeart(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	/*检查向host注册响应*/
	int OnTimerRegister(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	/*定时检查发布*/
	int OnTimerShareip(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);

	int onTimerSetLocal(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);

	int onTimerInviteCheck(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	int onTimerSendShareInfo(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	int onTimerShareInfoToHost(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);

	int onTimerSendTransData(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);

	int OnPostMsgRecept(AsynModel::ReceptType receptType,//TCP_SEND UDP_SEND
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

	/*小函数*/
	/*组装命令*/
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string name,int cmdid,int meetingid ,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string accountid,int cmdid,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string name,int cmdid,int meetingid,std::string accountid,std::string id,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,int cmdid,int status, std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result, std::string inviteid, int cmdid, std::string name, int meetingid, std::string methodMsg);

	void sendShareInfoToServer();

	/*发布共享ip*/
	void sendShareip();
	/*获取hostServerConnID*/
	AsynModel::ConnID GetHostServerConnID();
public:
	/*辅助函数*/
	bool  CreateGetRoutServerSession(const std::string & routeUrl);
	void  delRCSession();
	bool  GetRouteServerAddrFromUrl(const std::string & routeUrl);
	bool  GetServerAddrFromUrl(const std::string & routeUrl,std::string & meetingCtrlServerIp,unsigned short & meetingCtrlServerPort);
	/*向控制路由获取host地址*/
	void  StartRequestHost();
	bool  ContinueRequestHost();
	/*向host发送注册命令*/
	int   sendRegisterToServer();
	int   sendheart();
	/*发送命令给UI*/
	bool  sendCmdToUI(char *buf,int len);

	/*推荐请求命令处理*/
	bool ParseJson(char* buff,Json::Value & cmd);
	int  parseRecommendMsgResp(char* data,PerUserRecommendMsgResp resp);

	int getRecommendMsgAbstractWork(std::string contactlistid,char* outbuf,int &outlen);
	int getRecommendMsgWork(std::string contactlistid,char* outbuf);
	int deleteRecommendMsgWork(std::string contactlistid);
	int deleteRecommendMsgWorkRecord(std::string contactlistid);

	int sendRecommendInfoToUI(PerUserRecommendMsg msg);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int  buildQueryRecommendMsg(map<string,PerUserRecommendMsg> saveRecTask,std::string& result);
	int  buildPerUserRecommendMsg(PerUserRecommendMsg records,std::string& result);

	//将推荐给UI的记录保存到本地
	int  saveRecRecordMsgToDisk(std::string path,std::map<std::string,int> recRecordMsg);
	int  readRecRecordMsgFromDisk(std::string path,std::map<std::string,int> &recRecordMsg);

	//保存指定任务ID的单个推荐信息
	int saveRecMsgToDisk(std::string path,std::map<std::string,PerUserRecommendMsg> recmsg);
	//读取所有推荐任务ID的推荐信息
	int readAllRecMsgFromDisk(std::string path,std::multimap<std::string,Contact> &recMsg);
	//转化为目标推荐人及推荐信息的形式
	int tansToPerUerRecMsgMap(std::multimap<std::string,Contact> msg,std::map<string,PerUserRecommendMsg> &recMsg);
	//删除本地已推荐给UI的信息
	int deleteRecMsgFromDisk(std::string path,std::string recKey);

	//ip 映射
	void HandleStpIPMap(std::string & stpIp,unsigned short & stpPort);
public:
	//路由服务器地址
	std::string               m_rcAddr;
	std::string               m_routeServerIp1;
	unsigned short            m_routeServerPort1;
	std::string               m_routeServerIp2;
	unsigned short            m_routeServerPort2;
	client_session		      *m_rcClientStpr1;
	client_session	          *m_rcClientStpr2;
	int                       m_reqHostCount;   /*连续尝试rc1和rc2*/

	int                       m_sessionState;   /*表示session当前的工作状态*/
	std::string               m_hostip;
	int                       m_hostport;
	unsigned int              m_serverSessinid;

	std::string               m_regGuid;        /*注册时的guid*/
	int                       m_regCount;       /*注册尝试次数*/
	U64                       m_lastHeartTime;  /*最后的心跳响应时间*/
	bool                      m_bHeart;         /*是否继续心跳*/

	bool                      m_bHeartTime;     /*心跳定时器启动*/

	RecvUdpCmdInterfaceCtrl   *m_recv;

	unsigned int               m_sendShareInfoFlag;


	bool                      m_bShareip;       /*是否发布成功*/
	std::string               m_bShareguid;     /*发布的guid，做命令标识用*/
	bool                      m_bShareTimeStart;/*发布定时器启动*/
	std::string               m_shareip;        /*发布的地址*/
	int                       m_shareport;
	std::string               m_userid;
	std::string               m_name;

	int                       heart_index;    /*心跳索引*/

	std::string               m_token;		  //手机发布token信息
	std::string               m_checktoken;
	bool                      m_canRegist;

	map<std::string,std::string>    m_recMsgCheck;         //key为GUID，value为userid;
	map<string,PerUserRecommendMsg> m_saveRecTaskManage;   //保存推荐任务信息
	map<std::string,int>            m_saveRecVersion;      //保存UI获取通讯录的版本号
	 
	std::string						m_remoteIp;			   //屏幕分享打洞
	unsigned short			        m_remotePort;   
	bool                            m_isSendShareInfo;
	char*                           m_shareInfo;
	int                             m_shareInfoLen;
	int                             m_sendShareCount;

	map<std::string,InviteCmdContext>  m_sendInviteRecord;  //记录发出的邀请
	map<std::string,I64>               m_recvInviteRecord;  //接受到邀请命令并发送给UI后的记录map	

	int							   m_recvReqHostRespCount;	//记录向STPRC请求HOST的次数,达到一定次数就换端口
	unsigned int                   m_manageSid;             //记录ManageSeesionID

	unsigned int                   m_registerFlag;

	map<I64,meetingHost_trans_data_ex> m_transData;
	GMLock							   m_transDataLock;
};

