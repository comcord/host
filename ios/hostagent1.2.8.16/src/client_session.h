#pragma once
#include "stdafx.h"
#include "hostAgentAndSTPRCCmd.h"
#include "innercommand.h"
struct ReqUserData :public AsynModel::UserDataInfo,public GMMemAlloc<ReqUserData, GMPrivateHeapMemAllocMethod>
{
	ReqUserData():reqId(0)
	{}
	
	~ReqUserData()
	{}

	int  reqId;
};

#define  ROUTE_GET_HOST_REQ   10
#define  ROUTE_GET_HOST_RESP   1

class client_session: public AsynModel::Session
{
public:
	client_session(const char* ip, const U16 port,const char* name,unsigned int sid);
	client_session(const char* ip, const U16 port,const char* name);
	~client_session(void);

	void get_meeting_control_server_request();
	
	BEGIN_MSG_MAP(client_session)
		MSG_HANDLER(GET_HOST_SERVER_RESP_CMD,on_get_meeting_control_server_response)
		MSG_HANDLER(HOSTAGENT_GETHOSTADDR_REQ,on_get_meeting_control_server_req)
		MSG_HANDLER(MEETINGHOST_EXIT,onExit)
	END_MSG_MAP()

	BEGIN_TIMER_MAP(client_session)
		TIMER_HANDLER(1,respTimerOutTimer);
	END_TIMER_MAP()

	/*sessingÍË³ö*/
	int onExit(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	
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

protected:

	int on_get_meeting_control_server_response(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int on_get_meeting_control_server_req(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

	int     respTimerOutTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo);
	int     meetingSrcSid;
	bool    hasResped;
	int     reqId;

	string	m_name;
	char	m_ip[16];
	U16		m_port;

	unsigned int m_manageSid;
};
