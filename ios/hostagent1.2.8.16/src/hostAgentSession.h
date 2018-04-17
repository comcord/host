#pragma once
#include "stdafx.h"
#include "client_session.h"
#include "util.h"
#include "meetingHostCmd.h"

#include "RecvUdpCmdInterfaceCtrl.h"
#include "CommonData.h"
#include "tinyxml.h"
#include "Date.h"

//BUGENGIENʹ�õ���ͷ�ļ�
//#include "LPClientManager.h"
//#include "ClientHandleSession.h"
//using namespace LogPumpClient;
//#define WIN_TEST


#define  __MEETINGHOSTAGENT_SESSIONSTATE_NO       0 /*û�й���*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_HOST     1 /*��ȡhost*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_REG      2 /*��hostע��*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_HEART    3 /*��host������*/
#define  __MEETINGHOSTAGENT_SESSIONSTATE_DOWNLINE 4 /*��������״̬*/

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
	unsigned char m_sendTimes;		//���ʹ���		
	U64    m_lastSendTime;			//�ϴ�����ʱ��
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
		//BugEngien�����
		MSG_HANDLER(MEETINGHOST_CMD_BUGENGIEN,onBugEngien)
		//�յ�HOST���͵��Ƽ���Ϣ
		MSG_HANDLER(MEETINGHOST_CMD_AGENT_RECOMMEND_REQUEST,onRecommendRequest)
		MSG_HANDLER(MEETINGHOST_CMD_GET_RECOMMENDMSG_RESP,onRecommendMsgArrive)
		//�յ�HOST���͵Ĵ���Ϣ
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
	/*�յ�UI����hostע������*/
	int onRegister(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*ͨ��rc���host��Ӧ*/
	int onReqHostResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*��hostע���Ӧ*/
	int onRegisterResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*����λ���*/
	int onInvite(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*�յ�����*/
	int onInvited(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int onInvitedSingle(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onInviteSingleResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*sessing�˳�*/
	int onStop(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*������Ӧ*/
	int onHeartResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*�յ�UI����Agent����*/
	int onUICmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*�յ�ui,����������Ϣ*/
	int onShareInfo(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*�յ�server����������ϢӦ��*/
	int onShareipResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*�������»ظ�*/
	int onSharePhoneInfoResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*BugEngien�����*/
	int onBugEngien(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	/*�յ��Ƽ�����*/
	int onRecommendRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onRecommendMsgArrive(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int recommendResp(char* guid,char* userid,U32 srcsid,const AsynModel::SptrISender& sender);

	int onPuntchRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int onTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int	onRecvTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int onRecvTransDataResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	/*��ʱ��*/
	/*��hostServer����*/
	int OnTimerHeart(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	/*�����hostע����Ӧ*/
	int OnTimerRegister(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	/*��ʱ��鷢��*/
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

	/*С����*/
	/*��װ����*/
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string name,int cmdid,int meetingid ,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string accountid,int cmdid,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,std::string name,int cmdid,int meetingid,std::string accountid,std::string id,std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result,int cmdid,int status, std::string methodMsg);
	int CompositingCmd(Json::Value & cmd, std::string &result, std::string inviteid, int cmdid, std::string name, int meetingid, std::string methodMsg);

	void sendShareInfoToServer();

	/*��������ip*/
	void sendShareip();
	/*��ȡhostServerConnID*/
	AsynModel::ConnID GetHostServerConnID();
public:
	/*��������*/
	bool  CreateGetRoutServerSession(const std::string & routeUrl);
	void  delRCSession();
	bool  GetRouteServerAddrFromUrl(const std::string & routeUrl);
	bool  GetServerAddrFromUrl(const std::string & routeUrl,std::string & meetingCtrlServerIp,unsigned short & meetingCtrlServerPort);
	/*�����·�ɻ�ȡhost��ַ*/
	void  StartRequestHost();
	bool  ContinueRequestHost();
	/*��host����ע������*/
	int   sendRegisterToServer();
	int   sendheart();
	/*���������UI*/
	bool  sendCmdToUI(char *buf,int len);

	/*�Ƽ����������*/
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

	//���Ƽ���UI�ļ�¼���浽����
	int  saveRecRecordMsgToDisk(std::string path,std::map<std::string,int> recRecordMsg);
	int  readRecRecordMsgFromDisk(std::string path,std::map<std::string,int> &recRecordMsg);

	//����ָ������ID�ĵ����Ƽ���Ϣ
	int saveRecMsgToDisk(std::string path,std::map<std::string,PerUserRecommendMsg> recmsg);
	//��ȡ�����Ƽ�����ID���Ƽ���Ϣ
	int readAllRecMsgFromDisk(std::string path,std::multimap<std::string,Contact> &recMsg);
	//ת��ΪĿ���Ƽ��˼��Ƽ���Ϣ����ʽ
	int tansToPerUerRecMsgMap(std::multimap<std::string,Contact> msg,std::map<string,PerUserRecommendMsg> &recMsg);
	//ɾ���������Ƽ���UI����Ϣ
	int deleteRecMsgFromDisk(std::string path,std::string recKey);

	//ip ӳ��
	void HandleStpIPMap(std::string & stpIp,unsigned short & stpPort);
public:
	//·�ɷ�������ַ
	std::string               m_rcAddr;
	std::string               m_routeServerIp1;
	unsigned short            m_routeServerPort1;
	std::string               m_routeServerIp2;
	unsigned short            m_routeServerPort2;
	client_session		      *m_rcClientStpr1;
	client_session	          *m_rcClientStpr2;
	int                       m_reqHostCount;   /*��������rc1��rc2*/

	int                       m_sessionState;   /*��ʾsession��ǰ�Ĺ���״̬*/
	std::string               m_hostip;
	int                       m_hostport;
	unsigned int              m_serverSessinid;

	std::string               m_regGuid;        /*ע��ʱ��guid*/
	int                       m_regCount;       /*ע�᳢�Դ���*/
	U64                       m_lastHeartTime;  /*����������Ӧʱ��*/
	bool                      m_bHeart;         /*�Ƿ��������*/

	bool                      m_bHeartTime;     /*������ʱ������*/

	RecvUdpCmdInterfaceCtrl   *m_recv;

	unsigned int               m_sendShareInfoFlag;


	bool                      m_bShareip;       /*�Ƿ񷢲��ɹ�*/
	std::string               m_bShareguid;     /*������guid���������ʶ��*/
	bool                      m_bShareTimeStart;/*������ʱ������*/
	std::string               m_shareip;        /*�����ĵ�ַ*/
	int                       m_shareport;
	std::string               m_userid;
	std::string               m_name;

	int                       heart_index;    /*��������*/

	std::string               m_token;		  //�ֻ�����token��Ϣ
	std::string               m_checktoken;
	bool                      m_canRegist;

	map<std::string,std::string>    m_recMsgCheck;         //keyΪGUID��valueΪuserid;
	map<string,PerUserRecommendMsg> m_saveRecTaskManage;   //�����Ƽ�������Ϣ
	map<std::string,int>            m_saveRecVersion;      //����UI��ȡͨѶ¼�İ汾��
	 
	std::string						m_remoteIp;			   //��Ļ�����
	unsigned short			        m_remotePort;   
	bool                            m_isSendShareInfo;
	char*                           m_shareInfo;
	int                             m_shareInfoLen;
	int                             m_sendShareCount;

	map<std::string,InviteCmdContext>  m_sendInviteRecord;  //��¼����������
	map<std::string,I64>               m_recvInviteRecord;  //���ܵ�����������͸�UI��ļ�¼map	

	int							   m_recvReqHostRespCount;	//��¼��STPRC����HOST�Ĵ���,�ﵽһ�������ͻ��˿�
	unsigned int                   m_manageSid;             //��¼ManageSeesionID

	unsigned int                   m_registerFlag;

	map<I64,meetingHost_trans_data_ex> m_transData;
	GMLock							   m_transDataLock;
};

