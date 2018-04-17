#include "stdafx.h"
#include "client_session.h"
#include "hostAgentAndSTPRCCmd.h"
#include "innercommand.h"

#include "MeetingHostAgentManager.h"

#define  RESP_TIME_OUT    500

extern MeetingHostAgentManager *g_pmanage;

client_session::client_session(const char* ip, const U16 port,  const char* name,unsigned int sid):m_port(port),
	meetingSrcSid(0),
	hasResped(false),
	reqId(0)
{
	if (NULL != name)
		m_name.assign(name);

	if (NULL != ip)
	{
		memcpy(m_ip,ip,16);
		m_ip[15]=0;
	}
	else
		m_ip[0]=0;

	if (sid != 0)
	{
		m_manageSid = sid;
	}
}

client_session::client_session(const char* ip, const U16 port,  const char* name):m_port(port),
												meetingSrcSid(0),
												hasResped(false),
												reqId(0)
{
	if (NULL != name)
		m_name.assign(name);

	if (NULL != ip)
	{
		memcpy(m_ip,ip,16);
		m_ip[15]=0;
	}
	else
		m_ip[0]=0;
}


client_session::~client_session(void)
{
	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]client_session����");
}


void client_session::get_meeting_control_server_request()
{
	//���͵���Ϣ
	AsynModel::ConnID connid;
	connid.hosttype = AsynModel::ConnID::CONNID_OPEN_;
	memcpy(connid.openhost.openUDPIP,m_ip,16);
	connid.openhost.openUdpPort = m_port;

#ifdef  USE_RAW_SOCKET 
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,GET_HOST_SERVER_REQ_CMD,NULL,0);
#else
	AsynModel::AsynResultRet ret = PostMsg(connid,0,GET_HOST_SERVER_REQ_CMD, NULL, 0U, true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
#endif
	if (ret != AsynModel::SUCCESS)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,ͬ������ʧ��,ret=%d",ret);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,ͬ�����ͳɹ�,ret=%d",ret);
	}
}

int client_session::on_get_meeting_control_server_response(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{

	if(meetingSrcSid != 0 && !hasResped)
	{
		PostMsg(meetingSrcSid,HOSTAGENT_GETHOSTADDR_RESP,(char*)data,len);

		hasResped = true;
		meetingSrcSid = 0;
	}
	return 0;
}

int client_session::on_get_meeting_control_server_req(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(data == NULL)
		return -1;
	
	meetingSrcSid = *(int*)(data);  //getsessionID()���
	hasResped     = false;       
	int newReqId  = ++reqId;  
	
	get_meeting_control_server_request();

	ReqUserData* pData = new(nothrow) ReqUserData();
	if(pData == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,new MsgUserData�����ڴ�ʧ��,���ȣ�%d",sizeof(ReqUserData));
		return -1;
	}

	pData->reqId = newReqId;

	SetTimer(1,RESP_TIME_OUT,pData);
	return 0;
}

int client_session::respTimerOutTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	ReqUserData* pData = (ReqUserData* )pUserTimeDataInfo;
	if(pData ==NULL)
		return -1;

	if(!hasResped && pData->reqId == reqId)
	{
		char buff[512] = {0};
		GetHostServerRespCmd cmd;
		cmd.result = -100;	//��ʱ
		int slen = cmd.Seralize(buff,512);
		if(slen < 0)
		{
			pData->Release();
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,׼�����ͳ�ʱ��Ӧʱ��GetHostServerRespCmd���л�ʧ�� slen:%u",slen);
			return -1;
		}
		
		PostMsg(meetingSrcSid,HOSTAGENT_GETHOSTADDR_RESP,(char*)buff,slen);

		hasResped     = true;
		meetingSrcSid = 0;

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,׼�����ͳ�ʱ��Ӧʱ,��ǰreqId:%d Timer��reqId:%d",reqId,pData->reqId);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��·������host��ַ,��ʱ������,Ӧ��ɹ�,hasResped:%s ��ǰreqId:%d Timer��reqId:%d",
			hasResped?"true":"false",reqId,pData->reqId);
	}

	pData->Release();
	
	return 0;
}

int client_session::onExit(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	Terminate(0);
	return 0;
}

int client_session::OnPostMsgRecept(AsynModel::ReceptType receptType,
	AsynModel::ReceptResult receptResult,
	U32 msgId,
	const char *data, 
	U32 len, 
	U32 destSid,
	AsynModel::Session::ContactType contactType,
	AsynModel::ConnID& connid,
	const AsynModel::SptrISender& sender,
	AsynModel::UserDataInfo* userPara,
	U32 localIP,
	U16 localPort,
	bool isReliable)
{
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��·������host��ַ,���Ļ�Ӧ:receptType:%d,receptResult:%d,msgId:%d",
		receptType,receptResult,msgId);
#ifndef  USE_RAW_SOCKET 
	if(GET_HOST_SERVER_REQ_CMD == msgId)
	{
		if(receptResult != AsynModel::RECEPT_SUCCESS)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��·������host��ַ,��ִʧ��,receptResult:%d",receptResult);

		}
	}
#endif
	return 0;
}


