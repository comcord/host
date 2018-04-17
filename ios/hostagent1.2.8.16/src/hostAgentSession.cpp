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
	//Session启动,加载本地推荐信息到内存
	std::multimap<std::string,Contact> msg;		   //key为userid+guid
	std::map<string,PerUserRecommendMsg> recMsg;   //key为userid+guid

	readAllRecMsgFromDisk(g_recMsglocalpath,msg);

#if 1
	if (msg.size() != 0)
	{
		//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]读取本地存在的内容,存储推荐信息MULTIMAP的大小为:%d",msg.size());
		
		//multimap转化为map
		tansToPerUerRecMsgMap(msg,recMsg);

		map<string,PerUserRecommendMsg>::iterator iter;
		for (iter = recMsg.begin(); iter != recMsg.end();++iter)
		{
			m_saveRecTaskManage[iter->first] = iter->second;

			//通知UI
			sendRecommendInfoToUI(iter->second);
		}
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]本地无推荐信息");
	}

	//读取本地的推荐历史记录
	readRecRecordMsgFromDisk(g_recRecordlocalpath,m_saveRecVersion);
#endif

	SetTimer(__MEETINGHOSTAGENT_TIME_SETLOCAL,2000);
	SetTimer(__MEETINGHOSTAGENT_TIME_INVITEANDCHECK,INVITE_CHECK_TIME_STEP);

	//启动发布分享定时器
	SetTimer(__MEETINGHOSTAGENT_TIME_SENDSHAREINFOTOHOST,10);

	SetTimer(__MEETINGHOSTAGENT_TIME_SENDTRANSDATA,100);

	return 0;
}


int hostAgentSession::sendRecommendInfoToUI(PerUserRecommendMsg msg)
{
	std::string respStr;
	Json::Value respRoot;
	respRoot["cmdid"]              = 4001;
	respRoot["sourceid"]           = msg.sourceId;                 //推荐人
	respRoot["contactlistid"]      = msg.contactListId;            //通讯录id
	respRoot["contactlistversion"] = msg.contactListVersion;       //通讯录版本号
	respRoot["contactlistname"]    = msg.contactListName;	       //通讯录名称
#ifdef __IOS__
	respRoot["contactersnum"]      = (int)msg.contacters.size();    //联系人个数
#else
  respRoot["contactersnum"]        = (int)msg.contacters.size();         //联系人个数
#endif

	Json_Util_Write(respRoot,respStr);

	if (respStr.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]发送推荐信息给UI,内容为空");
		return -1;
	}

	sendCmdToUI((char*)respStr.c_str(),respStr.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][RECOMMENDTION]发送推荐信息给UI,内容为:%s",respStr.c_str());

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
	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]hostAgentSession析构");
}

bool hostAgentSession::sendCmdToUI(char *buf,int len)
{
    return m_recv->UdpSend(buf,len);
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向UI发送命令,内容为:%s",buf);
}

int hostAgentSession::onRegister(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_NO != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]工作session当前工作状态非（__MEETINGHOSTAGENT_SESSIONSTATE_NO)");
		return -1;
	}
	/*创建路由连接session*/
	if(true != CreateGetRoutServerSession(m_rcAddr))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][CreateGetRoutServerSession]调用失败,rcaddr:%s",m_rcAddr.c_str());
		return -1;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][CreateGetRoutServerSession]调用成功");
	}

	/*向第一个路由请求host*/
	StartRequestHost();
	m_recvReqHostRespCount = 0;
	return 0;
}

/*通过指定路由获取host*/
void  hostAgentSession::StartRequestHost()
{
	if(m_canRegist)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]收到下线命令,停止获取host");
		return;
	}
	
	m_sessionState   = __MEETINGHOSTAGENT_SESSIONSTATE_HOST;
	unsigned int sid = GetSessionID();
	m_reqHostCount	 = 0;

	int ret = PostMsg(m_rcClientStpr1->GetSessionID(),HOSTAGENT_GETHOSTADDR_REQ,(char*)&sid,sizeof(sid),false);
	if (ret !=0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent]向client_session1发送获取请求Host命令,同步失败,addr:(%s|%d),ret:%d",m_routeServerIp1.c_str(),m_routeServerPort1,ret);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向client_session1发送获取请求Host命令,同步成功,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent]向client_session2发送获取请求Host命令,同步失败,addr:(%s|%d),ret:%d",m_routeServerIp2.c_str(),m_routeServerPort2,ret);
		}
		else
		{
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向client_session2发送获取请求Host命令,同步成功,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][GetRouteServerAddrFromUrl]调用失败,routeUrl:%s",routeUrl.c_str());
		return false;
	}

	//第一个路由server
#ifdef USE_RAW_SOCKET
	m_rcClientStpr1 = new(nothrow) client_session(m_routeServerIp1.c_str(),m_routeServerPort1,"routeServer1",m_manageSid);
#else
	m_rcClientStpr1 = new(nothrow) client_session(m_routeServerIp1.c_str(),m_routeServerPort1,"routeServer1");
#endif
	if (NULL == m_rcClientStpr1)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1创建失败,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
	}
	
	int ret = m_rcClientStpr1->Start();
	if(ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1启动失败,addr:(%s|%d),ret:%d",m_routeServerIp1.c_str(),m_routeServerPort1,ret);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session1启动成功,addr:(%s|%d)",m_routeServerIp1.c_str(),m_routeServerPort1);
	}
	
    //第二个路由server
#ifdef USE_RAW_SOCKET
	m_rcClientStpr2 = new(nothrow) client_session(m_routeServerIp2.c_str(),m_routeServerPort2,"routeServer2",m_manageSid);
#else
	m_rcClientStpr2 = new(nothrow) client_session(m_routeServerIp2.c_str(),m_routeServerPort2,"routeServer2");
#endif
	if (NULL == m_rcClientStpr2)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2创建失败,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
		return false;
	}
	
	ret = m_rcClientStpr2->Start();
	if(ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2启动失败,addr:(%s|%d)ret:%d",m_routeServerIp2.c_str(),m_routeServerPort2,ret);
		return false;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]client_session2,启动成功,addr:(%s|%d)",m_routeServerIp2.c_str(),m_routeServerPort2);
	}

	return true;
}

int hostAgentSession::onReqHostResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_HOST != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]当前SESSION非__MEETINGHOSTAGENT_SESSIONSTATE_HOST状态!");
		return -1;
	}

	GetHostServerRespCmd cmd;
	int ret = cmd.UnSeralize((char*)data,len);
	if (ret < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]获取host地址,反序列化失败 len:%u",len);
		return -1;
	}

	if(0 == cmd.result)
	{
		//ip 映射 port
		string tmpIP = cmd.userIp;
		HandleStpIPMap(tmpIP, cmd.userPort);

		//增加注册定时器控制标志
		if(0 == m_registerFlag)
		{
			/*获得host成功*/
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]获取host地址,成功,userid=%s,IP:%s,Port:%u",m_userid.c_str(),tmpIP.c_str(),cmd.userPort);

			m_hostip       = tmpIP;
			m_hostport     = cmd.userPort;
			m_sessionState = __MEETINGHOSTAGENT_SESSIONSTATE_REG;
			m_regCount     = 0;

			m_regGuid = GetGuidStr();	//一次注册保持一个GUID

			/*向host发送注册命令*/
			sendRegisterToServer();
			
			m_registerFlag = 1;
			/*启动定时检查注册结果*/
			SetTimer(__MEETINGHOSTAGENT_TIME_REG,1000);
			return 0;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]获取host地址,成功但已经启动过注册,userid=%s,IP:%s,Port:%u",m_userid.c_str(),tmpIP.c_str(),cmd.userPort);
		}
	}
	else
	{
		//暂定30次没有收到回应就换端口
		if (m_recvReqHostRespCount > 20)
		{
#ifdef USE_RAW_SOCKET
			PostMsg(m_manageSid,MEETINGMANAGE_SEESION_RECREATE,NULL,0);
#else
			//Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向STPRC请求Host达到20次未成功,进行重设本地端口,localIP:%s,udpPort:%d,tcpPort:%d",
			//	g_pmanage->m_localip.c_str(),g_pmanage->m_localport1,g_pmanage->m_localport2);

			////重新绑定端口
			//int tmp_udp_port = 0;
			//int tmp_tcp_port = 0;
			//g_pmanage->resetUdpLocalipandPort(g_pmanage->m_localip,g_pmanage->m_localport1,tmp_udp_port);
			//g_pmanage->resetTcpLocalipandPort(g_pmanage->m_localip,g_pmanage->m_localport2,tmp_tcp_port);

			//g_pmanage->m_localport1 = tmp_udp_port;
			//g_pmanage->m_localport2 = tmp_tcp_port;
#endif
			//重新请求Host
			m_recvReqHostRespCount = 0;
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]获取host地址,次数大于20未成功,重建SOCKET");

		}
		else
		{
			m_recvReqHostRespCount++;
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]获取host地址,次数为:%d",m_recvReqHostRespCount);
		}

		/*尝试下一个路由地址*/
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]向host发送注册命令,host地址无效");
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向host发送注册命令,序列化失败");
			return -1;
		}

#ifdef USE_RAW_SOCKET
		int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_REGISTER,buf,len);
#else
		AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_REGISTER,buf,len,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
		if (0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]向host发送注册命令,同步失败,userid=%s,(%s:%d),次数:%d",regCmd._userid,m_hostip.c_str(),m_hostport,m_regCount);
			return -1;
		}

		m_regCount++;     //注册尝试次数
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]向host发送注册命令,同步成功,token非空,userid=%s,(%s:%d),次数:%d",regCmd._userid,m_hostip.c_str(),m_hostport,m_regCount);
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向host发送注册命令,序列化失败");
			return -1;
		}

#ifdef USE_RAW_SOCKET
		int ret = g_pmanage->sendDataByUdpManage(GetSessionID(),connid,0,MEETINGHOST_CMD_REGISTER,buf,slen);
#else
		AsynModel::AsynResultRet ret = PostMsg(connid, 0,MEETINGHOST_CMD_REGISTER, buf, slen, false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
#endif
		if (0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]向host发送注册命令,同步失败,userid=%s,(%s:%d),次数:%d",regCmdOld._userid,m_hostip.c_str(),m_hostport,m_regCount);
			return -1;
		}

		m_regCount++;  //注册尝试次数
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 3000,"[host agent]向host发送注册命令,同步成功,token为空,userid=%s,(%s:%d),次数:%d",regCmdOld._userid,m_hostip.c_str(),m_hostport,m_regCount);
	}

	return 0;
}

int hostAgentSession::onRegisterResult(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(__MEETINGHOSTAGENT_SESSIONSTATE_REG != m_sessionState)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到注册请求的回应,但注册状态已经结束.注册状态值:%d,当前状态值:%d",__MEETINGHOSTAGENT_SESSIONSTATE_REG, m_sessionState);
		return -1;
	}

	meetingHost_cmd_reg_resp respCmd;
	int unlen = respCmd.unserialize((char*)data,len);
	if(unlen <0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到注册请求的回应,反序列化失败");
		return -1;
	}

	/*判断是否是最后请求的guid*/
	if(m_regGuid != respCmd._guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到注册请求的回应,guid校验错误.recv_guid:%s,local_guid:%s",
			respCmd._guid, m_regGuid.c_str());
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到注册成功回应,userid:%s,recv_guid:%s,local_guid:%s",m_userid.c_str(),respCmd._guid, m_regGuid.c_str());

#ifdef USE_RAW_SOCKET
	meeting_syncHead_ext *pext = (meeting_syncHead_ext *)(data + unlen);
	m_serverSessinid           = pext->ssrcid;
#else
	m_serverSessinid = srcsid;     //保存与host交互的sessionID，后续可以采用它继续进行交互
#endif

	m_sessionState = __MEETINGHOSTAGENT_SESSIONSTATE_HEART;
	if(false == m_bHeartTime)
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到注册成功回应,启动心跳定时器,userid:%s",m_userid.c_str());
		
		/*启动心跳*/
		m_lastHeartTime = GetTickCount64();
		heart_index     = 0;
		m_bHeartTime    = true;
		SetTimer(__MEETINGHOSTAGENT_TIME_HEART,10);
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到注册成功回应,心跳定时器已经处于启动状态,userid:%s",m_userid.c_str());
	}

	/*注册成功，发布分享地址*/
	//sendShareip();

	//主要是心跳超时,重新注册成功后要发布
	//if (0 == m_sendShareInfoFlag)
	{
		if (0 != m_shareport && !m_shareip.empty())
		{
			m_sendShareInfoFlag = 1;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到注册成功回应,处于未发布标识,但分享地址无效,不进行发布");
		}
	}
	
	return 0;
}

int hostAgentSession::OnTimerHeart(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	m_bHeartTime = false;

	if(!m_bHeart)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]接收到下线命令,心跳停止,user:%s,host(%s:%u)",m_userid.c_str(),m_hostip.c_str(),m_hostport);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]心跳超时,停止心跳 user:%s,host(%s:%u)",m_userid.c_str(),m_hostip.c_str(),m_hostport);

		/*重新获取路由*/
		StartRequestHost();

		m_hostport = 0;
		/*尝试停止分享*/
		m_bShareip = true;


		m_sendShareInfoFlag = 0;

		return 0;
	}

	/*发送心跳请求*/
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]收到下线命令,停止注册定时器");
		return -1;
	}
	
	/*检查注册结果*/
	if(__MEETINGHOSTAGENT_SESSIONSTATE_REG == m_sessionState)
	{
		if(m_regCount < 10)
		{
			/*发送注册请求*/
			sendRegisterToServer();
			
			m_registerFlag = 1;
			SetTimer(__MEETINGHOSTAGENT_TIME_REG,1000);
		}
		else
		{
			/*发送请求host命令*/
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
		/*向hostServer发送邀请命令*/
		if (0 == m_hostport || m_hostip.empty())
		{
			/*冗余检查错误*/
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到UI邀请命令,host地址无效,userid=%s",m_userid.c_str());
			CompositingCmd(respRoot,respStr,3005,-2,"邀请命令");
			sendCmdToUI((char*)respStr.c_str(),respStr.length());
			return -1;
		}

		AsynModel::ConnID connid = GetHostServerConnID();
		Json::Value root;
		if(true != Json_Util_Open(root,data))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[Host]接收到UI邀请命令,解析失败");
			CompositingCmd(respRoot,respStr,3005,-3,"邀请命令");
			sendCmdToUI((char*)respStr.c_str(),respStr.length());
			return -1;
		}

		CompositingCmd(respRoot,respStr,3005,0,"邀请命令");
		sendCmdToUI((char*)respStr.c_str(),respStr.length());
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到UI邀请命令,并回复成功");

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
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到UI邀请命令,拆分邀请命令,格式化json失败,accounted:%s,name:%s,accountid:%s,meetingid:%d,guid:%s",
				                                                    tempUserid.c_str(),name.c_str(),userid.c_str(),meetid,tmpGuid.c_str());
				return -1;
			}

			//保存到本地内存,等待回复;否则继续重发
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
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到UI邀请命令,向host发送邀请命令同步失败,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
				return -1;
			}

			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到UI邀请命令,向Host发送邀请命令成功,addr:%s:%d,accounted:%s,name:%s,accountid:%s,meetingid:%d,guid:%d",
			                                                                   m_hostip.c_str(),m_hostport,tempUserid.c_str(),name.c_str(),userid.c_str(),meetid,tmpGuid.c_str());
			
			
			/************************************************************************/
			/*                      记录发送邀请的记录                             */
			/************************************************************************/
			
			char recordName[128] = {0};
			sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

			FILE* pInviteEventRecord = fopen(recordName,"a+");
			if (pInviteEventRecord == NULL)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]文件 %s 无法打开",recordName);
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
		/*向ui发送注册失败*/
		//if (0 == CompositingCmd(respRoot,respStr,3005,-2,"onInvite"))
		//{
		//	sendCmdToUI((char*)respStr.c_str(),respStr.length());
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]发邀请命令,用户未注册");
		//}
		//else
		//{
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]向UI发送命令格式化失败(发邀请命令时未注册),sesseion未处于心跳状态");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到邀请回复命令,反序列化失败");
	}

	std::string guid = inviteRespCmd.guid;
	std::map<std::string,InviteCmdContext>::iterator it = m_sendInviteRecord.find(guid);
	if (it != m_sendInviteRecord.end())
	{
		std::string tmp_guid = it->first;
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到邀请回复命令,本地存在相应发送邀请记录,guid:%s 邀请者:%s 会议号:%d,本地记录为 guid:%s meetingid:%d",inviteRespCmd.guid,inviteRespCmd.invitid,inviteRespCmd.meetid,tmp_guid.c_str(),it->second.meetingid);
		
		/************************************************************************/
		/*                  收到邀请回复的记录                               */
		/************************************************************************/

		char recordName[128] = {0};
		sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

		FILE* pInviteEventRecord = fopen(recordName,"a+");
		if (pInviteEventRecord == NULL)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]文件 %s 无法打开",recordName);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到邀请回复命令,本地无相应发送邀请记录,guid:%s 邀请者:%s 会议号:%d",inviteRespCmd.guid,inviteRespCmd.invitid,inviteRespCmd.meetid);
	}

	return 0;
}

int hostAgentSession::onInvitedSingle(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	//1、无条件，回复server
	//2、根据GUID去查找本地map(m_recvInviteRecord)
	//3、存在,不处理
	//4、不存在，发送UI，并存储到map
	meetingHost_invitedCmd_Single inviteCmd;
	int unlen = inviteCmd.UnSeralize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,反序列化失败");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,guid:%s,邀请人:%s,会议号:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);

	//回复SERVER->AGENT
	meetingHost_invitedCmd_Single_Resp inviteResp;
	strncpy(inviteResp.guid,inviteCmd.guid,sizeof(inviteResp.guid)-1);
	strncpy(inviteResp.invitid,inviteCmd.inviteid,sizeof(inviteResp.invitid)-1);
	inviteResp.meetid = inviteCmd.meetid;

	char buf[256] = {0};
	int serlen = inviteResp.Seralize(buf,sizeof(buf));
	if (serlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,回复信息序列化失败");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到被邀请信息,向host发送邀请回复命令,同步失败,(%s:%d),guid:%s,邀请人:%s,meetingid:%d",server_conn_id.openhost.openUDPIP,m_hostport,inviteResp.guid,inviteResp.invitid,inviteResp.meetid);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到被邀请信息,向host发送邀请回复命令,同步成功,(%s:%d),guid:%s,邀请人:%s,meetingid:%d",server_conn_id.openhost.openUDPIP,m_hostport,inviteResp.guid,inviteResp.invitid,inviteResp.meetid);
	}

	//根据GUID去查找本地map(m_recvInviteRecord)
	std::map<std::string,I64>::iterator iter = m_recvInviteRecord.find(inviteCmd.guid);
	if (iter != m_recvInviteRecord.end())
	{
		//本地存在记录,不处理
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,guid本地存在,guid:%s,邀请人:%s,会议号:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);
		return -1;
	}
	else
	{
		//本地不存在,发送给UI并记录到本地
		Json::Value cmd;
		std::string result;
		if(0 != CompositingCmd(cmd,result,inviteCmd.inviteid,3006,inviteCmd.invitename,inviteCmd.meetid,"onInvited"))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,发送给UI组装JSON失败");
			return -1 ;
		}

		sendCmdToUI((char*)result.c_str(),result.length());
		
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到被邀请信息,本地无相关记录,发送给UI,guid:%s,邀请人:%s,会议号:%d",inviteCmd.guid,inviteCmd.inviteid,inviteCmd.meetid);

		//记录已发信息
		m_recvInviteRecord[inviteCmd.guid] = GetTickCount64();


		/************************************************************************/
		/*                      接收到被邀请记录                                */
		/************************************************************************/
		char recordName[128] = {0};
		sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

		FILE* pInviteEventRecord = fopen(recordName,"a+");
		if (pInviteEventRecord == NULL)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]文件 %s 无法打开",recordName);
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
	/*向UI发送被邀请命令*/
	meetingHost_invitedCmd ivcmd;
	int unSerLen = ivcmd.UnSeralize((char*)data,len);
	if (unSerLen <= 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]被邀请命令反序列化失败,serlen = %d",unSerLen);
		return -1;
	}

	Json::Value cmd;
	std::string result;
	if(0 != CompositingCmd(cmd,result,ivcmd.inviteid,3006,ivcmd.invitename,ivcmd.meetid,"onInvited"))
	{
		return -1 ;
	}

	sendCmdToUI((char*)result.c_str(),result.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]收到被邀请命令,并发送给UI,inviteid:%s,invitename:%s,meetingid:%d,invited:%s",ivcmd.inviteid,ivcmd.invitename,ivcmd.meetid,ivcmd.invited);

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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,定时器状态被置为false");
		return 0;
	}

	if (m_sendShareCount > 20)
	{
		delete [] m_shareInfo;
		m_isSendShareInfo = false;
		m_sendShareCount  = 0;
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,尝试次数达到20,定时器停止");
		return 0;
	}

	//次数加一
	m_sendShareCount++;

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,读取文件名为:%s",g_shareScreenConfigPath.c_str());
	FILE *pfile = fopen(g_shareScreenConfigPath.c_str(),"r+");
	if (pfile == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,读取文件指针为空");
		return 0;
	}

	char buff[32]= {0};
	if(fgets(buff,32,pfile) == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,读取内容为空");
		return 0;
	}
	m_remotePort = atoi(buff);
	fclose(pfile);
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,读取文件为:%s,port:%d",g_shareScreenConfigPath.c_str(),m_remotePort);

	AsynModel::ConnID connid;
	strncpy(connid.openhost.openUDPIP,m_remoteIp.c_str(),sizeof(connid.openhost.openUDPIP)-1);
	connid.openhost.openUdpPort = m_remotePort;

	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_NOTICEPCADDR,m_shareInfo,m_shareInfoLen,true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
	if (0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,并向（%s|%d)发送成功失败",m_remoteIp.c_str(),m_remotePort);
		return -1;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]获取屏幕分享地址,并向（%s|%d)发送成功",m_remoteIp.c_str(),m_remotePort);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]收到打洞信息,反序列化失败");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][PUNTCH]收到打洞信息,userid:%s,publicAddr:(%s|%d)",msg.userid,msg.publicIp,msg.publicPort);

	//启动定时器,改变状态
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]接收到推荐信息,反序列化失败");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][RECOMMENDTION]接收到推荐信息,信息为userid:%s,guid:%s,sourceid:%s,contactlistname:%s,contactlistversion:%d",
		                 recMsg.userid.c_str(),recMsg.guid.c_str(),recMsg.sourceId.c_str(),recMsg.contactListName.c_str(),recMsg.contactListVersion);

	//无条件回复sever，接收推荐信息成功
	recommendResp((char*)recMsg.guid.c_str(),(char*)recMsg.userid.c_str(),srcsid,sender);

	//保存到内存
	//先检查已推荐记录,进行校验：存在就比较版本号,保存版本号新的；不存在,查看本地记录，比较版本号
	std::map<std::string,int>::iterator iter = m_saveRecVersion.find(recMsg.contactListId);
	if (iter != m_saveRecVersion.end())
	{
		if (recMsg.contactListVersion > iter->second)
		{
			m_saveRecTaskManage[recMsg.contactListId] = recMsg;
		}
		else
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent][RECOMMENDTION]接收到推荐信息,已推荐记录中存在该通讯录ID推荐信息,接收到的版本号较旧,id:%s,name:%s,%localversion:%d,arrversion:%d",
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
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐信息,已推荐记录中不存在,本地记录存在,接收到的版本号较旧,id:%s,name:%s,%localversion:%d,arrversion:%d",
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

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐信息,被推荐人为%s,推荐客户端:%s,GUID:%s,推荐联系人个数:%d,反序列化大小:%d",recMsg.userid.c_str(),recMsg.sourceId.c_str(),recMsg.guid.c_str(),recMsg.contacters.size(),unLen);
	return 0;
}

int hostAgentSession::onRecommendRequest(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	NotifyRecommendMsg recNotify;
	int unlen = recNotify.Unserialize((char*)data,len);
	if (unlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,反序列化失败");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,userid:%s,guid:%s",recNotify.userid.c_str(),recNotify.guid.c_str());

	std::map<std::string,std::string>::iterator it = m_recMsgCheck.find(recNotify.guid);
	if (it != m_recMsgCheck.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,本次存在该推荐记录,为重复推荐,guid:%s",recNotify.guid.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,向SERVER发送获取推荐信息,反序列化失败,userid:%s,guid:%s",recNotify.userid.c_str(),recNotify.guid.c_str());
		return -1;
	}

	//TCP主动连接，获取推荐信息
	AsynModel::ConnID connid;
	memset(&connid,0,sizeof(connid));
	strncpy(connid.openhost.openTCPIP,m_hostip.c_str(),sizeof(connid.openhost.openTCPIP)-1);
	connid.openhost.openTcpPort = m_hostport;

	AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_GET_RECOMMENDMSG_REQUEST,buf,serlen,false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::TCP);
	if(0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,向server发送获取推荐信息命令失败,userid:%s,host:(%s|%d)",recNotifyResp.userid.c_str(),m_hostip.c_str(),m_hostport);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]接收到推荐命令通知命令,向server发送获取推荐信息命令成功,userid:%s,host:(%s|%d)",recNotifyResp.userid.c_str(),m_hostip.c_str(),m_hostport);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到心跳回应,但长度校验错误,丢弃数据,addr:%s:%d", m_hostip.c_str(), m_hostport);
		return 0;
	}

	meetingHost_cmd_HeartBeatRespMsg* pResponse = &Response;
	/*判断guid是否相同*/
	if (string(pResponse->_guid) != m_regGuid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到心跳回应,guid错误,addr:%s:%d,recv_guid:%s,local_guid:%s,心跳序号:%d",
			                                                  m_hostip.c_str(), m_hostport,pResponse->_guid,m_regGuid.c_str(),Response.index);
		return -1;
	}

	Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到心跳回应,user:%s,addr:%s:%d,心跳序号:[%d]",m_userid.c_str(),m_hostip.c_str(),m_hostport,Response.index);

	/*更新心跳响应时间*/
	m_lastHeartTime = GetTickCount64();

	return 0;
}

int hostAgentSession::sendheart()
{
	meetingHost_cmd_HeartBeatReqMsg hbmsg;

	/*添加数据*/
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]向host发送心跳,序列化失败:(%s:%u)user_id:%s, guid:%s, 心跳序号:%d",
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]向host发送心跳,同步失败,host:(%s:%u),userid:%s,guid:%s,心跳序号:%d",
			server_conn_id.openhost.openUDPIP, m_hostport, hbmsg._userid, hbmsg._guid, hbmsg.index);
		return -1;
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]向host发送心跳,同步成功,host:(%s:%u),userid:%s,guid:%s,心跳序号:%d",
			server_conn_id.openhost.openUDPIP, m_hostport, hbmsg._userid, hbmsg._guid, hbmsg.index);
	}
	return 0;
}

/*获得会议路由地址*/
bool  hostAgentSession::GetRouteServerAddrFromUrl(const std::string & routeUrl)
{
	int pos = routeUrl.find("|");
	if(pos < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetRouteServerAddrFromUrl]调用失败,找不到关键字|,routeUrl:%s,pos:%d",routeUrl.c_str(),pos);
		return false;
	}

	std::string  tempStr = routeUrl.substr(0,pos);
	bool bSucc = GetServerAddrFromUrl(tempStr,m_routeServerIp1,m_routeServerPort1);
	if(!bSucc)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]调用失败 m_routeServerIp1");
		return false;
	}

	tempStr = routeUrl.substr(pos+1,routeUrl.length()-pos-1);
	bSucc = GetServerAddrFromUrl(tempStr,m_routeServerIp2,m_routeServerPort2);
	if(!bSucc)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]调用失败 m_routeServerIp2");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]调用失败,routeUrl:%s 里面找不到分隔符:",routeUrl.c_str());
		return false;
	}

	meetingCtrlServerPort = atoi(routeUrl.substr(pos+1,routeUrl.length() - pos-1).c_str());
	meetingCtrlServerIp   = routeUrl.substr(0,pos);

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][GetServerAddrFromUrl]调用成功,routeUrl:%s,解析出IP:%s,Port:%u",
										routeUrl.c_str(),meetingCtrlServerIp.c_str(),meetingCtrlServerPort);

	return true;
}

int hostAgentSession::onUICmd( const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender )
{
	/*解析JSON流 根据cmdid处理命令*/
	Json ::Value root;
	if (true != Json_Util_Open(root,data))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收UI命令,jason解析失败");
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收UI命令,内容为:%s",data);

	int cmdid = root["cmdid"].asInt();
	switch(cmdid)
	{
	case 3000:/*发布分享信息*/
		{
			onShareInfo(data,len,srcsid,sender);
		}
		break;
	case 3004:/*邀请用户*/
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到发布分享信息命令,无法解析");
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
		/*返回ui -1，日志*/
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到UI的发布分享信息命令,解析后,shareip或shareport错误");
		respRoot["status"] = -1;
	}
	else
	{
		/*返回给UI发布分享命令成功*/
		respRoot["status"] = 0;
		m_sendShareInfoFlag = 1;
		m_bShareguid = GetGuidStr();
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到UI的发布分享信息命令,shareip:%s,shareport:%d",m_shareip.c_str(),m_shareport);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到发布请求的回应,反序列化失败");
		return -1;
	}

	/*判断是否是最后请求的guid*/
	if(m_bShareguid != shareResp._guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到发布请求的回应,guid校验错误.recv_guid:%s,local_guid:%s",shareResp._guid, m_bShareguid.c_str());
		return -1;
	}

	//发布成功
	m_bShareip = true;    

	m_sendShareInfoFlag = 0;
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到发布请求的回应,发布成功,userid:%s.",m_userid.c_str());

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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][DOWNLINE]接收到下线通知命令,反序列化失败");
		return -1;
	}

	/*判断是否是最后请求的guid*/
	if(m_regGuid != sharePhoneInfoResp.guid)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]接收到下线通知命令,guid校验错误. recv_guid:%s, local_guid:%s",sharePhoneInfoResp.guid.c_str(), m_bShareguid.c_str());
		return -1;
	}

	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]接收到下线通知命令,userid:%s status:%d remote:%s:%d",sharePhoneInfoResp.uerid.c_str(),sharePhoneInfoResp.status,remote_addr_str,tempport);

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

	//回复server收到下线命令
	meetinghost_cmd_sharePhoneInfoResp downlineresp;
	downlineresp.uerid  = sharePhoneInfoResp.uerid;
	downlineresp.token  = sharePhoneInfoResp.token;
	downlineresp.guid   = sharePhoneInfoResp.guid;
	downlineresp.status = sharePhoneInfoResp.status;

	char buf[256] = {0};
	int serlen = downlineresp.Serialize(buf,sizeof(buf));
	if (serlen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[Host][DOWNLINE]接收到下线通知命令,向Host发送回复,序列化失败,userid:%s",sharePhoneInfoResp.uerid.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 5000,"[host agent][DOWNLINE]接收到下线通知命令,向Host发送回复,同步失败,userid:%s,host:%s:%d",sharePhoneInfoResp.uerid.c_str(),m_hostip.c_str(),m_hostport);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][DOWNLINE]接收到下线命令请求,向Host发送回复,同步成功,userid:%s,host:%s:%d",sharePhoneInfoResp.uerid.c_str(),m_hostip.c_str(),m_hostport);
	}

	return 0;
}

int hostAgentSession::OnTimerShareip(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	//m_bShareTimeStart = false;
	//if(true != m_bShareip)
	//{
	//	/*继续发布*/
	//	sendShareip();
	//}
	return 0;
}

void hostAgentSession::sendShareInfoToServer()
{
	if(0 == m_hostport || m_hostip.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]向Host发送分享信息,Host地址无效");
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

		//序列化
		char buf[256] = {0};
		int sslen = shareip.serialize(buf,sizeof(buf));
		if(sslen < 0)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]向Host发送分享信息,hostAdrr:(%s|%d),userid:%s,shareInfo:(%s|%d|%s),序列化失败",
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]向Host发送分享信息,同步发送失败,hostAdrr:(%s|%d),userid:%s,shareInfo:(%s:%d),name:%s",
				m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
		}
		else
		{
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]向Host发送分享信息,同步发送成功,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s,guid:%s",
				m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str(),m_bShareguid.c_str());
		}
	}

	return;
}


void hostAgentSession::sendShareip()
{
	if(0 == m_shareport)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]发布分享信息,分享信息错误");
		return ;
	}

	AsynModel::ConnID conid = GetHostServerConnID();
	if(conid.openhost.openUdpPort == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]发布分享信息,host地址无效");
		return ;
	}

	m_bShareip = false;               //是否发布成功
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
	
	//序列化
	char buf[256] = {0};
	int sslen = shareip.serialize(buf,sizeof(buf));
	if(sslen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]发布分享信息,userid:%s,shareInfo:(%s|%d|%s),序列化失败",
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]发布分享信息,同步发送失败,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s",
			                    m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]发布分享信息,同步发送成功,hostAdrr:(%s|%d),userid:%s,shareAddr:(%s:%d),name:%s",m_hostip.c_str(),m_hostport,m_userid.c_str(),m_shareip.c_str(),m_shareport,m_name.c_str());
	}

	return;
}

int hostAgentSession::getRecommendMsgAbstractWork(std::string contactlistid,char* outbuf,int &outlen)
{
	if (m_saveRecTaskManage.size() == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI查询推荐信息,本地无任何推荐信息");
		return -1;
	}
	
	if (contactlistid.empty())
	{
		//不指定通讯录ID,为查询所有推荐信息
		std::string result;
		buildQueryRecommendMsg(m_saveRecTaskManage,result);
		memcpy(outbuf,result.c_str(),result.length());

		outlen = result.length();

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI查询推荐信息,查询所有通讯录,data:%s",result.c_str());

		return 0;
	}
	else
	{
		//查询指定推荐通讯录ID的信息
		std::map<std::string,PerUserRecommendMsg>::iterator it2 = m_saveRecTaskManage.find(contactlistid);
		if (it2 == m_saveRecTaskManage.end())
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI获取推荐信息,无该信息,contactlistid:%s",contactlistid.c_str());
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

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI查询推荐信息,查询通讯录为:%s,data:%s,该通讯录推荐信息大小:%d",contactlistid.c_str(),record2.c_str(),outlen);
		
		return 0;
	}
}

int hostAgentSession::getRecommendMsgWork(std::string contactlistid,char* outbuf)
{
	if (contactlistid.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI获取推荐信息,通讯录ID为空");
		return -1;
	}

	std::map<std::string,PerUserRecommendMsg>::iterator it = m_saveRecTaskManage.find(contactlistid);
	if (it == m_saveRecTaskManage.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI获取推荐信息,无该推荐信息,contactlistid:%s",contactlistid.c_str());
		return -1;
	}

	std::string result;
	buildPerUserRecommendMsg(it->second,result);
	memcpy(outbuf,result.c_str(),result.length());

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]UI获取推荐信息,contactlistid:%s,contactlistname:%s,conntactlistverison:%d,sourid:%s,contactescount:%d,len:%d",
		contactlistid.c_str(),it->second.contactListName.c_str(),it->second.contactListVersion,it->second.sourceId.c_str(),it->second.contacters.size(),result.length());

	//记录发送该通讯录ID的版本号
	m_saveRecVersion[contactlistid] = it->second.contactListVersion;

	//删除本地存储信息
	m_saveRecTaskManage.erase(it);
	deleteRecMsgFromDisk(g_recMsglocalpath,contactlistid);

	return 0;

}

int hostAgentSession::deleteRecommendMsgWork(std::string contactlistid)
{
	if (contactlistid.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]删除推荐信息,通讯录ID为空");
		return -1;
	}

	std::map<std::string,PerUserRecommendMsg>::iterator it = m_saveRecTaskManage.find(contactlistid);
	if (it == m_saveRecTaskManage.end())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]删除推荐信息,无该信息记录,contactlistid:%s",contactlistid.c_str());
		return -1;
	}

	//删除内存存储以及本地记录
	m_saveRecTaskManage.erase(it);
	deleteRecMsgFromDisk(g_recMsglocalpath,contactlistid);

    Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]删除推荐信息,成功,contactlistid:%s",contactlistid.c_str());

	return 0;
}

int hostAgentSession::deleteRecommendMsgWorkRecord(std::string contactlistid)
{
	std::map<std::string,int>::iterator it = m_saveRecVersion.find(contactlistid);
	if (it != m_saveRecVersion.end())
	{
		m_saveRecVersion.erase(it);
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]删除推荐记录,通讯录ID为:%s",contactlistid.c_str());
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]删除推荐记录,通讯录ID为:%s,本地无该通讯录推荐记录",contactlistid.c_str());
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

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到推荐信息,回复SERVER,user:%s guid:%s status:%d",recMsgResp.userid.c_str(),recMsgResp.guid.c_str(),recMsgResp.status);

	char buf[128] = {0};
	int sslen = recMsgResp.Serialize(buf,sizeof(buf));
	if(sslen < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到推荐信息,回复SERVER,序列化失败,userid:%s",recMsgResp.userid.c_str());
		return -1;
	}

	//TCP链路回复
	AsynModel::AsynResultRet ret = PostMsg((AsynModel::SptrISender&)sender,srcsid,MEETINGHOST_CMD_AGENT_RECOMMEND_RESP,buf,sslen,false);
	if (0 != ret)	
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到推荐信息,回复SERVER,同步发送失败,userid:%s",recMsgResp.userid.c_str());
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收到推荐信息,回复SERVER,同步发送成功,user:%s,guid:%s,status:%d",recMsgResp.userid.c_str(),recMsgResp.guid.c_str(),recMsgResp.status);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s]命令格式化json失败",methodMsg.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] 命令格式化json失败",methodMsg.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] 命令格式化json失败",methodMsg.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] 命令格式化json失败",methodMsg.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][hostAgentSession::%s] 命令格式化json失败",methodMsg.c_str());
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
		 Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]邀请命令订阅回应: receptType:%d,receptResult:%d,msgId:%d",
			 receptType,receptResult,msgId);

		 int ret = 0;
		 if(receptResult != AsynModel::RECEPT_SUCCESS)
		 {
			 ret = -3;
		 }
		 /*返回UI命令发送成功*/
		 std::string respStr;
		 Json::Value respRoot;
		 CompositingCmd(respRoot,respStr,3005,ret,"邀请命令");
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]Json操作,失败,buff:%s",buff);
			return false;
		}
	}
	catch (...)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]Json操作,异常,buff:%s",buff);
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到bug分析引擎请求命令,反序列化失败");
		return -1;
	}
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到获取日志命令,bugEngienIP:%s,bugEngienPort:%d",notregistercmd.BugEngineIP,notregistercmd.BugEnginePort);

	/************************************************************************/
	/*     解析参数，调用bugEngien接口                                     */
	/************************************************************************/
//#ifdef WIN_TEST
//	char strFilePath[300] ={0};
//	DWORD dwError = GetModuleFileName(NULL,strFilePath,MAX_PATH );
//	if (dwError>0)
//	{
//		PathRemoveFileSpec(strFilePath);
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent] 获取当前目录成功:%s ！！ ",strFilePath );
//
//	}
//	else
//	{
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent] 获取当前目录失败！");
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
//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到获取日志命令,启动成功");
//	}
//	else
//	{
//		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到获取日志命令,启动失败");
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
				/*                  未收到邀请回复的记录                               */
				/************************************************************************/

				char recordName[128] = {0};
				sprintf(recordName,"%s/inviterecord_%s.txt",g_inviteRecordlocalpath.c_str(),m_userid.c_str());

				FILE* pInviteEventRecord = fopen(recordName,"a+");
				if (pInviteEventRecord == NULL)
				{
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]文件:%s 无法打开",recordName);
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

				Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时发送未收到回复的邀请命令,重发大于10次,GUID:%s,meetingid:%d",guid.c_str(),iter0->second.meetingid);
				m_sendInviteRecord.erase(iter0++);
			}
			else
			{
				//重发次数递增
				iter0->second.sendInviteMsgCount++;

				AsynModel::ConnID connid = GetHostServerConnID();
				if(connid.openhost.openUdpPort == 0)
				{
					/*冗余检查错误*/
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]定时发送未收到回复的邀请命令,未大于10次,userid=%s,向host发送邀请时,地址不可用",m_userid.c_str());
				}

				AsynModel::AsynResultRet ret = PostMsg(connid,0,MEETINGHOST_CMD_INVITE_SINGLE,iter0->second.InviteData,sizeof(iter0->second.InviteData),true,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_ALL,AsynModel::UDP);
				if (0 != ret)
				{
					Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]定时发送未收到回复的邀请命令,未大于10次,向Host发送邀请命令,同步失败,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(), m_hostport);
				}
				else
				{
					Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时发送未收到回复的邀请命令,未大于10次,向Host发送邀请命令,同步成功,(addr:%s:%d),userid:%s,meetingid:%d,guid:%s",m_hostip.c_str(),m_hostport,m_userid.c_str(),iter0->second.meetingid,guid.c_str());
				}				
				iter0++;
			}
		}
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时发送未收到回复的邀请命令,发送推荐信息记录为空");
	}
	
	/************************************************************************/
	/*                       定时检查邀请记录，超时删除                    */
	/************************************************************************/
	if (m_recvInviteRecord.size() != 0)
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时检查被邀请历史记录,本地保存记录数为:%d",m_recvInviteRecord.size());

		I64 timeNow = GetTickCount64();
		std::map<std::string,I64>::iterator iter1 = m_recvInviteRecord.begin();
		while(iter1 != m_recvInviteRecord.end())
		{
			if (timeNow - iter1->second > INVITE_RECEIVE_GUID_TIME_OUT)
			{
				std::string guid2 = iter1->first;
				Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时检查被邀请历史记录,guid:%s 超过30秒删除",guid2.c_str());
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
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]定时检查被邀请历史记录,本地保存记录数为空");
	}

	SetTimer(__MEETINGHOSTAGENT_TIME_INVITEANDCHECK,INVITE_CHECK_TIME_STEP);

	return 0;
}

int hostAgentSession::onTimerSetLocal(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	//定时将内存中的推荐信息刷新到本地
	if (m_saveRecTaskManage.size() != 0)
	{
		saveRecMsgToDisk(g_recMsglocalpath,m_saveRecTaskManage);
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]定时将内存推荐信息,刷新到本地,内存任务数为:%d",m_saveRecTaskManage.size());
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]定时将内存推荐信息,刷新到本地,内容任务数为空");
	}

	//定时将内存中的推荐记录刷新到本地
	if (m_saveRecVersion.size() != 0)
	{
		saveRecRecordMsgToDisk(g_recRecordlocalpath,m_saveRecVersion);
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]定时将已经推荐的记录,刷新到本地,内存任务数为:%d",m_saveRecVersion.size());
	}
	else
	{
		Log::writeMessage(MEETING_HOST_AGENT_LOG_SUBTYPE,2000,"[host agent]定时将已经推荐的记录,刷新到本地,内容任务数为空");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecRecordMsgToDisk]XML文件解析错误!");
		return -101;
	}

	TiXmlElement* RootElement = NULL;
	RootElement = myDocument.FirstChildElement("RecommendRecords");
	if (!RootElement)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[Host][saveRecRecordMsgToDisk]XML无RecommendRecords节点!");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readRecRecordMsgFromDisk]加载本地存储信息不存在,path:%s",path.c_str());
		return -1;
	}

	recommend = myDocument.FirstChildElement("RecommendRecords");
	if (!recommend)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readRecRecordMsgFromDisk]无RecommendRecords节点,path:%s",path.c_str());
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecMsgToDisk]XML文件解析错误!");
		return -101;
	}

	TiXmlElement* RootElement = NULL;
	RootElement = myDocument.FirstChildElement("RecommendInfo");
	if (!RootElement)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][saveRecMsgToDisk]XML无RecommendInfo节点!");
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]加载本地路径为空");
		return -1;
	}

	if (!myDocument.LoadFile(path.c_str()))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]加载本地存储信息不存在,path:%s",path.c_str());
		return -1;
	}

	recommend = myDocument.FirstChildElement("RecommendInfo");
	if (!recommend)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][readAllRecMsgFromDisk]无RecommendInfo节点,path:%s",path.c_str());
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

		//KEY不能为空,需要判断

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

		recMsg.insert(make_pair(tmpkey,userinfo));	//存入multimap

		oneUser = oneUser->NextSiblingElement();
	}

#endif
	return 0;
}

int hostAgentSession::tansToPerUerRecMsgMap(std::multimap<std::string,Contact> msg,std::map<string,PerUserRecommendMsg> &recMsg)
{
	//一个taskID可对应多个推荐信息（Contct）,将其转化为map<destID,PerUserMSG>的形式
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

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,5000,"[host agent][tansToPerUerRecMsgMap]本地保存的推荐信息,任务数为:%d",recMsg.size());

	return 0;
}

int hostAgentSession::deleteRecMsgFromDisk(std::string path,std::string recKey)
{
	TiXmlDocument myDocument;
	TiXmlElement* root = NULL ;

	if (!myDocument.LoadFile(path.c_str()))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][deleteRecMsgFromDisk]无法加载本地存储推荐信息文件,path:%s",path.c_str());
		return -1;
	}

	root = myDocument.FirstChildElement("RecommendInfo");
	if (!root)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent][deleteRecMsgFromDisk]无RecommendInfo节点,path:%s",path.c_str());
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

		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到UI转发数据命令,向host发送命令同步失败,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
		return -1;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]接收到UI转发数据命令,向Host发送命令成功,addr:%s:%d",
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到HOST转发数据命令,转发目标视讯号与本地视讯号不一致，丢弃。from:%s,to:%s,local:%s",
			datactx.from_userid, datactx.to_userid, m_userid.c_str());
		return -1;
	}

	//回调
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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到HOST转发数据命令,向host发送应答命令同步失败,userid=%s,addr:%s:%d",m_userid.c_str(),m_hostip.c_str(),m_hostport);
	}
	return 0;
}

int hostAgentSession::onRecvTransDataResp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_trans_data datactx;
	int unSersize = datactx.UnSeralize(data,len);

	if (m_userid.compare(datactx.from_userid) != 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到HOST转发数据命令回应,发送者视讯号与本地视讯号不一致，丢弃。from:%s,to:%s,local:%s",
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
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host agent]接收到UI转发数据命令,向host发送命令同步失败[uid:%lld,第%d次],userid=%s,addr:%s:%d",
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
