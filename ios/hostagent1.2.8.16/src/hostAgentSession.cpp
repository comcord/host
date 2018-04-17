#include "stdafx.h"
#include "hostAgentSession.h"
#include "hostAgentAndSTPRCCmd.h"
#include "meetingHostCmd.h"
#include "innercommand.h"

#include "MeetingHostAgentManager.h"
#include "configParser.h"

#include <set>

#ifndef WIN32
#ifdef __IOS__
#include <uuid32_ios.h>
#else
#include "uuid32.h"
#endif
#include <unistd.h>
#	ifndef DWORD
#		define DWORD unsigned int
#	endif
#	ifndef GUID
#ifdef __IOS__
#		define GUID ios_uuid_t
#else
#   define GUID uuid_t
#endif
#	endif
#endif

#define  RESP_TIME_OUT    (2*1000)

extern std::string g_recMsglocalpath;
extern std::string g_recRecordlocalpath;    
extern std::string g_inviteRecordlocalpath; 
extern std::string g_shareScreenConfigPath;

extern MeetingHostAgentManager *g_pmanage;
extern CConfigParser_STPIPMAP g_ConfigParser;

static void ip_ulong_to_str(unsigned long ip, char ip_str[16])
{
	unsigned long temp = htonl(ip);
	inet_ntop(AF_INET, &temp, ip_str, 16);
	return;
}

std::string GetGuidStr()
{
#ifdef WIN32
	GUID guid;
	CoCreateGuid(&guid);
	string guid_str;
	char buff[37] = {0};
	_snprintf(buff,sizeof(buff),"%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0],guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	guid_str = buff;
	return guid_str;
#else

	string guid_str;
	GUID guid;
	char buff[37] = {0};

	//#ifdef android
	uuid_create(&guid);
	_snprintf(buff,sizeof(buff),"%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.data1,
		guid.data2,
		guid.data3,
		guid.data4[0],guid.data4[1],guid.data4[2],guid.data4[3],guid.data4[4],guid.data4[5],guid.data4[6],guid.data4[7]);

	guid_str = buff;

	return guid_str;

#endif
}

int	hostAgentSession::OnStart()
{
	//Session����,���ر����Ƽ���Ϣ���ڴ�
	std::multimap<std::string,Contact> msg;		   //keyΪuserid+guid
	std::map<string,PerUserRecommendMsg> recMsg;   //keyΪuserid+guid

	readAllRecMsgFromDisk(g_recMsglocalpath,msg);

#if 1
	if (msg.size() != 0)
	{
		//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]��ȡ���ش��ڵ�����,�洢�Ƽ���ϢMULTIMAP�Ĵ�СΪ:%d",msg.size());
		
		//multimapת��Ϊmap
		tansToPerUerRecMsgMap(msg,recMsg);

		map<string,PerUserRecommendMsg>::iterator iter;
		for (iter = recMsg.begin(); iter != recMsg.end();++iter)
		{
			m_saveRecTaskManage[iter->first] = iter->second;

			//֪ͨUI
			sendRecommendInfoToUI(iter->second);
		}
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]�������Ƽ���Ϣ");
	}

	//��ȡ���ص��Ƽ���ʷ��¼
	readRecRecordMsgFromDisk(g_recRecordlocalpath,m_saveRecVersion);
#endif

	SetTimer(__MEETINGHOSTAGENT_TIME_SETLOCAL,2000);
	SetTimer(__MEETINGHOSTAGENT_TIME_INVITEANDCHECK,INVITE_CHECK_TIME_STEP);

	//������������ʱ��
	SetTimer(__MEETINGHOSTAGENT_TIME_SENDSHAREINFOTOHOST,10);

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDTRANSDATA,100);

	return 0;
}


int hostAgentSession::sendRecommendInfoToUI(PerUserRecommendMsg msg)
{
	std::string respStr;
	Json::Value respRoot;
	respRoot["cmdid"]              = 4001;
	respRoot["sourceid"]           = msg.sourceId;                 //�Ƽ���
	respRoot["contactlistid"]      = msg.contactListId;            //ͨѶ¼id
	respRoot["contactlistversion"] = msg.contactListVersion;       //ͨѶ¼�汾��
	respRoot["contactlistname"]    = msg.contactListName;	       //ͨѶ¼����
#ifdef __IOS__
	respRoot["contactersnum"]      = (int)msg.contacters.size();    //��ϵ�˸���
#else
  respRoot["contactersnum"]        = (int)msg.contacters.size();         //��ϵ�˸���
#endif

	Json_Util_Write(respRoot,respStr);

	if (respStr.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]�����Ƽ���Ϣ��UI,����Ϊ��");
		return -1;
	}

	sendCmdToUI((char*)respStr.c_str(),respStr.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][RECOMMENDTION]�����Ƽ���Ϣ��UI,����Ϊ:%s",respStr.c_str());

	return 0;
}

hostAgentSession::hostAgentSession()
{
	m_bHeart         = true;
	m_bHeartTime     = false;
	m_serverSessinid = 0;
	m_hostport       = 0;
	m_lastHeartTime  = 0;
	m_reqHostCount   = 0;
	m_regCount       = 0;
	
	m_shareport      = 0;
	m_sessionState   = __MEETINGHOSTAGENT_SESSIONSTATE_NO;
	heart_index      = 0;
	m_recv           = NULL;

	m_canRegist       = false;
	m_isSendShareInfo = false;
	m_shareInfo       = NULL;
	m_shareInfoLen    = 0;
	m_sendShareCount  = 0;

	m_recvReqHostRespCount = 0;


	m_sendShareInfoFlag = 0;

	m_registerFlag      = 0;
}
hostAgentSession::~hostAgentSession(void)
{
	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]hostAgentSession����");
}

bool hostAgentSession::sendCmdToUI(char *buf,int len)
{
    return m_recv->UdpSend(buf,len);
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��UI��������,����Ϊ:%s",buf);
}

int hostAgentSession::onRegister(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_NO != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����session��ǰ����״̬�ǣ�__MEETINGHOSTAGENT_SESSIONSTATE_NO)");
		return -1;
	}
	/*����·������session*/
	if(true != CreateGetRoutServerSession(m_rcAddr))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][CreateGetRoutServerSession]����ʧ��,rcaddr:%s",m_rcAddr.c_str());
		return -1;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][CreateGetRoutServerSession]���óɹ�");
	}

	/*���һ��·������host*/
	StartRequestHost();
	m_recvReqHostRespCount = 0;
	return 0;
}

/*ͨ��ָ��·�ɻ�ȡhost*/
void  hostAgentSession::StartRequestHost()
{
	if(m_canRegist)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�յ���������,ֹͣ��ȡhost");
		return;
	}
	
	m_sessionState   = __MEETINGHOSTAGENT_SESSIONSTATE_HOST;
	unsigned int sid = GetSessionID();
	m_reqHostCount	 = 0;

	int ret = PostMsg(m_rcClientStpr1->GetSessionID(),HOSTAGENT_GETHOSTADDR_REQ,(char*)&sid,sizeof(sid),false);
	if (ret !=0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent]��client_session1���ͻ�ȡ����Host����,ͬ��ʧ��,addr:(%s|%d),ret:%d",m_routeServerIp1.c_str(),m_routeServerPort1,ret);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��client_session1���ͻ�ȡ����Host����,ͬ���ɹ�,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
	}
}

bool hostAgentSession::ContinueRequestHost()
{
	m_sessionState = __MEETINGHOSTAGENT_SESSIONSTATE_HOST;

	if(0 == m_reqHostCount)
	{
		unsigned int sid = GetSessionID();
		m_reqHostCount = 1;

		int ret = PostMsg(m_rcClientStpr2->GetSessionID(),HOSTAGENT_GETHOSTADDR_REQ,(char*)&sid,sizeof(sid),false);
		if (ret !=0)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent]��client_session2���ͻ�ȡ����Host����,ͬ��ʧ��,addr:(%s|%d),ret:%d",m_routeServerIp2.c_str(),m_routeServerPort2,ret);
		}
		else
		{
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��client_session2���ͻ�ȡ����Host����,ͬ���ɹ�,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
		}

		return true;
	}

	return false;
}

bool  hostAgentSession::CreateGetRoutServerSession(const std::string & routeUrl)
{
	bool bSucc = GetRouteServerAddrFromUrl(routeUrl);
	if(!bSucc)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][GetRouteServerAddrFromUrl]����ʧ��,routeUrl:%s",routeUrl.c_str());
		return false;
	}

	//��һ��·��server
#ifdef USE_RAW_SOCKET
	m_rcClientStpr1 = new(nothrow) client_session(m_routeServerIp1.c_str(),m_routeServerPort1,"routeServer1",m_manageSid);
#else
	m_rcClientStpr1 = new(nothrow) client_session(m_routeServerIp1.c_str(),m_routeServerPort1,"routeServer1");
#endif
	if (NULL == m_rcClientStpr1)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1����ʧ��,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
	}
	
	int ret = m_rcClientStpr1->Start();
	if(ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1����ʧ��,addr:(%s|%d),ret:%d",m_routeServerIp1.c_str(),m_routeServerPort1,ret);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1�����ɹ�,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
	}
	
    //�ڶ���·��server
#ifdef USE_RAW_SOCKET
	m_rcClientStpr2 = new(nothrow) client_session(m_routeServerIp2.c_str(),m_routeServerPort2,"routeServer2",m_manageSid);
#else
	m_rcClientStpr2 = new(nothrow) client_session(m_routeServerIp2.c_str(),m_routeServerPort2,"routeServer2");
#endif
	if (NULL == m_rcClientStpr2)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2����ʧ��,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
		return false;
	}
	
	ret = m_rcClientStpr2->Start();
	if(ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2����ʧ��,addr:(%s|%d)ret:%d",m_routeServerIp2.c_str(),m_routeServerPort2,ret);
		return false;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2,�����ɹ�,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
	}

	return true;
}

int hostAgentSession::onReqHostResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_HOST != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ǰSESSION��__MEETINGHOSTAGENT_SESSIONSTATE_HOST״̬!");
		return -1;
	}

	GetHostServerRespCmd cmd;
	int ret = cmd.UnSeralize((char*)data,len);
	if (ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ȡhost��ַ,�����л�ʧ�� len:%u",len);
		return -1;
	}

	if(0 == cmd.result)
	{
		//ip ӳ�� port
		string tmpIP = cmd.userIp;
		HandleStpIPMap(tmpIP, cmd.userPort);

		//����ע�ᶨʱ�����Ʊ�־
		if(0 == m_registerFlag)
		{
			/*���host�ɹ�*/
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ȡhost��ַ,�ɹ�,userid=%s,IP:%s,Port:%u",m_userid.c_str(),tmpIP.c_str(),cmd.userPort);

			m_hostip       = tmpIP;
			m_hostport     = cmd.userPort;
			m_sessionState = __MEETINGHOSTAGENT_SESSIONSTATE_REG;
			m_regCount     = 0;

			m_regGuid = GetGuidStr();	//һ��ע�ᱣ��һ��GUID

			/*��host����ע������*/
			sendRegisterToServer();
			
			m_registerFlag = 1;
			/*������ʱ���ע����*/
			SetTimer(__MEETINGHOSTAGENT_TIME_REG,1000);
			return 0;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ȡhost��ַ,�ɹ����Ѿ�������ע��,userid=%s,IP:%s,Port:%u",m_userid.c_str(),tmpIP.c_str(),cmd.userPort);
		}
	}
	else
	{
		//�ݶ�30��û���յ���Ӧ�ͻ��˿�
		if (m_recvReqHostRespCount > 20)
		{
#ifdef USE_RAW_SOCKET
			PostMsg(m_manageSid,MEETINGMANAGE_SEESION_RECREATE,NULL,0);
#else
			//Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��STPRC����Host�ﵽ20��δ�ɹ�,�������豾�ض˿�,localIP:%s,udpPort:%d,tcpPort:%d",
			//	g_pmanage->m_localip.c_str(),g_pmanage->m_localport1,g_pmanage->m_localport2);

			////���°󶨶˿�
			//int tmp_udp_port = 0;
			//int tmp_tcp_port = 0;
			//g_pmanage->resetUdpLocalipandPort(g_pmanage->m_localip,g_pmanage->m_localport1,tmp_udp_port);
			//g_pmanage->resetTcpLocalipandPort(g_pmanage->m_localip,g_pmanage->m_localport2,tmp_tcp_port);

			//g_pmanage->m_localport1 = tmp_udp_port;
			//g_pmanage->m_localport2 = tmp_tcp_port;
#endif
			//��������Host
			m_recvReqHostRespCount = 0;
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ȡhost��ַ,��������20δ�ɹ�,�ؽ�SOCKET");

		}
		else
		{
			m_recvReqHostRespCount++;
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��ȡhost��ַ,����Ϊ:%d",m_recvReqHostRespCount);
		}

		/*������һ��·�ɵ�ַ*/
		if(true != ContinueRequestHost())
		{
			StartRequestHost();
		}
	}
	return 0;
}

int hostAgentSession::sendRegisterToServer()
{
	if (0 == m_hostport || m_hostip.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��host����ע������,host��ַ��Ч");
		return -1;
	}

	AsynModel::ConnID connid = GetHostServerConnID();

	char buf[256] = {0};
	if (!m_checktoken.empty())
	{
		meetingHost_cmd_reg regCmd;

		//m_regGuid = GetGuidStr();  
		strncpy(regCmd._guid,m_regGuid.c_str(), sizeof(regCmd._guid)-1);
		strncpy(regCmd._userid,m_userid.c_str(),sizeof(regCmd._userid)-1);
		regCmd._token = m_checktoken;

		int len = regCmd.serialize(buf,sizeof(buf));
		if(0 > len)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��host����ע������,���л�ʧ��");
			return -1;
		}

#ifdef USE_RAW_SOCKET
		int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_REGISTER,buf,len);
#else
		AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_REGISTER,buf,len,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
		if (0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]��host����ע������,ͬ��ʧ��,userid=%s,(%s:%d),����:%d",regCmd._userid,m_hostip.c_str(),m_hostport,m_regCount);
			return -1;
		}

		m_regCount++;     //ע�᳢�Դ���
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]��host����ע������,ͬ���ɹ�,token�ǿ�,userid=%s,(%s:%d),����:%d",regCmd._userid,m_hostip.c_str(),m_hostport,m_regCount);
	}
	else
	{
		meetingHost_cmd_reg_old regCmdOld;

		//m_regGuid = GetGuidStr();
		strncpy(regCmdOld._guid,m_regGuid.c_str(), sizeof(regCmdOld._guid)-1);
		strncpy(regCmdOld._userid,m_userid.c_str(),sizeof(regCmdOld._userid)-1);

		int slen = regCmdOld.serialize(buf,sizeof(buf));
		if(0 > slen)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��host����ע������,���л�ʧ��");
			return -1;
		}

#ifdef USE_RAW_SOCKET
		int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_REGISTER,buf,slen);
#else
		AsynModel::AsynResultRet ret = PostMsg(connid, 0,MEETINGHOST_CMD_REGISTER, buf, slen, false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
		if (0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]��host����ע������,ͬ��ʧ��,userid=%s,(%s:%d),����:%d",regCmdOld._userid,m_hostip.c_str(),m_hostport,m_regCount);
			return -1;
		}

		m_regCount++;  //ע�᳢�Դ���
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]��host����ע������,ͬ���ɹ�,tokenΪ��,userid=%s,(%s:%d),����:%d",regCmdOld._userid,m_hostip.c_str(),m_hostport,m_regCount);
	}

	return 0;
}

int hostAgentSession::onRegisterResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_REG != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�ע������Ļ�Ӧ,��ע��״̬�Ѿ�����.ע��״ֵ̬:%d,��ǰ״ֵ̬:%d",__MEETINGHOSTAGENT_SESSIONSTATE_REG, m_sessionState);
		return -1;
	}

	meetingHost_cmd_reg_resp respCmd;
	int unlen = respCmd.unserialize((char*)data,len);
	if(unlen <0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�ע������Ļ�Ӧ,�����л�ʧ��");
		return -1;
	}

	/*�ж��Ƿ�����������guid*/
	if(m_regGuid != respCmd._guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�ע������Ļ�Ӧ,guidУ�����.recv_guid:%s,local_guid:%s",
			respCmd._guid, m_regGuid.c_str());
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�ע��ɹ���Ӧ,userid:%s,recv_guid:%s,local_guid:%s",m_userid.c_str(),respCmd._guid, m_regGuid.c_str());

#ifdef USE_RAW_SOCKET
	meeting_syncHead_ext *pext = (meeting_syncHead_ext *)(data + unlen);
	m_serverSessinid           = pext->ssrcid;
#else
	m_serverSessinid = srcsid;     //������host������sessionID���������Բ������������н���
#endif

	m_sessionState = __MEETINGHOSTAGENT_SESSIONSTATE_HEART;
	if(false == m_bHeartTime)
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�ע��ɹ���Ӧ,����������ʱ��,userid:%s",m_userid.c_str());
		
		/*��������*/
		m_lastHeartTime = GetTickCount64();
		heart_index     = 0;
		m_bHeartTime    = true;
		SetTimer(__MEETINGHOSTAGENT_TIME_HEART,10);
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�ע��ɹ���Ӧ,������ʱ���Ѿ���������״̬,userid:%s",m_userid.c_str());
	}

	/*ע��ɹ������������ַ*/
	//sendShareip();

	//��Ҫ��������ʱ,����ע��ɹ���Ҫ����
	//if (0 == m_sendShareInfoFlag)
	{
		if (0 != m_shareport && !m_shareip.empty())
		{
			m_sendShareInfoFlag = 1;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�ע��ɹ���Ӧ,����δ������ʶ,�������ַ��Ч,�����з���");
		}
	}
	
	return 0;
}

int hostAgentSession::OnTimerHeart(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	m_bHeartTime = false;

	if(!m_bHeart)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]���յ���������,����ֹͣ,user:%s,host(%s:%u)",m_userid.c_str(),m_hostip.c_str(),m_hostport);
		return -1;
	}

	U64  timediff = GetTickCount64() - m_lastHeartTime;
	if (timediff > 3000 && timediff < 10000)
	{
		sendheart();
		m_bHeartTime = true;
		SetTimer(__MEETINGHOSTAGENT_TIME_HEART,300);
		return 0;
	}
	else if(timediff > 10000)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]������ʱ,ֹͣ���� user:%s,host(%s:%u)",m_userid.c_str(),m_hostip.c_str(),m_hostport);

		/*���»�ȡ·��*/
		StartRequestHost();

		m_hostport = 0;
		/*����ֹͣ����*/
		m_bShareip = true;


		m_sendShareInfoFlag = 0;

		return 0;
	}

	/*������������*/
	sendheart();

	m_bHeartTime = true;
	SetTimer(__MEETINGHOSTAGENT_TIME_HEART,1000);
	return 0;
}

int hostAgentSession::OnTimerRegister(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	m_registerFlag = 0;
	if (m_canRegist)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]�յ���������,ֹͣע�ᶨʱ��");
		return -1;
	}
	
	/*���ע����*/
	if(__MEETINGHOSTAGENT_SESSIONSTATE_REG == m_sessionState)
	{
		if(m_regCount < 10)
		{
			/*����ע������*/
			sendRegisterToServer();
			
			m_registerFlag = 1;
			SetTimer(__MEETINGHOSTAGENT_TIME_REG,1000);
		}
		else
		{
			/*��������host����*/
			StartRequestHost();
		}
	}
	return 0;
}

int hostAgentSession::onInvite(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	std::string respStr;
	Json::Value respRoot;
	//if(__MEETINGHOSTAGENT_SESSIONSTATE_HEART == m_sessionState)
	{
		/*��hostServer������������*/
		if (0 == m_hostport || m_hostip.empty())
		{
			/*���������*/
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�UI��������,host��ַ��Ч,userid=%s",m_userid.c_str());
			CompositingCmd(respRoot,respStr,3005,-2,"��������");
			sendCmdToUI((char*)respStr.c_str(),respStr.length());
			return -1;
		}

		AsynModel::ConnID connid = GetHostServerConnID();
		Json::Value root;
		if(true != Json_Util_Open(root,data))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[Host]���յ�UI��������,����ʧ��");
			CompositingCmd(respRoot,respStr,3005,-3,"��������");
			sendCmdToUI((char*)respStr.c_str(),respStr.length());
			return -1;
		}

		CompositingCmd(respRoot,respStr,3005,0,"��������");
		sendCmdToUI((char*)respStr.c_str(),respStr.length());
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�UI��������,���ظ��ɹ�");

		std::string name   = root["name"].asString();
		std::string userid = root["accountid"].asString();
		int         meetid = root["meetingid"].asInt();
		int         count  = root["accounted"].size();

		for (int i = 0; i < count ;i++)
		{
			std::string tempUserid;
			tempUserid = root["accounted"][i].asString();

			std::string tmpGuid = GetGuidStr();

			Json::Value sendInvite;
			std::string result;

			sendInvite["accountid"] = Json::Value(userid);
			sendInvite["accounted"] = Json::Value(tempUserid);
			sendInvite["name"]      = Json::Value(name);
			sendInvite["meetingid"] = Json::Value(meetid);
			sendInvite["guid"]      = Json::Value(tmpGuid);
			
			if (true != Json_Util_Write(sendInvite,result))
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ�UI��������,�����������,��ʽ��jsonʧ��,accounted:%s,name:%s,accountid:%s,meetingid:%d,guid:%s",
				                                                    tempUserid.c_str(),name.c_str(),userid.c_str(),meetid,tmpGuid.c_str());
				return -1;
			}

			//���浽�����ڴ�,�ȴ��ظ�;��������ط�
			InviteCmdContext cmd;
			cmd.meetingid = meetid;
			memcpy(cmd.InviteData,result.c_str(),result.length());
			m_sendInviteRecord[tmpGuid] = cmd;

#ifdef USE_RAW_SOCKET 
			int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_INVITE_SINGLE,cmd.InviteData,sizeof(cmd.InviteData));
#else
			AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_INVITE_SINGLE,cmd.InviteData,sizeof(cmd.InviteData),true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
#endif
			if (0 != ret)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�UI��������,��host������������ͬ��ʧ��,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
				return -1;
			}

			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ�UI��������,��Host������������ɹ�,addr:%s:%d,accounted:%s,name:%s,accountid:%s,meetingid:%d,guid:%d",
			                                                                   m_hostip.c_str(),m_hostport,tempUserid.c_str(),name.c_str(),userid.c_str(),meetid,tmpGuid.c_str());
			
			
			/************************************************************************/
			/*                      ��¼��������ļ�¼                             */
			/************************************************************************/
			
			char recordName[128] = {0};
			sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

			FILE* pInviteEventRecord = fopen(recordName,"a+");
			if (pInviteEventRecord == NULL)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]�ļ� %s �޷���",recordName);
				return 0;
			}

			Date tmp_now;
			char inviteMsg[512] = {0};
			sprintf(inviteMsg,"Time:%s|InviteId:%s|InvitedId:%s|Guid:%s|MeetingId:%d|SendInvite\r\n",tmp_now.toString("yyyymmddhhmi").c_str(),m_userid.c_str(),tempUserid.c_str(),tmpGuid.c_str(),meetid);
			if (inviteMsg != NULL)
			{
				fwrite(inviteMsg,sizeof(char),strlen(inviteMsg),pInviteEventRecord);
				fflush(pInviteEventRecord);
				fclose(pInviteEventRecord);
			}

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
		
		}
	}
	//else
	//{
		/*��ui����ע��ʧ��*/
		//if (0 == CompositingCmd(respRoot,respStr,3005,-2,"onInvite"))
		//{
		//	sendCmdToUI((char*)respStr.c_str(),respStr.length());
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]����������,�û�δע��");
		//}
		//else
		//{
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��UI���������ʽ��ʧ��(����������ʱδע��),sesseionδ��������״̬");
		//	return -1;
		//}
	//} 
	
	return 0;
}

int hostAgentSession::onInviteSingleResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_invitedCmd_Single_Resp inviteRespCmd;
	int unlen = inviteRespCmd.UnSeralize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ�����ظ�����,�����л�ʧ��");
	}

	std::string guid = inviteRespCmd.guid;
	std::map<std::string,InviteCmdContext>::iterator it = m_sendInviteRecord.find(guid);
	if (it != m_sendInviteRecord.end())
	{
		std::string tmp_guid = it->first;
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ�����ظ�����,���ش�����Ӧ���������¼,guid:%s ������:%s �����:%d,���ؼ�¼Ϊ guid:%s meetingid:%d",inviteRespCmd.guid,inviteRespCmd.invitid,inviteRespCmd.meetid,tmp_guid.c_str(),it->second.meetingid);
		
		/************************************************************************/
		/*                  �յ�����ظ��ļ�¼                               */
		/************************************************************************/

		char recordName[128] = {0};
		sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

		FILE* pInviteEventRecord = fopen(recordName,"a+");
		if (pInviteEventRecord == NULL)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]�ļ� %s �޷���",recordName);
			return 0;
		}

		Json ::Value root;
		if (true != Json_Util_Open(root,it->second.InviteData))
		{
			return -1;
		}

		std::string invitedID = root["accounted"].asString();

		Date tmp_now;
		char inviteMsg[512] = {0};
		sprintf(inviteMsg,"Time:%s|InviteId:%s|InvitedId:%s|Guid:%s|MeetingId:%d|InviteResp\r\n",tmp_now.toString("yyyymmddhhmi").c_str(),inviteRespCmd.invitid,invitedID.c_str(),inviteRespCmd.guid,inviteRespCmd.meetid);
		if (inviteMsg != NULL)
		{
			fwrite(inviteMsg,sizeof(char),strlen(inviteMsg),pInviteEventRecord);
			fflush(pInviteEventRecord);
			fclose(pInviteEventRecord);
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		m_sendInviteRecord.erase(tmp_guid);
	}
	else
	{		
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ�����ظ�����,��������Ӧ���������¼,guid:%s ������:%s �����:%d",inviteRespCmd.guid,inviteRespCmd.invitid,inviteRespCmd.meetid);
	}

	return 0;
}

int hostAgentSession::onInvitedSingle(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	//1�����������ظ�server
	//2������GUIDȥ���ұ���map(m_recvInviteRecord)
	//3������,������
	//4�������ڣ�����UI�����洢��map
	meetingHost_invitedCmd_Single inviteCmd;
	int unlen = inviteCmd.UnSeralize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,�����л�ʧ��");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,guid:%s,������:%s,�����:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);

	//�ظ�SERVER->AGENT
	meetingHost_invitedCmd_Single_Resp inviteResp;
	strncpy(inviteResp.guid,inviteCmd.guid,sizeof(inviteResp.guid)-1);
	strncpy(inviteResp.invitid,inviteCmd.inviteid,sizeof(inviteResp.invitid)-1);
	inviteResp.meetid = inviteCmd.meetid;

	char buf[256] = {0};
	int serlen = inviteResp.Seralize(buf,sizeof(buf));
	if (serlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,�ظ���Ϣ���л�ʧ��");
	}

	AsynModel::ConnID server_conn_id;
	memcpy(server_conn_id.openhost.openUDPIP, m_hostip.c_str(),sizeof(server_conn_id.openhost.openUDPIP));
	server_conn_id.openhost.openUdpPort = m_hostport;

#ifdef USE_RAW_SOCKET
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),server_conn_id,m_serverSessinid,MEETINGHOST_CMD_INVITED_SINGLE_RESP,buf,serlen);
#else
	AsynModel::AsynResultRet ret = PostMsg(server_conn_id, m_serverSessinid,MEETINGHOST_CMD_INVITED_SINGLE_RESP,buf,serlen, false, SENMSGDEFAULTTIMROUT, AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
	if (0 != ret)	
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ���������Ϣ,��host��������ظ�����,ͬ��ʧ��,(%s:%d),guid:%s,������:%s,meetingid:%d",server_conn_id.openhost.openUDPIP,m_hostport,inviteResp.guid,inviteResp.invitid,inviteResp.meetid);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ���������Ϣ,��host��������ظ�����,ͬ���ɹ�,(%s:%d),guid:%s,������:%s,meetingid:%d",server_conn_id.openhost.openUDPIP,m_hostport,inviteResp.guid,inviteResp.invitid,inviteResp.meetid);
	}

	//����GUIDȥ���ұ���map(m_recvInviteRecord)
	std::map<std::string,I64>::iterator iter = m_recvInviteRecord.find(inviteCmd.guid);
	if (iter != m_recvInviteRecord.end())
	{
		//���ش��ڼ�¼,������
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,guid���ش���,guid:%s,������:%s,�����:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);
		return -1;
	}
	else
	{
		//���ز�����,���͸�UI����¼������
		Json::Value cmd;
		std::string result;
		if(0 != CompositingCmd(cmd,result,inviteCmd.inviteid,3006,inviteCmd.invitename,inviteCmd.meetid,"onInvited"))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,���͸�UI��װJSONʧ��");
			return -1 ;
		}

		sendCmdToUI((char*)result.c_str(),result.length());
		
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ���������Ϣ,��������ؼ�¼,���͸�UI,guid:%s,������:%s,�����:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);

		//��¼�ѷ���Ϣ
		m_recvInviteRecord[inviteCmd.guid] = GetTickCount64();


		/************************************************************************/
		/*                      ���յ��������¼                                */
		/************************************************************************/
		char recordName[128] = {0};
		sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

		FILE* pInviteEventRecord = fopen(recordName,"a+");
		if (pInviteEventRecord == NULL)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]�ļ� %s �޷���",recordName);
			return 0;
		}

		Date tmp_now;
		char inviteMsg[512] = {0};
		sprintf(inviteMsg,"Time:%s|InviteId:%s|InvitedId:%s|Guid:%s|MeetingId:%d|RecvInvited\r\n",tmp_now.toString("yyyymmddhhmi").c_str(),inviteCmd.inviteid,inviteCmd.invited,inviteCmd.guid,inviteCmd.meetid);
		if (inviteMsg != NULL)
		{
			fwrite(inviteMsg,sizeof(char),strlen(inviteMsg),pInviteEventRecord);
			fflush(pInviteEventRecord);
			fclose(pInviteEventRecord);
		}
	}

	return 0;
}

int hostAgentSession::onInvited(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	/*��UI���ͱ���������*/
	meetingHost_invitedCmd ivcmd;
	int unSerLen = ivcmd.UnSeralize((char*)data,len);
	if (unSerLen <= 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]������������л�ʧ��,serlen = %d",unSerLen);
		return -1;
	}

	Json::Value cmd;
	std::string result;
	if(0 != CompositingCmd(cmd,result,ivcmd.inviteid,3006,ivcmd.invitename,ivcmd.meetid,"onInvited"))
	{
		return -1 ;
	}

	sendCmdToUI((char*)result.c_str(),result.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]�յ�����������,�����͸�UI,inviteid:%s,invitename:%s,meetingid:%d,invited:%s",ivcmd.inviteid,ivcmd.invitename,ivcmd.meetid,ivcmd.invited);

	return 0;
}

int hostAgentSession::onTimerShareInfoToHost(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	if (1 == m_sendShareInfoFlag)
	{
		sendShareInfoToServer();
	}

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDSHAREINFOTOHOST,500);
	return 0;
}

int hostAgentSession::onTimerSendShareInfo(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	if (!m_isSendShareInfo)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,��ʱ��״̬����Ϊfalse");
		return 0;
	}

	if (m_sendShareCount > 20)
	{
		delete [] m_shareInfo;
		m_isSendShareInfo = false;
		m_sendShareCount  = 0;
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,���Դ����ﵽ20,��ʱ��ֹͣ");
		return 0;
	}

	//������һ
	m_sendShareCount++;

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,��ȡ�ļ���Ϊ:%s",g_shareScreenConfigPath.c_str());
	FILE *pfile = fopen(g_shareScreenConfigPath.c_str(),"r+");
	if (pfile == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,��ȡ�ļ�ָ��Ϊ��");
		return 0;
	}

	char buff[32]= {0};
	if(fgets(buff,32,pfile) == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,��ȡ����Ϊ��");
		return 0;
	}
	m_remotePort = atoi(buff);
	fclose(pfile);
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,��ȡ�ļ�Ϊ:%s,port:%d",g_shareScreenConfigPath.c_str(),m_remotePort);

	AsynModel::ConnID connid;
	strncpy(connid.openhost.openUDPIP,m_remoteIp.c_str(),sizeof(connid.openhost.openUDPIP)-1);
	connid.openhost.openUdpPort = m_remotePort;

	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_NOTICEPCADDR,m_shareInfo,m_shareInfoLen,true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
	if (0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,����%s|%d)���ͳɹ�ʧ��",m_remoteIp.c_str(),m_remotePort);
		return -1;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]��ȡ��Ļ�����ַ,����%s|%d)���ͳɹ�",m_remoteIp.c_str(),m_remotePort);
		delete [] m_shareInfo;
		m_isSendShareInfo = false;
		m_sendShareCount  = 0;
	}

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDSHAREINFO,1000);

	return 0;
}

int hostAgentSession::onPuntchRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	digHoleCmdToClientResultMsg msg;
	int unlen = msg.Unserialize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]�յ�����Ϣ,�����л�ʧ��");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]�յ�����Ϣ,userid:%s,publicAddr:(%s|%d)",msg.userid,msg.publicIp,msg.publicPort);

	//������ʱ��,�ı�״̬
	m_isSendShareInfo = true;
	m_sendShareCount  = 0;

	m_shareInfo = new char[len];
	memset(m_shareInfo,0,len);
	memcpy(m_shareInfo,data,len);
	m_shareInfoLen = len;

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDSHAREINFO,10);

	return 0;
}

int hostAgentSession::onRecommendMsgArrive(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	//TCP
	PerUserRecommendMsg recMsg;
	int unLen = recMsg.Unserialize((char*)data,len);
	if (unLen <= 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]���յ��Ƽ���Ϣ,�����л�ʧ��");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][RECOMMENDTION]���յ��Ƽ���Ϣ,��ϢΪuserid:%s,guid:%s,sourceid:%s,contactlistname:%s,contactlistversion:%d",
		                 recMsg.userid.c_str(),recMsg.guid.c_str(),recMsg.sourceId.c_str(),recMsg.contactListName.c_str(),recMsg.contactListVersion);

	//�������ظ�sever�������Ƽ���Ϣ�ɹ�
	recommendResp((char*)recMsg.guid.c_str(),(char*)recMsg.userid.c_str(),srcsid,sender);

	//���浽�ڴ�
	//�ȼ�����Ƽ���¼,����У�飺���ھͱȽϰ汾��,����汾���µģ�������,�鿴���ؼ�¼���Ƚϰ汾��
	std::map<std::string,int>::iterator iter = m_saveRecVersion.find(recMsg.contactListId);
	if (iter != m_saveRecVersion.end())
	{
		if (recMsg.contactListVersion > iter->second)
		{
			m_saveRecTaskManage[recMsg.contactListId] = recMsg;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]���յ��Ƽ���Ϣ,���Ƽ���¼�д��ڸ�ͨѶ¼ID�Ƽ���Ϣ,���յ��İ汾�ŽϾ�,id:%s,name:%s,%localversion:%d,arrversion:%d",
				               recMsg.contactListId.c_str(),recMsg.contactListName.c_str(),iter->second,recMsg.contactListVersion);
			return 0;
		}
	}
	else
	{
		std::map<std::string,PerUserRecommendMsg>::iterator it = m_saveRecTaskManage.find(recMsg.contactListId);
		if (it != m_saveRecTaskManage.end())
		{
			if (recMsg.contactListVersion > it->second.contactListVersion)
			{
				m_saveRecTaskManage[recMsg.contactListId] = recMsg;
			}
			else
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ���Ϣ,���Ƽ���¼�в�����,���ؼ�¼����,���յ��İ汾�ŽϾ�,id:%s,name:%s,%localversion:%d,arrversion:%d",
					recMsg.contactListId.c_str(),recMsg.contactListName.c_str(),it->second.contactListVersion,recMsg.contactListVersion);
				
				return 0;
			}
		}
		else
		{
			m_saveRecTaskManage[recMsg.contactListId] = recMsg;
		}
	}
	
	sendRecommendInfoToUI(recMsg);

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ���Ϣ,���Ƽ���Ϊ%s,�Ƽ��ͻ���:%s,GUID:%s,�Ƽ���ϵ�˸���:%d,�����л���С:%d",recMsg.userid.c_str(),recMsg.sourceId.c_str(),recMsg.guid.c_str(),recMsg.contacters.size(),unLen);
	return 0;
}

int hostAgentSession::onRecommendRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	NotifyRecommendMsg recNotify;
	int unlen = recNotify.Unserialize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,�����л�ʧ��");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,userid:%s,guid:%s",recNotify.userid.c_str(),recNotify.guid.c_str());

	std::map<std::string,std::string>::iterator it = m_recMsgCheck.find(recNotify.guid);
	if (it != m_recMsgCheck.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,���δ��ڸ��Ƽ���¼,Ϊ�ظ��Ƽ�,guid:%s",recNotify.guid.c_str());
		return -1;
	}

	m_recMsgCheck[recNotify.guid] = recNotify.userid;

	GetRecommendMsg recNotifyResp;
	recNotifyResp.userid = recNotify.userid;
	recNotifyResp.guid   = recNotify.guid;

	char buf[128] = {0};
	int serlen = recNotifyResp.Serialize(buf,sizeof(buf));
	if (serlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,��SERVER���ͻ�ȡ�Ƽ���Ϣ,�����л�ʧ��,userid:%s,guid:%s",recNotify.userid.c_str(),recNotify.guid.c_str());
		return -1;
	}

	//TCP�������ӣ���ȡ�Ƽ���Ϣ
	AsynModel::ConnID connid;
	memset(&connid,0,sizeof(connid));
	strncpy(connid.openhost.openTCPIP,m_hostip.c_str(),sizeof(connid.openhost.openTCPIP)-1);
	connid.openhost.openTcpPort = m_hostport;

	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_GET_RECOMMENDMSG_REQUEST,buf,serlen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::TCP);
	if(0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,��server���ͻ�ȡ�Ƽ���Ϣ����ʧ��,userid:%s,host:(%s|%d)",recNotifyResp.userid.c_str(),m_hostip.c_str(),m_hostport);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]���յ��Ƽ�����֪ͨ����,��server���ͻ�ȡ�Ƽ���Ϣ����ɹ�,userid:%s,host:(%s|%d)",recNotifyResp.userid.c_str(),m_hostip.c_str(),m_hostport);
	}

	return 0;
}

int hostAgentSession::onStop(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	delRCSession();
	Terminate(0);
	return 0;
}

void  hostAgentSession::delRCSession()
{
	if(NULL != m_rcClientStpr1)
	{
		PostMsg(m_rcClientStpr1->GetSessionID(),MEETINGHOST_EXIT,NULL,0,false);
		m_rcClientStpr1 = NULL;
	}

	if(NULL != m_rcClientStpr2)
	{
		PostMsg(m_rcClientStpr2->GetSessionID(),MEETINGHOST_EXIT,NULL,0,false);
		m_rcClientStpr2 = NULL;
	}
}

int hostAgentSession::onHeartResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_cmd_HeartBeatRespMsg Response;
	if(Response.unSerialBuff((char*)data,len) < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�������Ӧ,������У�����,��������,addr:%s:%d", m_hostip.c_str(), m_hostport);
		return 0;
	}

	meetingHost_cmd_HeartBeatRespMsg* pResponse = &Response;
	/*�ж�guid�Ƿ���ͬ*/
	if (string(pResponse->_guid) != m_regGuid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�������Ӧ,guid����,addr:%s:%d,recv_guid:%s,local_guid:%s,�������:%d",
			                                                  m_hostip.c_str(), m_hostport,pResponse->_guid,m_regGuid.c_str(),Response.index);
		return -1;
	}

	Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�������Ӧ,user:%s,addr:%s:%d,�������:[%d]",m_userid.c_str(),m_hostip.c_str(),m_hostport,Response.index);

	/*����������Ӧʱ��*/
	m_lastHeartTime = GetTickCount64();

	return 0;
}

int hostAgentSession::sendheart()
{
	meetingHost_cmd_HeartBeatReqMsg hbmsg;

	/*�������*/
	strncpy(hbmsg._guid,m_regGuid.c_str(),sizeof(hbmsg._guid));
	strncpy(hbmsg._userid,m_userid.c_str(),sizeof(hbmsg._userid)-1);
	hbmsg.index = ++heart_index;
	
	AsynModel::ConnID server_conn_id;
	memcpy(server_conn_id.openhost.openUDPIP, m_hostip.c_str(),sizeof(server_conn_id.openhost.openUDPIP));
	server_conn_id.openhost.openUdpPort = m_hostport;

	char buf[64];
	int blen = hbmsg.SerialBuff(buf,sizeof(buf));
	if(blen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��host��������,���л�ʧ��:(%s:%u)user_id:%s, guid:%s, �������:%d",
			server_conn_id.openhost.openUDPIP, m_hostport, hbmsg._userid, hbmsg._guid, hbmsg.index);
		return -1;
	}

#ifdef USE_RAW_SOCKET
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),server_conn_id,m_serverSessinid,MEETINGHOST_CMD_HEART,buf,blen);
#else
    AsynModel::AsynResultRet ret = PostMsg(server_conn_id, m_serverSessinid, MEETINGHOST_CMD_HEART, buf, blen, false, SENMSGDEFAULTTIMROUT, AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
	if (0 != ret)	
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��host��������,ͬ��ʧ��,host:(%s:%u),userid:%s,guid:%s,�������:%d",
			server_conn_id.openhost.openUDPIP, m_hostport, hbmsg._userid, hbmsg._guid, hbmsg.index);
		return -1;
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��host��������,ͬ���ɹ�,host:(%s:%u),userid:%s,guid:%s,�������:%d",
			server_conn_id.openhost.openUDPIP, m_hostport, hbmsg._userid, hbmsg._guid, hbmsg.index);
	}
	return 0;
}

/*��û���·�ɵ�ַ*/
bool  hostAgentSession::GetRouteServerAddrFromUrl(const std::string & routeUrl)
{
	int pos = routeUrl.find("|");
	if(pos < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetRouteServerAddrFromUrl]����ʧ��,�Ҳ����ؼ���|,routeUrl:%s,pos:%d",routeUrl.c_str(),pos);
		return false;
	}

	std::string  tempStr = routeUrl.substr(0,pos);
	bool bSucc = GetServerAddrFromUrl(tempStr,m_routeServerIp1,m_routeServerPort1);
	if(!bSucc)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]����ʧ�� m_routeServerIp1");
		return false;
	}

	tempStr = routeUrl.substr(pos+1,routeUrl.length()-pos-1);
	bSucc = GetServerAddrFromUrl(tempStr,m_routeServerIp2,m_routeServerPort2);
	if(!bSucc)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]����ʧ�� m_routeServerIp2");
		return false;
	}
	return true;
}

bool  hostAgentSession::GetServerAddrFromUrl(const std::string & routeUrl,
	std::string & meetingCtrlServerIp,unsigned short & meetingCtrlServerPort)
{
	int pos = routeUrl.find(":");
	if(pos < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]����ʧ��,routeUrl:%s �����Ҳ����ָ���:",routeUrl.c_str());
		return false;
	}

	meetingCtrlServerPort = atoi(routeUrl.substr(pos+1,routeUrl.length() - pos-1).c_str());
	meetingCtrlServerIp   = routeUrl.substr(0,pos);

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]���óɹ�,routeUrl:%s,������IP:%s,Port:%u",
										routeUrl.c_str(),meetingCtrlServerIp.c_str(),meetingCtrlServerPort);

	return true;
}

int hostAgentSession::onUICmd( const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender )
{
	/*����JSON�� ����cmdid��������*/
	Json ::Value root;
	if (true != Json_Util_Open(root,data))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����UI����,jason����ʧ��");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����UI����,����Ϊ:%s",data);

	int cmdid = root["cmdid"].asInt();
	switch(cmdid)
	{
	case 3000:/*����������Ϣ*/
		{
			onShareInfo(data,len,srcsid,sender);
		}
		break;
	case 3004:/*�����û�*/
		{
			onInvite(data,len,srcsid,sender);
		}
		break;
	default:
		{

		}
		break;
	}
	return 0;
}

int hostAgentSession::onShareInfo(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	Json::Value root;
	if(true != Json_Util_Open(root,data))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�����������Ϣ����,�޷�����");
		return -1;
	}

	m_shareip   = root["ssip"].asString();
	m_shareport = root["ssport"].asInt();
	m_name      = root["name"].asString();
	
	std::string respStr;
	Json::Value respRoot;
	respRoot["cmdid"] = 3001;
	if(m_shareip.empty() || 0 == m_shareport )
	{
		/*����ui -1����־*/
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�UI�ķ���������Ϣ����,������,shareip��shareport����");
		respRoot["status"] = -1;
	}
	else
	{
		/*���ظ�UI������������ɹ�*/
		respRoot["status"] = 0;
		m_sendShareInfoFlag = 1;
		m_bShareguid = GetGuidStr();
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ�UI�ķ���������Ϣ����,shareip:%s,shareport:%d",m_shareip.c_str(),m_shareport);
	}

	Json_Util_Write(respRoot,respStr);
	sendCmdToUI((char*)respStr.c_str(),respStr.length());

	//sendShareip();

	return 0;
}

int hostAgentSession::onShareipResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_cmd_shareIpResp shareResp;
	if(shareResp.unserialize((char*)data,len) < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ���������Ļ�Ӧ,�����л�ʧ��");
		return -1;
	}

	/*�ж��Ƿ�����������guid*/
	if(m_bShareguid != shareResp._guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ���������Ļ�Ӧ,guidУ�����.recv_guid:%s,local_guid:%s",shareResp._guid, m_bShareguid.c_str());
		return -1;
	}

	//�����ɹ�
	m_bShareip = true;    

	m_sendShareInfoFlag = 0;
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ���������Ļ�Ӧ,�����ɹ�,userid:%s.",m_userid.c_str());

	return 0;
}

int hostAgentSession::onSharePhoneInfoResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	unsigned long  tempip;
	unsigned short tempport;
	sender->GetRemoteIPPort(tempip, tempport);
	char remote_addr_str[16] = {0};
	ip_ulong_to_str(tempip, remote_addr_str);
	
	meetinghost_cmd_sharePhoneInfoResp sharePhoneInfoResp;
	if(sharePhoneInfoResp.Unserialize((char*)data,len) < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][DOWNLINE]���յ�����֪ͨ����,�����л�ʧ��");
		return -1;
	}

	/*�ж��Ƿ�����������guid*/
	if(m_regGuid != sharePhoneInfoResp.guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]���յ�����֪ͨ����,guidУ�����. recv_guid:%s, local_guid:%s",sharePhoneInfoResp.guid.c_str(), m_bShareguid.c_str());
		return -1;
	}

	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]���յ�����֪ͨ����,userid:%s status:%d remote:%s:%d",sharePhoneInfoResp.uerid.c_str(),sharePhoneInfoResp.status,remote_addr_str,tempport);

	if (0 != sharePhoneInfoResp.status)
	{
		m_canRegist = true;
		m_bHeart    = false;

		std::string respStr;
		Json::Value respRoot;
		respRoot["cmdid"]      = 4005;
		respRoot["accountid"]  = sharePhoneInfoResp.uerid;
		respRoot["token"]      = sharePhoneInfoResp.token;
		
		Json_Util_Write(respRoot,respStr);
		sendCmdToUI((char*)respStr.c_str(),respStr.length());
	}

	//�ظ�server�յ���������
	meetinghost_cmd_sharePhoneInfoResp downlineresp;
	downlineresp.uerid  = sharePhoneInfoResp.uerid;
	downlineresp.token  = sharePhoneInfoResp.token;
	downlineresp.guid   = sharePhoneInfoResp.guid;
	downlineresp.status = sharePhoneInfoResp.status;

	char buf[256] = {0};
	int serlen = downlineresp.Serialize(buf,sizeof(buf));
	if (serlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[Host][DOWNLINE]���յ�����֪ͨ����,��Host���ͻظ�,���л�ʧ��,userid:%s",sharePhoneInfoResp.uerid.c_str());
		return -1;
	}

	AsynModel::ConnID server_conn_id;
	memcpy(server_conn_id.openhost.openUDPIP, m_hostip.c_str(),sizeof(server_conn_id.openhost.openUDPIP));
	server_conn_id.openhost.openUdpPort = m_hostport;

#ifdef USE_RAW_SOCKET
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),server_conn_id,m_serverSessinid,MEETINGHOST_CMD_DOWNLINERESP,buf,serlen);
#else
	AsynModel::AsynResultRet ret = PostMsg(sender,srcsid, MEETINGHOST_CMD_DOWNLINERESP,buf,serlen,false);
#endif
	if (0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[host agent][DOWNLINE]���յ�����֪ͨ����,��Host���ͻظ�,ͬ��ʧ��,userid:%s,host:%s:%d",sharePhoneInfoResp.uerid.c_str(),m_hostip.c_str(),m_hostport);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]���յ�������������,��Host���ͻظ�,ͬ���ɹ�,userid:%s,host:%s:%d",sharePhoneInfoResp.uerid.c_str(),m_hostip.c_str(),m_hostport);
	}

	return 0;
}

int hostAgentSession::OnTimerShareip(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	//m_bShareTimeStart = false;
	//if(true != m_bShareip)
	//{
	//	/*��������*/
	//	sendShareip();
	//}
	return 0;
}

void hostAgentSession::sendShareInfoToServer()
{
	if(0 == m_hostport || m_hostip.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��Host���ͷ�����Ϣ,Host��ַ��Ч");
	}
	else
	{	
		AsynModel::ConnID conid = GetHostServerConnID();

		meetingHost_cmd_shareIpReq shareip;
		memset(&shareip,0,sizeof(shareip));
		strncpy(shareip.shareip,m_shareip.c_str(),sizeof(shareip.shareip)-1);
		shareip.shareport = m_shareport;
		strncpy(shareip.userid,m_userid.c_str(),sizeof(shareip.userid)-1);
		//m_bShareguid = GetGuidStr();
		strncpy(shareip._guid,m_bShareguid.c_str(), sizeof(shareip._guid)-1);
		strncpy(shareip._regGuid,m_regGuid.c_str(), sizeof(shareip._regGuid)-1);
		strncpy(shareip.name,m_name.c_str(),sizeof(shareip.name)-1);

		//���л�
		char buf[256] = {0};
		int sslen = shareip.serialize(buf,sizeof(buf));
		if(sslen < 0)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��Host���ͷ�����Ϣ,hostAdrr:(%s|%d),userid:%s,shareInfo:(%s|%d|%s),���л�ʧ��",
				m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
			return;
		}
#ifdef USE_RAW_SOCKET
		int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),conid,m_serverSessinid,MEETINGHOST_CMD_SHAREIP_REQ,buf,sslen);
#else
		AsynModel::AsynResultRet ret = PostMsg(conid,m_serverSessinid,MEETINGHOST_CMD_SHAREIP_REQ,buf,sslen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
		if (0 != ret)	
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��Host���ͷ�����Ϣ,ͬ������ʧ��,hostAdrr:(%s|%d),userid:%s,shareInfo:(%s:%d),name:%s",
				m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
		}
		else
		{
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��Host���ͷ�����Ϣ,ͬ�����ͳɹ�,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s,guid:%s",
				m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str(),m_bShareguid.c_str());
		}
	}

	return;
}


void hostAgentSession::sendShareip()
{
	if(0 == m_shareport)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����������Ϣ,������Ϣ����");
		return ;
	}

	AsynModel::ConnID conid = GetHostServerConnID();
	if(conid.openhost.openUdpPort == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����������Ϣ,host��ַ��Ч");
		return ;
	}

	m_bShareip = false;               //�Ƿ񷢲��ɹ�
	if(false == m_bShareTimeStart)
	{
		m_bShareTimeStart = true;
		SetTimer(__MEETINGHOSTAGENT_TIME_SHAREIP,1000);
	}

	meetingHost_cmd_shareIpReq shareip;
	memset(&shareip,0,sizeof(shareip));
	strncpy(shareip.shareip,m_shareip.c_str(),sizeof(shareip.shareip)-1);
	shareip.shareport = m_shareport;
	strncpy(shareip.userid,m_userid.c_str(),sizeof(shareip.userid)-1);
	m_bShareguid = GetGuidStr();
	memcpy(shareip._guid,m_bShareguid.c_str(), sizeof(shareip._guid));
	memcpy(shareip._regGuid,m_regGuid.c_str(), sizeof(shareip._regGuid));
	strncpy(shareip.name,m_name.c_str(),sizeof(shareip.name)-1);
	
	//���л�
	char buf[256] = {0};
	int sslen = shareip.serialize(buf,sizeof(buf));
	if(sslen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����������Ϣ,userid:%s,shareInfo:(%s|%d|%s),���л�ʧ��",
			m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
		return;
	}
#ifdef USE_RAW_SOCKET
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),conid,m_serverSessinid,MEETINGHOST_CMD_SHAREIP_REQ,buf,sslen);
#else
	AsynModel::AsynResultRet ret = PostMsg(conid,m_serverSessinid,MEETINGHOST_CMD_SHAREIP_REQ,buf,sslen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif	
	if (0 != ret)	
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]����������Ϣ,ͬ������ʧ��,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s",
			                    m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]����������Ϣ,ͬ�����ͳɹ�,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s",m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
	}

	return;
}

int hostAgentSession::getRecommendMsgAbstractWork(std::string contactlistid,char* outbuf,int &outlen)
{
	if (m_saveRecTaskManage.size() == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ѯ�Ƽ���Ϣ,�������κ��Ƽ���Ϣ");
		return -1;
	}
	
	if (contactlistid.empty())
	{
		//��ָ��ͨѶ¼ID,Ϊ��ѯ�����Ƽ���Ϣ
		std::string result;
		buildQueryRecommendMsg(m_saveRecTaskManage,result);
		memcpy(outbuf,result.c_str(),result.length());

		outlen = result.length();

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ѯ�Ƽ���Ϣ,��ѯ����ͨѶ¼,data:%s",result.c_str());

		return 0;
	}
	else
	{
		//��ѯָ���Ƽ�ͨѶ¼ID����Ϣ
		std::map<std::string,PerUserRecommendMsg>::iterator it2 = m_saveRecTaskManage.find(contactlistid);
		if (it2 == m_saveRecTaskManage.end())
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ȡ�Ƽ���Ϣ,�޸���Ϣ,contactlistid:%s",contactlistid.c_str());
			return -1;
		}

		std::string result;
		buildPerUserRecommendMsg(it2->second,result);
		outlen = result.length();

		Json::Value root2;
		root2["contactlistid"]      = Json::Value(it2->second.contactListId);
		root2["contactlistversion"] = Json::Value(it2->second.contactListVersion);
		root2["contactlistname"]    = Json::Value(it2->second.contactListName);
		root2["sourceid"]           = Json::Value(it2->second.sourceId);
		root2["contactersnum"]      = Json::Value((int)it2->second.contacters.size());
		
		std::string record2;
		Json_Util_Write(root2,record2);
		memcpy(outbuf,record2.c_str(),record2.length());

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ѯ�Ƽ���Ϣ,��ѯͨѶ¼Ϊ:%s,data:%s,��ͨѶ¼�Ƽ���Ϣ��С:%d",contactlistid.c_str(),record2.c_str(),outlen);
		
		return 0;
	}
}

int hostAgentSession::getRecommendMsgWork(std::string contactlistid,char* outbuf)
{
	if (contactlistid.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ȡ�Ƽ���Ϣ,ͨѶ¼IDΪ��");
		return -1;
	}

	std::map<std::string,PerUserRecommendMsg>::iterator it = m_saveRecTaskManage.find(contactlistid);
	if (it == m_saveRecTaskManage.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ȡ�Ƽ���Ϣ,�޸��Ƽ���Ϣ,contactlistid:%s",contactlistid.c_str());
		return -1;
	}

	std::string result;
	buildPerUserRecommendMsg(it->second,result);
	memcpy(outbuf,result.c_str(),result.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI��ȡ�Ƽ���Ϣ,contactlistid:%s,contactlistname:%s,conntactlistverison:%d,sourid:%s,contactescount:%d,len:%d",
		contactlistid.c_str(),it->second.contactListName.c_str(),it->second.contactListVersion,it->second.sourceId.c_str(),it->second.contacters.size(),result.length());

	//��¼���͸�ͨѶ¼ID�İ汾��
	m_saveRecVersion[contactlistid] = it->second.contactListVersion;

	//ɾ�����ش洢��Ϣ
	m_saveRecTaskManage.erase(it);
	deleteRecMsgFromDisk(g_recMsglocalpath,contactlistid);

	return 0;

}

int hostAgentSession::deleteRecommendMsgWork(std::string contactlistid)
{
	if (contactlistid.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]ɾ���Ƽ���Ϣ,ͨѶ¼IDΪ��");
		return -1;
	}

	std::map<std::string,PerUserRecommendMsg>::iterator it = m_saveRecTaskManage.find(contactlistid);
	if (it == m_saveRecTaskManage.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]ɾ���Ƽ���Ϣ,�޸���Ϣ��¼,contactlistid:%s",contactlistid.c_str());
		return -1;
	}

	//ɾ���ڴ�洢�Լ����ؼ�¼
	m_saveRecTaskManage.erase(it);
	deleteRecMsgFromDisk(g_recMsglocalpath,contactlistid);

    Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]ɾ���Ƽ���Ϣ,�ɹ�,contactlistid:%s",contactlistid.c_str());

	return 0;
}

int hostAgentSession::deleteRecommendMsgWorkRecord(std::string contactlistid)
{
	std::map<std::string,int>::iterator it = m_saveRecVersion.find(contactlistid);
	if (it != m_saveRecVersion.end())
	{
		m_saveRecVersion.erase(it);
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]ɾ���Ƽ���¼,ͨѶ¼IDΪ:%s",contactlistid.c_str());
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]ɾ���Ƽ���¼,ͨѶ¼IDΪ:%s,�����޸�ͨѶ¼�Ƽ���¼",contactlistid.c_str());
		return -1;
	}

	return 0;
}

int hostAgentSession::recommendResp(char* guid,char* userid,U32 srcsid,const AsynModel::SptrISender& sender)
{
	PerUserRecommendMsgResp recMsgResp;
	recMsgResp.guid   = guid;
	recMsgResp.userid = userid;
	recMsgResp.status = 0;

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ��Ƽ���Ϣ,�ظ�SERVER,user:%s guid:%s status:%d",recMsgResp.userid.c_str(),recMsgResp.guid.c_str(),recMsgResp.status);

	char buf[128] = {0};
	int sslen = recMsgResp.Serialize(buf,sizeof(buf));
	if(sslen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ��Ƽ���Ϣ,�ظ�SERVER,���л�ʧ��,userid:%s",recMsgResp.userid.c_str());
		return -1;
	}

	//TCP��·�ظ�
	AsynModel::AsynResultRet ret = PostMsg((AsynModel::SptrISender&)sender,srcsid,MEETINGHOST_CMD_AGENT_RECOMMEND_RESP,buf,sslen,false);
	if (0 != ret)	
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ��Ƽ���Ϣ,�ظ�SERVER,ͬ������ʧ��,userid:%s",recMsgResp.userid.c_str());
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]���յ��Ƽ���Ϣ,�ظ�SERVER,ͬ�����ͳɹ�,user:%s,guid:%s,status:%d",recMsgResp.userid.c_str(),recMsgResp.guid.c_str(),recMsgResp.status);
	}

	return 0;
}

int hostAgentSession::CompositingCmd(Json::Value & cmd, std::string &result, std::string name,int cmdid,int meetingid ,std::string methodMsg )
{
	cmd["name"]      = Json::Value(name);
	cmd["cmdid"]     = Json::Value(cmdid);
	cmd["meetingid"] = Json::Value(meetingid);
	if (true != Json_Util_Write(cmd,result))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s]�����ʽ��jsonʧ��",methodMsg.c_str());
		return -1;
	}
	return 0;
}

int hostAgentSession::CompositingCmd(Json::Value &cmd, std::string &result, std::string inviteid, int cmdid, std::string name, int meetingid, std::string methodMsg)
{
	cmd["inviteid"] = Json::Value(inviteid);
	cmd["cmdid"]     = Json::Value(cmdid);
	cmd["meetingid"] = Json::Value(meetingid);
	cmd["name"]      = Json::Value(name);
	if (true != Json_Util_Write(cmd,result))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] �����ʽ��jsonʧ��",methodMsg.c_str());
		return -1;
	}
	return 0;
}
int hostAgentSession::CompositingCmd( Json::Value & cmd, std::string &result,std::string accountid,int cmdid,std::string methodMsg )
{
	cmd["accountid"] = Json::Value(accountid);
	cmd["cmdid"]     = Json::Value(cmdid);
	if (true != Json_Util_Write(cmd,result))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] �����ʽ��jsonʧ��",methodMsg.c_str());
		return -1;
	}
	return 0;
}

int hostAgentSession::CompositingCmd( Json::Value & cmd, std::string &result,int cmdid,int status, std::string methodMsg )
{
	cmd["cmdid"]     = Json::Value(cmdid);
	cmd["status"]    = Json::Value(status);
	if (true != Json_Util_Write(cmd,result))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] �����ʽ��jsonʧ��",methodMsg.c_str());
		return -1;
	}
	return 0;
}

int hostAgentSession::CompositingCmd( Json::Value & cmd, std::string &result,std::string name,int cmdid,int meetingid,std::string accountid,std::string id,std::string methodMsg )
{
	cmd["accountid"] = Json::Value(accountid);
	cmd["cmdid"]     = Json::Value(cmdid);
	cmd["meetingid"] = Json::Value(meetingid);
	cmd["id"]        = Json::Value(id);
	cmd["name"]      = Json::Value(name);
	if (true != Json_Util_Write(cmd,result))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] �����ʽ��jsonʧ��",methodMsg.c_str());
		return -1;
	}
	return 0;
}

 AsynModel::ConnID hostAgentSession::GetHostServerConnID()
{
	AsynModel::ConnID connid;
	strncpy(connid.openhost.openUDPIP,m_hostip.c_str(),sizeof(connid.openhost.openUDPIP)-1);
	connid.openhost.openUdpPort = m_hostport;
	return connid;
}

int hostAgentSession::OnPostMsgRecept(AsynModel::ReceptType receptType,
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
#if 0
	 if(MEETINGHOST_CMD_INVITE == msgId)
	 {
		 Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��������Ļ�Ӧ: receptType:%d,receptResult:%d,msgId:%d",
			 receptType,receptResult,msgId);

		 int ret = 0;
		 if(receptResult != AsynModel::RECEPT_SUCCESS)
		 {
			 ret = -3;
		 }
		 /*����UI����ͳɹ�*/
		 std::string respStr;
		 Json::Value respRoot;
		 CompositingCmd(respRoot,respStr,3005,ret,"��������");
		 sendCmdToUI((char*)respStr.c_str(),respStr.length());
	}
#endif
	 return 0;
 }


bool hostAgentSession::ParseJson(char* buff,Json::Value & cmd)
{
	Json::Reader read;
	try
	{
		bool bSucc = read.parse(buff,cmd,false);
		if(!bSucc)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]Json����,ʧ��,buff:%s",buff);
			return false;
		}
	}
	catch (...)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]Json����,�쳣,buff:%s",buff);
		return false;
	}

	return true;
}

int hostAgentSession::buildQueryRecommendMsg(map<string,PerUserRecommendMsg> saveRecTask,std::string& result)
{
	Json::Value root;
	root["count"] = (int)saveRecTask.size();

	Json::Value recommend;
	std::map<std::string,PerUserRecommendMsg>::iterator it = saveRecTask.begin();
	for(;it != saveRecTask.end();it++)
	{
		Json::Value rec;

		rec["contactlistid"]      = Json::Value(it->second.contactListId);
		rec["contactlistversion"] = Json::Value(it->second.contactListVersion);
		rec["contactlistname"]    = Json::Value(it->second.contactListName);
		rec["sourceid"]           = Json::Value(it->second.sourceId);
		rec["contactersnum"]      = Json::Value((int)it->second.contacters.size());

		root["recmsg"].append(rec);
	}

	std::string record;
	Json_Util_Write(root,record);

	result = record;

	return 0;
}

int hostAgentSession::buildPerUserRecommendMsg(PerUserRecommendMsg records,std::string& result)
{
	Json::Value root;
	root["contactlistid"]      = Json::Value(records.contactListId);
	root["contactlistversion"] = Json::Value(records.contactListVersion);
	root["contactlistname"]    = Json::Value(records.contactListName);
	root["sourceid"]           = Json::Value(records.sourceId);
	root["userid"]             = Json::Value(records.userid);

	Json::Value recMsg;
	int num = records.contacters.size();
	recMsg["count"] = Json::Value(num);

	std::list<Contact>::iterator iter = records.contacters.begin();
	for (;iter != records.contacters.end();iter++)
	{
		Json::Value rec;
		rec["userid"]     = Json::Value(iter->userid);
		rec["name"]       = Json::Value(iter->name);
		rec["department"] = Json::Value(iter->department);
		rec["phonenum"]   = Json::Value(iter->phonenum);
		//rec["serilnum"]   = Json::Value(iter->serilnum);

		recMsg["contacters"].append(rec);
	}

	root["recommendmsg"] = recMsg;

	std::string record;
	Json_Util_Write(root,record);

	result = record;

	return 0;
}

int hostAgentSession::onBugEngien(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	notifyRegister notregistercmd;
	int unlen = notregistercmd.UnSeralize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ�bug����������������,�����л�ʧ��");
		return -1;
	}
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ���ȡ��־����,bugEngienIP:%s,bugEngienPort:%d",notregistercmd.BugEngineIP,notregistercmd.BugEnginePort);

	/************************************************************************/
	/*     ��������������bugEngien�ӿ�                                     */
	/************************************************************************/
//#ifdef WIN_TEST
//	char strFilePath[300] ={0};
//	DWORD dwError = GetModuleFileName(NULL,strFilePath,MAX_PATH );
//	if (dwError>0)
//	{
//		PathRemoveFileSpec(strFilePath);
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent] ��ȡ��ǰĿ¼�ɹ�:%s ���� ",strFilePath );
//
//	}
//	else
//	{
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent] ��ȡ��ǰĿ¼ʧ�ܣ�");
//		return -11;
//	}
//
//	strcat(strFilePath,"\\LOG");
//
//	if (0 == LPClientManager::GetInst()->InitHandleSession(notregistercmd.BugEngineIP,
//		notregistercmd.BugEnginePort,
//		notregistercmd.BugEngineSid,
//		notregistercmd.diagId,
//		notregistercmd.userId,
//		strFilePath,strFilePath))   // "D:/X1_Code/MeetingHost/MeetingHostAgent_20150413/dll/w32r/LOG/","D:/X1_Code/MeetingHost/MeetingHostAgent_20150413/dll/w32r/LOG/"
//#else
//	if (0 == LPClientManager::GetInst()->InitHandleSession(notregistercmd.BugEngineIP,
//		notregistercmd.BugEnginePort,
//		notregistercmd.BugEngineSid,
//		notregistercmd.diagId,
//		notregistercmd.userId,
//		"/sdcard/meeting/log/cn.redcdn.x1meeting/","/sdcard/meeting/log/logwriter/"))   // "D:/X1_Code/MeetingHost/MeetingHostAgent_20150413/dll/w32r/LOG/","D:/X1_Code/MeetingHost/MeetingHostAgent_20150413/dll/w32r/LOG/"
//#endif
//	{
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ���ȡ��־����,�����ɹ�");
//	}
//	else
//	{
//		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ���ȡ��־����,����ʧ��");
//	}

	return 0;
}

int hostAgentSession::onTimerInviteCheck(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	if (m_sendInviteRecord.size() != 0)
	{
		std::map<std::string,InviteCmdContext>::iterator iter0 = m_sendInviteRecord.begin();
		while(iter0 != m_sendInviteRecord.end())
		{
			std::string  guid = iter0->first;
			if (iter0->second.sendInviteMsgCount > INVITE_SEND_GUID_TIMES)
			{
				/************************************************************************/
				/*                  δ�յ�����ظ��ļ�¼                               */
				/************************************************************************/

				char recordName[128] = {0};
				sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

				FILE* pInviteEventRecord = fopen(recordName,"a+");
				if (pInviteEventRecord == NULL)
				{
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]�ļ�:%s �޷���",recordName);
					return 0;
				}

				Json ::Value root;
				if (true != Json_Util_Open(root,iter0->second.InviteData))
				{
					return -1;
				}

				std::string invitedID = root["accounted"].asString();

				Date tmp_now;
				char inviteMsg[512] = {0};
				sprintf(inviteMsg,"Time:%s|InviteId:%s|InvitedId:%s|Guid:%s|MeetingId:%d|NoResp\r\n",tmp_now.toString("yyyymmddhhmi").c_str(),m_userid.c_str(),invitedID.c_str(),guid.c_str(),iter0->second.meetingid);
				if (inviteMsg != NULL)
				{
					fwrite(inviteMsg,sizeof(char),strlen(inviteMsg),pInviteEventRecord);
					fflush(pInviteEventRecord);
					fclose(pInviteEventRecord);
				}
				/************************************************************************/
				/*                                                                      */
				/************************************************************************/

				Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ����δ�յ��ظ�����������,�ط�����10��,GUID:%s,meetingid:%d",guid.c_str(),iter0->second.meetingid);
				m_sendInviteRecord.erase(iter0++);
			}
			else
			{
				//�ط���������
				iter0->second.sendInviteMsgCount++;

				AsynModel::ConnID connid = GetHostServerConnID();
				if(connid.openhost.openUdpPort == 0)
				{
					/*���������*/
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��ʱ����δ�յ��ظ�����������,δ����10��,userid=%s,��host��������ʱ,��ַ������",m_userid.c_str());
				}

				AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_INVITE_SINGLE,iter0->second.InviteData,sizeof(iter0->second.InviteData),true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
				if (0 != ret)
				{
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]��ʱ����δ�յ��ظ�����������,δ����10��,��Host������������,ͬ��ʧ��,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(), m_hostport);
				}
				else
				{
					Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ����δ�յ��ظ�����������,δ����10��,��Host������������,ͬ���ɹ�,(addr:%s:%d),userid:%s,meetingid:%d,guid:%s",m_hostip.c_str(),m_hostport,m_userid.c_str(),iter0->second.meetingid,guid.c_str());
				}				
				iter0++;
			}
		}
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ����δ�յ��ظ�����������,�����Ƽ���Ϣ��¼Ϊ��");
	}
	
	/************************************************************************/
	/*                       ��ʱ��������¼����ʱɾ��                    */
	/************************************************************************/
	if (m_recvInviteRecord.size() != 0)
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ��鱻������ʷ��¼,���ر����¼��Ϊ:%d",m_recvInviteRecord.size());

		I64 timeNow = GetTickCount64();
		std::map<std::string,I64>::iterator iter1 = m_recvInviteRecord.begin();
		while(iter1 != m_recvInviteRecord.end())
		{
			if (timeNow - iter1->second > INVITE_RECEIVE_GUID_TIME_OUT)
			{
				std::string guid2 = iter1->first;
				Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ��鱻������ʷ��¼,guid:%s ����30��ɾ��",guid2.c_str());
				m_recvInviteRecord.erase(iter1++);
			}
			else
			{
				iter1++;
			}
		}
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]��ʱ��鱻������ʷ��¼,���ر����¼��Ϊ��");
	}

	SetTimer(__MEETINGHOSTAGENT_TIME_INVITEANDCHECK,INVITE_CHECK_TIME_STEP);

	return 0;
}

int hostAgentSession::onTimerSetLocal(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	//��ʱ���ڴ��е��Ƽ���Ϣˢ�µ�����
	if (m_saveRecTaskManage.size() != 0)
	{
		saveRecMsgToDisk(g_recMsglocalpath,m_saveRecTaskManage);
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]��ʱ���ڴ��Ƽ���Ϣ,ˢ�µ�����,�ڴ�������Ϊ:%d",m_saveRecTaskManage.size());
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]��ʱ���ڴ��Ƽ���Ϣ,ˢ�µ�����,����������Ϊ��");
	}

	//��ʱ���ڴ��е��Ƽ���¼ˢ�µ�����
	if (m_saveRecVersion.size() != 0)
	{
		saveRecRecordMsgToDisk(g_recRecordlocalpath,m_saveRecVersion);
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]��ʱ���Ѿ��Ƽ��ļ�¼,ˢ�µ�����,�ڴ�������Ϊ:%d",m_saveRecVersion.size());
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]��ʱ���Ѿ��Ƽ��ļ�¼,ˢ�µ�����,����������Ϊ��");
	}
	
	SetTimer(__MEETINGHOSTAGENT_TIME_SETLOCAL,60000);

	return 0;
}

int hostAgentSession::saveRecRecordMsgToDisk(std::string path,std::map<std::string,int> recRecordMsg)
{
	TiXmlDocument myDocument;
	myDocument.Parse("<?xml version='1.0' encoding='gb2312'?><RecommendRecords></RecommendRecords>");
	if (myDocument.Error())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecRecordMsgToDisk]XML�ļ���������!");
		return -101;
	}

	TiXmlElement* RootElement = NULL;
	RootElement = myDocument.FirstChildElement("RecommendRecords");
	if (!RootElement)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[Host][saveRecRecordMsgToDisk]XML��RecommendRecords�ڵ�!");
		return -102;
	}

	std::map<std::string,int>::iterator iter = recRecordMsg.begin();
	for (;iter != recRecordMsg.end();iter++)
	{

			TiXmlElement ContactListIdElement("contactlistid");
			TiXmlText ContactListIdContent(iter->first.c_str());
			ContactListIdElement.InsertEndChild(ContactListIdContent);

			char buf[4] = {0};
			sprintf(buf,"%d",iter->second);
			std::string contactListVersion = buf;
			TiXmlElement ContactListVersionElement("contactlistversion");
			TiXmlText ContactListVersionContent(contactListVersion.c_str());
			ContactListVersionElement.InsertEndChild(ContactListVersionContent);


			TiXmlElement contactinfo("RecommendRecordItem");
			contactinfo.InsertEndChild(ContactListIdElement);
			contactinfo.InsertEndChild(ContactListVersionElement);

			RootElement->InsertEndChild(contactinfo);
	}


	myDocument.SaveFile(path.c_str());

	return 0;
}

int hostAgentSession::readRecRecordMsgFromDisk(std::string path,std::map<std::string,int> &recRecordMsg)
{
	TiXmlDocument myDocument;
	TiXmlElement* recommend = NULL ;

	if (!myDocument.LoadFile(path.c_str()))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readRecRecordMsgFromDisk]���ر��ش洢��Ϣ������,path:%s",path.c_str());
		return -1;
	}

	recommend = myDocument.FirstChildElement("RecommendRecords");
	if (!recommend)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readRecRecordMsgFromDisk]��RecommendRecords�ڵ�,path:%s",path.c_str());
		return -1;
	}

	TiXmlElement* oneUser = recommend->FirstChildElement("RecommendRecordItem");

	while(oneUser)
	{
		TiXmlElement* contactlistid = oneUser->FirstChildElement("contactlistid");
		if(!contactlistid)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}

		TiXmlElement* contactlistversion = oneUser->FirstChildElement("contactlistversion");
		if(!contactlistversion)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}

		recRecordMsg[contactlistid->GetText()] = atoi(contactlistversion->GetText());

		oneUser = oneUser->NextSiblingElement();
	}

	return 0;
}

int hostAgentSession::saveRecMsgToDisk(std::string path,std::map<std::string,PerUserRecommendMsg> recmsg)
{
	TiXmlDocument myDocument;
	myDocument.Parse("<?xml version='1.0' encoding='gb2312'?><RecommendInfo></RecommendInfo>");
	if (myDocument.Error())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecMsgToDisk]XML�ļ���������!");
		return -101;
	}

	TiXmlElement* RootElement = NULL;
	RootElement = myDocument.FirstChildElement("RecommendInfo");
	if (!RootElement)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecMsgToDisk]XML��RecommendInfo�ڵ�!");
		return -102;
	}

	std::map<std::string,PerUserRecommendMsg>::iterator iter = recmsg.begin();
	for (;iter != recmsg.end();iter++)
	{
		std::list<Contact>::iterator it = (iter->second.contacters).begin();
		while(it != iter->second.contacters.end())
		{
			Contact msg;
			msg.userid = it->userid;
			msg.name   = it->name;
			msg.department = it->department;
			msg.phonenum   = it->phonenum;
			msg.serilnum   = it->serilnum;

			std::string key = iter->first;

			TiXmlElement ContactListIdElement("contactlistid");
			TiXmlText ContactListIdContent(iter->second.contactListId.c_str());
			ContactListIdElement.InsertEndChild(ContactListIdContent);

			TiXmlElement ContactListNameElement("contactlistname");
			TiXmlText ContactListNameContent(iter->second.contactListName.c_str());
			ContactListNameElement.InsertEndChild(ContactListNameContent);

			char buf[4] = {0};
			sprintf(buf,"%d",iter->second.contactListVersion);
			std::string contactListVersion = buf;
			TiXmlElement ContactListVersionElement("contactlistversion");
			TiXmlText ContactListVersionContent(contactListVersion.c_str());
			ContactListVersionElement.InsertEndChild(ContactListVersionContent);

			TiXmlElement GuidElement("guid");
			TiXmlText GuidContent(iter->second.guid.c_str());
			GuidElement.InsertEndChild(GuidContent);

			TiXmlElement DestElement("destid");
			TiXmlText DestContent(iter->second.userid.c_str());
			DestElement.InsertEndChild(DestContent);

			TiXmlElement SourceElement("sourceid");
			TiXmlText SourceContent(iter->second.sourceId.c_str());
			SourceElement.InsertEndChild(SourceContent);

			TiXmlElement UseridElement("userid");
			TiXmlText UseridContent(msg.userid.c_str());
			UseridElement.InsertEndChild(UseridContent);

			TiXmlElement NameElement("name");
			TiXmlText NameContent(msg.name.c_str());
			NameElement.InsertEndChild(NameContent);

			TiXmlElement DepartmentElement("department");
			TiXmlText DepartmentContent(msg.department.c_str());
			DepartmentElement.InsertEndChild(DepartmentContent);

			TiXmlElement PhonenumElement("phonenum");
			TiXmlText PhonenumContent(msg.phonenum.c_str());
			PhonenumElement.InsertEndChild(PhonenumContent);

			TiXmlElement SerilnumElement("serilnum");
			TiXmlText SerilContent(msg.serilnum.c_str());
			SerilnumElement.InsertEndChild(SerilContent);

			TiXmlElement contactinfo("ContactItem");
			contactinfo.InsertEndChild(ContactListIdElement);
			contactinfo.InsertEndChild(ContactListNameElement);
			contactinfo.InsertEndChild(ContactListVersionElement);
			contactinfo.InsertEndChild(GuidElement);
			contactinfo.InsertEndChild(DestElement);
			contactinfo.InsertEndChild(SourceElement);
			contactinfo.InsertEndChild(UseridElement);
			contactinfo.InsertEndChild(NameElement);
			contactinfo.InsertEndChild(DepartmentElement);
			contactinfo.InsertEndChild(PhonenumElement);
			contactinfo.InsertEndChild(SerilnumElement);

			RootElement->InsertEndChild(contactinfo);

			it++;
		}
	}


	myDocument.SaveFile(path.c_str());

	return 0;
}

int hostAgentSession::readAllRecMsgFromDisk(std::string path,std::multimap<std::string,Contact> &recMsg)
{
	TiXmlDocument myDocument;
	TiXmlElement* recommend = NULL ;

	if (path.length() == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]���ر���·��Ϊ��");
		return -1;
	}

	if (!myDocument.LoadFile(path.c_str()))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]���ر��ش洢��Ϣ������,path:%s",path.c_str());
		return -1;
	}

	recommend = myDocument.FirstChildElement("RecommendInfo");
	if (!recommend)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]��RecommendInfo�ڵ�,path:%s",path.c_str());
		return -1;
	}
#if 1

	TiXmlElement* oneUser = recommend->FirstChildElement("ContactItem");

	std::string tmpkey = "";
	while(oneUser)
	{
		Contact userinfo;
		TiXmlElement* destid = oneUser->FirstChildElement("destid");
		if(!destid )
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (destid->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		tmpkey = destid->GetText();
		tmpkey += "|";

		TiXmlElement* guid = oneUser->FirstChildElement("guid");
		if(!guid)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (guid->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}

		tmpkey += guid->GetText();
		tmpkey += "|";

		TiXmlElement* sourceid = oneUser->FirstChildElement("sourceid");
		if(!sourceid)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (sourceid->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		tmpkey += sourceid->GetText();
		tmpkey += "|";

		TiXmlElement* contactlistid = oneUser->FirstChildElement("contactlistid");
		if(!contactlistid)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (contactlistid->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}

		tmpkey += contactlistid->GetText();
		tmpkey +="|";

		TiXmlElement* contactlistname = oneUser->FirstChildElement("contactlistname");
		if(!contactlistname)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (contactlistname->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		tmpkey += contactlistname->GetText();
		tmpkey += "|";

		TiXmlElement* contactlistversion = oneUser->FirstChildElement("contactlistversion");
		if(!contactlistversion)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (contactlistversion->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		tmpkey += contactlistversion->GetText();

		//KEY����Ϊ��,��Ҫ�ж�

		TiXmlElement* userid = oneUser->FirstChildElement("userid");
		if(!userid)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if(userid->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		userinfo.userid =userid->GetText();

		TiXmlElement* name = oneUser->FirstChildElement("name");
		if(!name)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}

		if (name->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		userinfo.name = name->GetText();
		
		TiXmlElement* department = oneUser->FirstChildElement("department");
		if(!department)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (department->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		userinfo.department = department->GetText();

		TiXmlElement* phonenum = oneUser->FirstChildElement("phonenum");
		if(!phonenum)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		
		if (phonenum->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		userinfo.phonenum = phonenum->GetText();
		
		TiXmlElement* serilnum = oneUser->FirstChildElement("serilnum");
		if(!serilnum)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
	
		if (serilnum->GetText() == NULL)
		{
			oneUser = oneUser->NextSiblingElement();
			continue;
		}
		userinfo.serilnum = serilnum->GetText();

		recMsg.insert(make_pair(tmpkey,userinfo));	//����multimap

		oneUser = oneUser->NextSiblingElement();
	}

#endif
	return 0;
}

int hostAgentSession::tansToPerUerRecMsgMap(std::multimap<std::string,Contact> msg,std::map<string,PerUserRecommendMsg> &recMsg)
{
	//һ��taskID�ɶ�Ӧ����Ƽ���Ϣ��Contct��,����ת��Ϊmap<destID,PerUserMSG>����ʽ
	std::string curId = "";
	std::string key   = "";

	std::multimap<std::string,Contact>::iterator iter = msg.begin();
	for (iter; iter != msg.end(); iter = msg.upper_bound(iter->first))  
	{
		PerUserRecommendMsg tmp;
		std::pair<std::multimap<std::string,Contact>::iterator, std::multimap<std::string,Contact>::iterator> res = msg.equal_range(iter->first); 
		for (std::multimap<std::string,Contact>::iterator i = res.first; i != res.second; ++i)  
		{  
			tmp.contacters.push_back(i->second);
			//destid|guid|sourid|contactlistid|contactlistname|contactlistversion
			curId = i->first;

			int pos0 = curId.find_first_of('|');
			tmp.userid = curId.substr(0,pos0);

			curId = curId.substr(pos0+1,curId.length());  //guid|sourid|contactlistid|contactlistname|contactlistversion
			int pos1 = curId.find_first_of('|');
			tmp.guid = curId.substr(0,pos1);

			curId = curId.substr(pos1+1,curId.length());  //sourid|contactlistid|contactlistname|contactlistversion
			int pos2     = curId.find_first_of('|');
			tmp.sourceId = curId.substr(0,pos2);

			curId = curId.substr(pos2+1,curId.length());  //contactlistid|contactlistname|contactlistversion
			int pos3 = curId.find_first_of('|');
			tmp.contactListId   = curId.substr(0,pos3);

			curId = curId.substr(pos3+1,curId.length());
			int pos4 = curId.find_first_of('|');
			tmp.contactListName = curId.substr(0,pos4);

			std::string contactlistversion = curId.substr(pos4+1,curId.length());
			tmp.contactListVersion = atoi(contactlistversion.c_str());

			recMsg[tmp.contactListId] = tmp; 
		}  	
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,5000,"[host agent][tansToPerUerRecMsgMap]���ر�����Ƽ���Ϣ,������Ϊ:%d",recMsg.size());

	return 0;
}

int hostAgentSession::deleteRecMsgFromDisk(std::string path,std::string recKey)
{
	TiXmlDocument myDocument;
	TiXmlElement* root = NULL ;

	if (!myDocument.LoadFile(path.c_str()))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][deleteRecMsgFromDisk]�޷����ر��ش洢�Ƽ���Ϣ�ļ�,path:%s",path.c_str());
		return -1;
	}

	root = myDocument.FirstChildElement("RecommendInfo");
	if (!root)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][deleteRecMsgFromDisk]��RecommendInfo�ڵ�,path:%s",path.c_str());
		return -1;
	}

	TiXmlElement* contactItem = root->FirstChildElement("ContactItem");
	while(contactItem)
	{
		TiXmlElement* childElement = contactItem->FirstChildElement("contactlistid");
		if (!childElement)
		{
			contactItem = contactItem->NextSiblingElement();
			continue;
		}

		if (recKey == childElement->GetText())
		{
			TiXmlElement* contactItemTmp;
			contactItemTmp = contactItem;
			contactItem = contactItem->NextSiblingElement();
			root->RemoveChild(contactItemTmp);
			continue;
		}

		contactItem = contactItem->NextSiblingElement();
	}

	myDocument.SaveFile(path.c_str());

	return 0;
}

void hostAgentSession::HandleStpIPMap( std::string & stpIp,unsigned short & stpPort )
{
	serverInfo info;
	serverInfo info_out;
	info.ip   = stpIp;
	info.port = stpPort;

	bool bGetMap = g_ConfigParser.GetMappedStpIp(info,info_out);
	if(bGetMap)
	{
		stpIp   = info_out.ip;
		stpPort = info_out.port;
		return;
	}
}

int hostAgentSession::onTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_trans_data_ex datactx;
	int unSersize = datactx.UnSeralize(data,len);

	AsynModel::ConnID connid = GetHostServerConnID();
#ifdef USE_RAW_SOCKET 
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_TRANSDATA_REQ,(char*)data,len);
#else
	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_TRANSDATA_REQ,(char*)data,len,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
#endif
	if (0 != ret)
	{
		g_pmanage->OnTransDataResult(datactx.trans_data_uid,-1);

		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�UIת����������,��host��������ͬ��ʧ��,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]���յ�UIת����������,��Host��������ɹ�,addr:%s:%d",
			          m_hostip.c_str(),m_hostport);
	
	m_transDataLock.lock();
	datactx.m_lastSendTime = GMGetTickCount();
	datactx.m_sendTimes = 1;
	m_transData[datactx.trans_data_uid] = datactx;
	m_transDataLock.unlock();
			
	return 0;
}

int	hostAgentSession::onRecvTransData(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_trans_data datactx;
	int unSersize = datactx.UnSeralize(data,len);

	if (m_userid.compare(datactx.to_userid) != 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�HOSTת����������,ת��Ŀ����Ѷ���뱾����Ѷ�Ų�һ�£�������from:%s,to:%s,local:%s",
			datactx.from_userid, datactx.to_userid, m_userid.c_str());
		return -1;
	}

	//�ص�
	g_pmanage->OnRecvTransData(datactx.from_userid,datactx.buf,datactx.dataLen);

	AsynModel::ConnID connid = GetHostServerConnID();
	int respLen = len - datactx.dataLen;
	datactx.dataLen = 0;
	char respData[512] = { 0 };
	respLen = datactx.Seralize(respData,sizeof(respData));
#ifdef USE_RAW_SOCKET 
	int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_TRANSDATA_RESP,(char*)respData,respLen);
#else
	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_TRANSDATA_RESP,(char*)respData,respLen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
#endif
	if (0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�HOSTת����������,��host����Ӧ������ͬ��ʧ��,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
	}
	return 0;
}

int hostAgentSession::onRecvTransDataResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_trans_data datactx;
	int unSersize = datactx.UnSeralize(data,len);

	if (m_userid.compare(datactx.from_userid) != 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�HOSTת�����������Ӧ,��������Ѷ���뱾����Ѷ�Ų�һ�£�������from:%s,to:%s,local:%s",
			datactx.from_userid, datactx.to_userid, m_userid.c_str());
		//return -1;
	}

	m_transDataLock.lock();
	if (m_transData.find(datactx.trans_data_uid) != m_transData.end())
	{
		m_transData.erase(datactx.trans_data_uid);
	}
	m_transDataLock.unlock();

	g_pmanage->OnTransDataResult(datactx.trans_data_uid,0);

	return 0;
}

int hostAgentSession::onTimerSendTransData(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	//return 0;

	U64 tnow = GMGetTickCount();
	std::set<I64> timeoutReqs;
	//std::set<I64> resendReqs;
	m_transDataLock.lock();
	map<I64,meetingHost_trans_data_ex>::iterator iter = m_transData.begin();
	while (iter != m_transData.end())
	{
		if (iter->second.m_sendTimes > 3)
		{
			timeoutReqs.insert(iter->first);
			iter = m_transData.erase(iter);
			continue;
		}
		if (tnow - iter->second.m_lastSendTime > 200)
		{
			//resendReqs.insert(iter->first);
			char lszBuf[2048] = { 0 };
			int senLen = iter->second.Seralize(lszBuf,sizeof(lszBuf));
			iter->second.m_sendTimes++;
			AsynModel::ConnID connid = GetHostServerConnID();
#ifdef USE_RAW_SOCKET 
			int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_TRANSDATA_REQ,(char*)lszBuf,senLen);
#else
			AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_TRANSDATA_REQ,(char*)lszBuf,senLen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
#endif
			if (0 != ret)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]���յ�UIת����������,��host��������ͬ��ʧ��[uid:%lld,��%d��],userid=%s,addr:%s:%d",
					iter->first,iter->second.m_sendTimes,m_userid.c_str(),m_hostip.c_str(),m_hostport);
			}
		}
		iter++;
	}
	m_transDataLock.unlock();

	std::set<I64>::iterator iterReq = timeoutReqs.begin();
	while (iterReq != timeoutReqs.end())
	{
		g_pmanage->OnTransDataResult(*iterReq,1);
		iterReq++;
	}

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDTRANSDATA,100);
	return 0;
}
