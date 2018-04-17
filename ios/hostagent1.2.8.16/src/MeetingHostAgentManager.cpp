#include "stdafx.h"

#include "MeetingHostAgentManager.h"
#include "configParser.h"

extern CConfigParser_STPIPMAP g_ConfigParser;

bool isValidIP(string ip);
bool checkIsRightType(const string &srcStr);
int splitString(const string &srcStr, const string &splitStr, vector<string> &destVec);

int getStpRCUrl(std::string routeUrl,std::string & ip);

#ifdef _ANDROID_NDK_
#include <android/log.h>
#define __AND_LOG_TAG "meetingHost"
#define __AND_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , __AND_LOG_TAG, __VA_ARGS__)
#define __AND_LOGI(...) __android_log_print(ANDROID_LOG_INFO   , __AND_LOG_TAG, __VA_ARGS__)
#define __AND_LOGW(...) __android_log_print(ANDROID_LOG_WARN   , __AND_LOG_TAG, __VA_ARGS__)
#define __AND_LOGE(...) __android_log_print(ANDROID_LOG_ERROR   , __AND_LOG_TAG, __VA_ARGS__)
#else
#define __AND_LOGD(...)
#define __AND_LOGI(...)
#define __AND_LOGW(...)
#define __AND_LOGE(...)
#endif

#ifdef _ABI_ARMEABIV7A_
#define DETECT_VERSION  "(armeabi-v7a)"
#elif defined(_ABI_X86_)
#define DETECT_VERSION  "(x86)"
#elif __IOS__
#define DETECT_VERSION "(ios)"
#else
#ifdef X64
#define DETECT_VERSION "(win64)"
#else
#define DETECT_VERSION "(win32)"
#endif
#endif

#define CURRENT_VERSION "justmeeting_version 1.2.8.16" DETECT_VERSION 

MeetingHostAgentManager::MeetingHostAgentManager()
{
	m_start          = 0;
	m_pclientSession = NULL;

#ifdef USE_RAW_SOCKET
	m_manageSid      = 0;
	m_manageSession  = NULL;
#endif

	m_commonTransDataCB = NULL;
}

MeetingHostAgentManager::~MeetingHostAgentManager()
{
	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]MeetingHostAgentManager析构");
}

int  MeetingHostAgentManager::init(std::string logfile,std::string logOutDir)
{
#if defined(_ANDROID_NDK_) || defined(__IOS__)
	std::string indir;
	std::string outdir;
	
	indir = logfile;
	indir += "/Log.xml";

	outdir = logOutDir;
	//outdir += "/hostagent";

	if(logfile.empty())
	{
		Log::open(true,(char*)"-dGMfyds",false,(char*)"/mnt/sdcard/Log.xml",(char*)"/mnt/sdcard/hostagent");
	}
	else
	{
		Log::open(true,(char*)"-dGMfyds",false,(char*)indir.c_str(),(char*)outdir.c_str());
	}

	__AND_LOGW("[hostAgent Version : %s]", CURRENT_VERSION);
	__AND_LOGW("[hostAgent log in_path:%s, out_path:%s]",indir.c_str(), outdir.c_str());

#else
	if (logfile.empty() || logOutDir.empty())
	{
		Log::open(true,"-dGMfyds",false);
	}
	else
	{
		std::string indir;

		indir = logfile;
		indir += "/Log.xml";

		Log::open2(true,"-dGMfyds",false,(char*)indir.c_str(),(char*)logOutDir.c_str());
	}
#endif

	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 1000,"[MeetingHostAgent Version : %s]", CURRENT_VERSION);

	AsynModel::AsynResultRet ret0 = InitAsynModel(5);
	if(ret0 != AsynModel::SUCCESS)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::init][host agent]异步模型初始化失败,ret0:%d",ret0);
		return ret0;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::init][host agent]异步模型初始化成功");
	}
	
	AsynModel::AsynResultRet ret1 = StartAsynModel();
	if(ret1 != AsynModel::SUCCESS)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::init][host agent]异步模型启动失败:ret1:%d",ret1);
		return ret1;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::init][host agent]异步模型启动成功");
	}

	return 0;
}

bool MeetingHostAgentManager::Start2(std::string accounted,
	std::string localip,int localport,   
	string localCmdIp,int  localCmdPort,
	string UICmdIp,int  UICmdPort,
	string  rcAddr,string token,unsigned short& returnPort)
{
	return Start(accounted,localip,localport,localCmdIp,localCmdPort,UICmdIp,UICmdPort,rcAddr,token,returnPort);
}

bool  MeetingHostAgentManager::Start(std::string accounted,
									std::string localip,int localport,   
									string localCmdIp,int  localCmdPort,
									string UICmdIp,int  UICmdPort,
									string  rcAddr,string token,unsigned short& returnPort)
{ 
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][MeetingHostAgentManager::Start]接口被调用,userid:%s,localip:%s,localPort:%u,localCmdIp:%s,localCmdPort:%u,UICmdIp=%s,UICmdPort=%d,rcAddr:%s",
		                                             accounted.c_str(),localip.c_str(),localport,localCmdIp.c_str(),localCmdPort,UICmdIp.c_str(),UICmdPort,rcAddr.c_str());

	if(accounted.empty() || localport == 0 ||
		localip.empty() || localCmdIp.empty() || localCmdPort == 0 || 
		UICmdIp.empty() || UICmdPort == 0 ||
		rcAddr.empty())
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][MeetingHostAgentManager::Start]接口被调用,传入的参数错误!");
		return false;
	}

	if(1 == m_start)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][MeetingHostAgentManager::Start]接口被调用,重复调用错误!");
		return false;
	}

	/*rcAddr格式检查*/
	if (!checkIsRightType(rcAddr))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][MeetingHostAgentManager::Start]接口被调用,地址输入格式错误!");
		return false;
	}

	//启动定时器
	//m_timer.Enable();

	//获取STPRC地址
	std::string ip;
	getStpRCUrl(rcAddr,ip);

	//获取本地IP,用于打日志
	T_IP  localIPTemp;
	if(!GMGetLocalHostActiveIp(localIPTemp,(char*)ip.c_str()))
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,获取本地IP失败,stpRC:%s",ip.c_str());
#ifdef __IOS__
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,IOS版本,获取本地IP失败,stpRC:%s",ip.c_str());
		return false;
#endif
	}
	else
	{
		Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,获取本地IP成功,stpRC:%s,localip:%s",ip.c_str(),localIPTemp);
#ifdef __IOS__
		//本地IP采用自己获取的值
		localip = localIPTemp;
#endif
	}

#ifdef USE_RAW_SOCKET
	/************************************************************************/
	/*              增加ManageSession接收命令                              */
	/************************************************************************/   
	MeetingHostAgentManagerSession* tmp_managesesion = new(nothrow) MeetingHostAgentManagerSession(this);
	if (NULL == tmp_managesesion)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,创建MeetingHostAgentManagerSession为NULL");
		return false;
	}
	tmp_managesesion->m_localIp = localip;
	strncpy(tmp_managesesion->m_stpRCIP,ip.c_str(),sizeof(tmp_managesesion->m_stpRCIP)-1);
    if(tmp_managesesion->init() != 0)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,MeetingHostAgentManagerSession初始化失败");
		return false;
	}

	if (tmp_managesesion->Start() < 0)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,MeetingHostAgentManagerSession启动失败");
		return false;
	}

	int tmp_manageSid = tmp_managesesion->GetSessionID();
	m_manageSession   = tmp_managesesion;

	PostMsg(tmp_managesesion->GetSessionID(),MEETINGMANAGE_SEESION_RECREATE,NULL,0);
#endif
	
	m_pclientSession = new hostAgentSession;
	if(NULL == m_pclientSession)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::Start]接口被调用,创建工作hostAgentSession失败!");
		return false;
	}

#ifdef USE_RAW_SOCKET 
	//工作Session保存ManageSeesionID
	m_pclientSession->m_manageSid = tmp_manageSid;
#endif

	m_pclientSession->m_userid     = accounted;
	m_pclientSession->m_rcAddr     = rcAddr;
	m_pclientSession->m_checktoken = token;
	
	//Puntch功能
	m_pclientSession->m_remoteIp   = localip;

	strncpy(m_recvCtrl.m_targetIP,UICmdIp.c_str(),sizeof(m_recvCtrl.m_targetIP)-1);
	m_recvCtrl.m_targetPort = UICmdPort;
	
	m_pclientSession->m_recv = &m_recvCtrl;

	/*创建工作session*/
	if(m_pclientSession->Start() < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::Start]接口被调用,工作hostAgentSession启动失败！");
		return false;
	}

	m_recvCtrl.m_workSessionId = m_pclientSession->GetSessionID();

	unsigned short localUsePort = localport;
	if(!StartRecvCtrl(localCmdIp,localUsePort,UICmdIp,UICmdPort))
	{
		return false;
	}
	
	//记录接收UI命令绑定成功的IP以及端口号,用于重建与UI交互的SOCKET使用
	memcpy(m_recvCtrl.m_localIP1,localCmdIp.c_str(),localCmdIp.length());
	m_recvCtrl.m_localPort1 = localUsePort;

	//返回可用端口
	returnPort = localUsePort;
	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[host agent][MeetingHostAgentManager::Start]接口被调用,已经确定空闲端口,tempFreePortReturn:%d",returnPort);

	RegistEventCallBackInterface(this);

	//设置UDP监听端口
	PostMsg(tmp_manageSid,MEETINGMANAGE_SEESION_RESETUDP,NULL,0);
	
	//unsigned short udpUsePort = localport;
	//if(!SetUdpPort_Uniform(localip,udpUsePort))
	//{
	//	return false;
	//}
	
	//PostMsg(tmp_manageSid,MEETINGMANAGE_SEESION_RESETTCP,NULL,0);
	//客户端发送时,异步模型会自动寻找端口发送
	//unsigned short tcpUsePort = localport;
	//if (!SetTcpPort_Uniform(localip,tcpUsePort))
	//{
	//	return false;
	//}

	m_localip = localip;
	AsynModel::AsynResultRet ret3 = PostMsg(m_pclientSession->GetSessionID(),MEETINGHOST_REGISTER,NULL,0);
	if(0 != ret3)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::Start]接口被调用,向工作SESSION发送注册命令失败!");
		return false;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::Start]接口被调用,向工作SESSION发送注册命令成功!");
	}

	m_start = 1;
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::Start]接口被调用,调用成功!");
	
	return true;
}

int MeetingHostAgentManager::getRecommendMsgAbstractMange(std::string contactlistid,char* outbuf,int &outlen)
{
	m_pclientSession->getRecommendMsgAbstractWork(contactlistid,outbuf,outlen);

	return 0;
}

int MeetingHostAgentManager::getRecommendMsgMange(std::string contactlistid,char* outbuf)
{
	 m_pclientSession->getRecommendMsgWork(contactlistid,outbuf);

	 return 0;
}

int MeetingHostAgentManager::deleteRecommendMsgMange(std::string contactlistid)
{
	 m_pclientSession->deleteRecommendMsgWork(contactlistid);

	 return 0;
}

int MeetingHostAgentManager::deleteRecommendMsgMangeRecord(std::string contactlistid)
{
	m_pclientSession->deleteRecommendMsgWorkRecord(contactlistid);

	return 0;
}

void  MeetingHostAgentManager::Stop()
{
	if(1 == m_start)
	{
		m_manageSession->uninit();

		AsynModel::AsynResultRet ret = PostMsg(m_pclientSession->GetSessionID(),MEETINGHOSTAGENT_STOP,NULL,0);
		if (ret != AsynModel::SUCCESS)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][MeetingHostAgentManager::Stop]向工作Session发送停止命令失败,ret:%d",ret);
		}

		//销毁新增处理命令的线程
		PostCancelSessionEvent(m_manageSession->GetSessionID());
		m_manageSessionLock.lock();
		m_manageSession = NULL;
		m_manageSessionLock.unlock();

		//m_timer.Disable();
		m_recvCtrl.Stop();

		m_pclientSession = NULL;
		m_localip.clear();
		m_start      = 0;	
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent][MeetingHostAgentManager::Stop]启动时,调用失败");
	}

	return;
}

void MeetingHostAgentManager::UDPDefaultPortFailureCallBack(char errorIP[16],unsigned short errorPort,AsynModel::AsynResultRet errorType)
{
	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][UDPDefaultPortFailureCallBack]默认端口失效,errorIP:%s errorPort:%u",errorIP,errorPort);
	PostMsg(m_manageSid,MEETINGMANAGE_SEESION_RESETUDP,NULL,0);
	//m_timer.SetTimer(3000,this,&MeetingHostAgentManager::StatAddUdpTimerCallbackProc,NULL);
}

void MeetingHostAgentManager::TCPDefaultListenPortFailureCallBack(char errorIP[16],unsigned short errorPort,AsynModel::AsynResultRet errorType)
{
	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][TCPDefaultPortFailureCallBack]默认端口失效,errorIP:%s errorPort:%u",errorIP,errorPort);
	PostMsg(m_manageSid,MEETINGMANAGE_SEESION_RESETTCP,NULL,0);
	//m_timer.SetTimer(3000,this,&MeetingHostAgentManager::StatAddTcpTimerCallbackProc,NULL);
}

#ifdef X64
int MeetingHostAgentManager::StatAddUdpTimerCallbackProc(GMCustomTimerCallBackType type,void* prama)
#else
int MeetingHostAgentManager::StatAddUdpTimerCallbackProc(GMCustomTimerCallBackType type,unsigned int prama)
#endif
{
	return 0;

	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddUdpTimerCallbackProc]设置默认UDP端口回调函数启动");

	//unsigned short udpUsePort = 0;
	//if(!SetUdpPort_Uniform(m_localip,udpUsePort))
	//{
	//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddUdpTimerCallbackProc]]默认端口失效,重新设置失败,ip:%s,port:%u",m_localip.c_str(),udpUsePort);
	//	m_timer.SetTimer(3000,this,&MeetingHostAgentManager::StatAddUdpTimerCallbackProc,NULL);
	//}

	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddUdpTimerCallbackProc]默认端口失效,重新设置成功,ip:%s,port:%u",m_localip.c_str(),udpUsePort);
	//return 0;
}

#ifdef X64
int MeetingHostAgentManager::StatAddTcpTimerCallbackProc(GMCustomTimerCallBackType type,void* prama)
#else
int MeetingHostAgentManager::StatAddTcpTimerCallbackProc(GMCustomTimerCallBackType type,unsigned int prama)
#endif
{
	return 0;
	
	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddTcpTimerCallbackProc]设置默认TCP端口回调函数启动");

	//unsigned short tcpUsePort = 0;
	//if(!SetTcpPort_Uniform(m_localip,tcpUsePort))
	//{
	//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddTcpTimerCallbackProc]]默认端口失效,重新设置失败,ip:%s,port:%u",m_localip.c_str(),tcpUsePort);
	//	m_timer.SetTimer(3000,this,&MeetingHostAgentManager::StatAddTcpTimerCallbackProc,NULL);
	//}

	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][StatAddTcpTimerCallbackProc]默认端口失效,重新设置成功,ip:%s,port:%u",m_localip.c_str(),tcpUsePort);
	//
	//return 0;
}


//rcAddr检查
#if 1
int splitString(const string &srcStr, const string &splitStr, vector<string> &destVec){
	if(srcStr.size() == 0){   
		return 0;
	}   
	size_t oldPos, newPos;
	oldPos = 0;
	newPos = 0;
	string tempData;
	while(1){   
		newPos = srcStr.find(splitStr,oldPos);
		if(newPos != string::npos){   
			tempData = srcStr.substr(oldPos,newPos-oldPos);
			destVec.push_back(tempData);
			oldPos = newPos + splitStr.size();
		}   
		else if(oldPos <= srcStr.size()){   
			tempData = srcStr.substr(oldPos);
			destVec.push_back(tempData);
			break;
		}   
		else{   
			break;
		}   
	}   
	return 0;
}
bool isValidIP(string ip){
	string delim = ".";
	string ret[4];

	string::size_type loc = 0,start = 0;
	for(int i = 0;i < 4; i++){
		loc = ip.find(delim, start);
		if(loc != string::npos){
			ret[i] = ip.substr(start, loc - start);
			start = loc + 1;
		}else if(i == 3){
			ret[i] = ip.substr(start);
		}else{
			return false;
		}
	}
	
	if(ip.substr(start) != ret[3])
		return false;
	for(int i = 0;i < 4;i++){
		int num = atoi(ret[i].c_str());
		if(num > 255){
			return false;
		}else if((num == 0) && (ret[i].compare("0"))){
			return false;
		}
	}

	return true;
}

bool checkIsRightType(const string &srcStr)
{
	vector<string> ipAndPortString;
	splitString(srcStr, "|", ipAndPortString);

	if(ipAndPortString.size() != 2)
		return false;

	vector<string>::iterator iter;
	for(iter = ipAndPortString.begin(); iter != ipAndPortString.end(); ++iter){
		vector<string>ipOrPortString;
		splitString(*iter, ":", ipOrPortString);

		if (ipOrPortString.size() !=2 ){
			return false;
		}

		if(!isValidIP(ipOrPortString[0]))
			return false;
		int len = ipOrPortString[1].length();
		string tmp = ipOrPortString[1];
		for(int i = 0; i < len; ++i)
			if(tmp[i] < '0' || tmp[i] > '9')
				return false;
	}
	return true;
}

#endif

bool  MeetingHostAgentManager::SetUdpPort(std::string localIp,unsigned short & usePort)
{
	unsigned short tempPort = FREE_PORT_START;
	for(;tempPort < FREE_PORT_END; ++tempPort)
  {
#ifdef __IOS__
    unsigned long ip = 0;
    U16 port = 0;
    int ret = GetDefaultLocalUdpAddr(ip,port);
    if(ret == 0 && port > 0)
    {
      usePort = port;
      return true;
    }
    
    if( SetDefaultLocalUdpAddr((char*)localIp.c_str(),tempPort,false) == AsynModel::SUCCESS)

#else
      if( SetDefaultLocalUdpAddr((char*)localIp.c_str(),tempPort) == AsynModel::SUCCESS)
#endif
		{
			usePort = tempPort;
			return true;
		}	
		Sleep(100);
	}

	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认UDP IP和端口,从%d尝试到%d IP:%s 没有找到空闲端口",FREE_PORT_START,FREE_PORT_END,localIp.c_str());
	return false;
}

bool  MeetingHostAgentManager::SetTcpPort(std::string localIp,unsigned short & usePort)
{
	unsigned short tempPort = FREE_PORT_START;
	for(;tempPort < FREE_PORT_END;++tempPort)
	{
#ifdef __IOS__
    unsigned long ip = 0;
    U16 port = 0;
    int ret = GetDefaultLocalTcpAddr(ip,port);
    if(ret == 0 && port > 0)
    {
      usePort = port;
      return true;
    }
    
    if( SetDefaultLocalTcpAddr((char*)localIp.c_str(),tempPort,false) == AsynModel::SUCCESS)
      
#else
      if( SetDefaultLocalTcpAddr((char*)localIp.c_str(),tempPort) == AsynModel::SUCCESS)
#endif
		{
			usePort = tempPort;
			return true;
		}
		Sleep(100);
	}

	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认TCP IP和端口,从%d尝试到%d IP:%s 没有找到空闲端口",FREE_PORT_START,FREE_PORT_END,localIp.c_str());
	return false;
}

bool  MeetingHostAgentManager::SetUdpPort_Uniform(std::string localIp,unsigned short & usePort)
{
#ifdef CELL_PHONE_VERSION
	if(!SetUdpPort(localIp,usePort))
#else
	if( SetDefaultLocalUdpAddr((char*)localIp.c_str(),usePort) != AsynModel::SUCCESS)
#endif
	{
		Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认UDP IP和Udp端口失败,IP%s",localIp.c_str());
		return false;
	}	

	Log::writeWarning(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认UDP IP和Udp端口成功:IP:%s,port:%u",localIp.c_str(),usePort);
	return true;
}

bool  MeetingHostAgentManager::SetTcpPort_Uniform(std::string localIp,unsigned short & usePort)
{
#ifdef CELL_PHONE_VERSION
	if(!SetTcpPort(localIp,usePort))
#else
	if( SetDefaultLocalTcpAddr((char*)localIp.c_str(),usePort) != AsynModel::SUCCESS)
#endif
	{
		Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认TCP IP和端口失败,IP:%s",localIp.c_str());
		return false;
	}	

	Log::writeWarning(MCTRLAGENT_LOG, 6000, "[host agent]异步模型设置默认TCP IP和端口成功,IP:%s,port:%u",localIp.c_str(),usePort);

	return true;
}

bool MeetingHostAgentManager::StartRecvCtrl(std::string localIP,unsigned short & localPort,std::string targetIP,unsigned short targetPort)
{
#ifdef CELL_PHONE_VERSION
	unsigned short tempPort = FREE_PORT_START;
	for(;tempPort < FREE_PORT_END; ++tempPort)
	{
		if(m_recvCtrl.Start((char*)localIP.c_str(),tempPort,(char*)targetIP.c_str(),targetPort))
		{
			Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][start]监听UI命令线程启动成功,localIP:%s,localPort:%u",localIP.c_str(),tempPort);
			localPort = tempPort;
			return true;
		}
	}
	localPort = 0;
#else
	if(m_recvCtrl.Start((char*)localIP.c_str(),localPort,(char*)targetIP.c_str(),targetPort))
	{
		Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][start]监听UI命令线程启动成功,localIP:%s,localPort:%u",localIP.c_str(),localPort);
		return true;
	}
#endif

	Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][start]监听UI命令线程启动失败,localIP:%s,localPort:%u",localIP.c_str(),localPort);

	return false;
}

void MeetingHostAgentManager::resetUdpLocalipandPort(std::string localIp,int port,int &udpUsePort)
{
	//UDP
	int tmpcount = 0;
	int tempPort = port + 50;
	while( SetDefaultLocalUdpAddr((char*)localIp.c_str(),tempPort) != AsynModel::SUCCESS && tmpcount < 1000)
	{
		tmpcount++;
		tempPort += 5;
		continue;
	}
	if (tmpcount >= 1000)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent]重新绑定本地UDP端口,尝试1000次仍失败");
	}

	udpUsePort = tempPort;

	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent]重新绑定本地UDP端口成功,IP:%s,Port:%d",localIp.c_str(),tempPort);

	return;
}

//TCP
void MeetingHostAgentManager::resetTcpLocalipandPort(std::string localIp,int port,int &tcpUsePort)
{
	int tmpcount1 = 0;
	int tempPort1 = port + 50;
	while( SetDefaultLocalTcpAddr((char*)localIp.c_str(),tempPort1) != AsynModel::SUCCESS && tmpcount1 < 1000)
	{
		tmpcount1++;
		tempPort1 += 5;
		continue;
	}
	if (tmpcount1 >= 1000)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent]重新绑定本地TCP端口,尝试1000次仍失败");
	}

	tcpUsePort = tempPort1;

	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent]重新绑定本地TCP端口成功,IP:%s,Port:%d",localIp.c_str(),tempPort1);

	return;
}

int   MeetingHostAgentManager::sendDataByUdpManage(U32 localssid,AsynModel::ConnID& connID,U32 destSid,U32 msgid, char* msgData,U32 dataLen)
{
	int ret = 0;
#ifdef USE_RAW_SOCKET
	m_manageSessionLock.lock();

	if (NULL == m_manageSession)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][sendDataByUdp]m_manageSession为NULL");
		m_manageSessionLock.unlock();
		return -1;
	}
	ret = m_manageSession->sendDataByUdp(localssid,connID,destSid,msgid,msgData,dataLen);
	if (ret != 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][sendDataByUdp]发送失败");
	}

	m_manageSessionLock.unlock();
#endif


	return ret;
}

bool MeetingHostAgentManager::GetServerAddrFromUrl( const std::string & routeUrl, std::string & meetingCtrlServerIp, unsigned short & meetingCtrlServerPort )
{
	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"进入 MeetingCtrlAgentManager::GetServerAddrFromUrl,routeUrl:%s",routeUrl.c_str());
	int pos = routeUrl.find(":");
	if(pos == -1)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"在 routeUrl:%s 里面，找不到 分隔符：---GetServerAddrFromUrl",routeUrl.c_str());
		return false;
	}

	meetingCtrlServerPort = atoi(routeUrl.substr(pos+1,routeUrl.length() - pos-1).c_str());
	meetingCtrlServerIp   = routeUrl.substr(0,pos);

	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"GetServerAddrFromUrl 成功,routeUrl:%s,解析出IP：%s,Port:%u",
		routeUrl.c_str(),meetingCtrlServerIp.c_str(),meetingCtrlServerPort);

	return true;
}

void MeetingHostAgentManager::manager_UpdateIPPortMap( std::map<std::string,std::string> IPPortmap )
{
	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"IPPortmap.size:%d ",IPPortmap.size());
	std::map<std::string,std::string>::iterator iter = IPPortmap.begin();
	for(; iter!= IPPortmap.end(); ++iter)
	{
		Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[%s,%s]",iter->first.c_str(),iter->second.c_str());
		std::string ip;
		unsigned short port;
		bool bSucc = GetServerAddrFromUrl(iter->first,ip,port);
		if(!bSucc)
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"ipport：%s 解析失败",iter->first.c_str());
			return ;
		}

		serverInfo info,info_out;
		info.ip   = ip;
		info.port = port;

		bSucc = GetServerAddrFromUrl(iter->second,ip,port);
		if(!bSucc)
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"ipport：%s 解析失败",iter->second.c_str());
			return ;
		}

		info_out.ip   = ip;
		info_out.port = port;

		g_ConfigParser.SetMappedStpIp(info,info_out);

	}
}

int MeetingHostAgentManagerSession::init()
{
	//m_recvThreadRun = 1;
	m_recvActive = true;
	if( !m_recvThread.run())
	{
		//m_recvThreadRun = 0;
		m_recvActive    = false;
		return -9;
	}

	return 0;
}

int MeetingHostAgentManagerSession::uninit()
{
	//if(1 == m_recvThreadRun)
	//{
	//	m_recvThreadRun = 0;
	//	m_recvSocket.close();
	//	while(2 != m_recvThreadRun)
	//	{
	//		//等待
	//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]等待网络命令接收线程退出");
	//		Sleep(100);
	//	}
	//}

	//等待结束
	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]等待网络命令接收线程退出结束");

	m_recvActive = false;
	m_recvSocket.close();

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]等待网络命令接收线程退出,开始");
	if (m_recvThread.waitThreadExit(INFINITE))
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]网络命令接收线程,退出成功");
		return 0;
	}
	else
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]网络命令接收线程,退出失败");
		return -1;
	}
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent][线程]等待网络命令接收线程退出,成功");
}

int MeetingHostAgentManagerSession::createRecvSocket(unsigned short startPort,unsigned short endPort)
{
	unsigned short i;
	unsigned short tstart = startPort;
	unsigned short tend   = endPort;

	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]创建socket,尝试端口区间为[%d:%d]",tstart,tend);
	for(i = tstart;i<= tend;i++)
	{
		try
		{
			m_recvSocket.close();
			if(false == m_recvSocket.createSocket())
			{
				Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]创建socket,失败");
				return -4;
			}

			m_recvSocket.setSockBuffSize(64*1024,2*1024*1024);

			if(true == m_recvSocket.bind(i,(char*)m_localIp.c_str()))
			{
				break;
			}
		}
		catch (GMGeneralException& e)
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]创建socket,接收socket异常抛出:%s",e.what());
		}
	}

	if(i != tend)
	{
		//成功
		m_bindSuccPort = i;
		Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]创建socket,成功,IP:%s,Port:%d",m_localIp.c_str(),m_bindSuccPort);
	}
	else
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]创建socket,失败,尝试端口区间为[%d:%d]",tstart,tend);
		m_recvSocket.close();
		return -1;
	}

	return 0;
}

void MeetingHostAgentManagerSession::recvWorkThreadFun(void *p)
{
	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][线程]网络命令接收线程启动");
	//接收空间清理
	memset(m_recvBuff,0,sizeof(m_recvBuff));
	
	int  recvLen = 0;
	sockaddr_in sin;
	socklen_t addrLen = sizeof(sin);
	memset( &sin, 0, addrLen);

	//while(1 == m_recvThreadRun)
	while(m_recvActive)
	{
		/*if(1 != m_recvThreadRun)
		{
			break;
		}*/

		if (false == m_recvActive)
		{
			break;
		}

		recvLen = recvfrom(m_recvSocket.m_nSock,m_recvBuff,sizeof(m_recvBuff),0,(sockaddr *)&sin, &addrLen);
		if(recvLen <= 0)
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent]接收线程,接收数据异常,需要重建socket,recvLen=%d,errcode=%d",recvLen,GMWSAGetLastError());
			
			//抛出重建命令
			PostMsg(GetSessionID(),MEETINGMANAGE_SEESION_RECREATE,NULL,0);
			Sleep(1000);
			continue;
		}
		if(recvLen == sizeof(m_recvBuff))
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]接收线程,接收数据异常,长度%d.",recvLen);
			continue;
		}

		//无条件向本地MangeSeesion发送接收到的消息
		PostMsg(GetSessionID(),MEETINGMANAGE_SEESION_RECVCMD_PROC,m_recvBuff,recvLen);
	}

	//m_recvThreadRun = 2;
	Log::writeWarning(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][线程]网络命令接收线程退出");
}

int   MeetingHostAgentManagerSession::sendDataByUdp(U32 localssid,AsynModel::ConnID& connID,U32 destSid,U32 msgid, char* msgData,U32 dataLen)
{
	char buf[1500] = {0};
	char *pdata = buf + sizeof(asynPro);

	int ret = 0;
	if (NULL == msgData)
	{
		std::string tmpIp1 = connID.openhost.openUDPIP;
		ret = sendDataToAsynModel(localssid,m_recvSocket,tmpIp1,connID.openhost.openUdpPort,destSid,msgid,pdata,0);
	}
	else
	{
		if (dataLen > 1500 - sizeof(asynPro))
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][sendDataByUdp]发送消息,消息长度过大,大小为:%d",dataLen);
			return -1;
		}
		memcpy(pdata,msgData,dataLen);
		std::string tmpIp2 = connID.openhost.openUDPIP;
		ret = sendDataToAsynModel(localssid,m_recvSocket,tmpIp2,connID.openhost.openUdpPort,destSid,msgid,pdata,dataLen);
	}

	if (ret != 0)
	{
		//重新创建SOCKET,发消息
		PostMsg(GetSessionID(),MEETINGMANAGE_SEESION_RECREATE,NULL,0);
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][sendDataByUdp]发送消息,失败,向本地seesion发送重建信息");
	}
	else
	{
		Log::writeMessage(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][sendDataByUdp]发送消息,成功,消息大小为:%d",dataLen);
	}
	return ret;
}

int   MeetingHostAgentManagerSession::sendDataToAsynModel(U32 localssid,GMUdpSocket &sock,string &dstip,int dstPort,int dstSessionId,int msgId,char *pdata,int len)
{
	asynPro *paynHead = (asynPro *)(pdata - sizeof(asynPro));
	paynHead->init();

	paynHead->length  = sizeof(asynPro) + len;
	paynHead->destsid = dstSessionId;
	paynHead->msgid   = msgId;
	paynHead->srcsid  = localssid;

	try
	{
		int ret = sock.sendDgramTo((unsigned char*)paynHead,paynHead->length,(char*)dstip.c_str(),dstPort);
		Log::writeMessage(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][sendDataToAsynModel]发送信息:命令ID:%d,长度:%d,Sid:%d,ret:%d",msgId,paynHead->length,paynHead->srcsid,ret);
		return 0;
	}
	catch (GMGeneralException& e)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][sendDataToAsynModel]发送信息,抛出异常,命令ID:%d,Sid:%d,错误描述:%s",msgId,localssid,e.what());
	}
	return -1;
}

int MeetingHostAgentManagerSession::onReCreate(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(0 != m_creatSocketTime)
	{
		unsigned int timediff = GMGetTickCount() - m_creatSocketTime;
		if (timediff < 1000)
		{
			Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][MANAGESEESSION]离上次重建时间现在不到1s,不进行重建");
			return -1;
		}
	}
	
	m_creatSocketTime = GMGetTickCount();
	//每次尝试10个端口
	if(m_bindSuccPort < FREE_PORT_START)
	{
		m_bindSuccPort = FREE_PORT_START;
	}
	unsigned short startPort = m_bindSuccPort + 1;
	if(startPort >= FREE_PORT_END)
	{
		startPort = FREE_PORT_START;
	}
	unsigned short endPort  = startPort + 10;
	if (endPort > FREE_PORT_END)
	{
		endPort = FREE_PORT_END;
	}

	//每次尝试从新的PORT开始
	m_bindSuccPort = endPort;

	createRecvSocket(startPort,endPort);

	return 0;
}

int MeetingHostAgentManagerSession::onRecvCmdMsg(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meeting_syncHead localHead;
	meeting_syncHead* pasynHead = &localHead;
	
	int asynLen = asynHeadToLocal(data,&localHead);
	int  tmplen = len - asynLen;
	char buf[1500] = {0};
	if (tmplen + sizeof(meeting_syncHead_ext) > 1500)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][接收消息]接收消息,接收到长度过大,len:%d",len);
		return -1;
	}

	//去掉异步模型头,复制除扩展信息以外的信息
	memcpy(buf,(char*)data + asynLen,tmplen);
	//复制扩展信息
	meeting_syncHead_ext tmp_ext;
	tmp_ext.ssrcid = pasynHead->srcsid;
	memcpy(buf + tmplen,&tmp_ext,sizeof(tmp_ext));

	int ret = PostMsg(pasynHead->destsid,pasynHead->msgid,buf,tmplen + sizeof(tmp_ext));
	if (ret != AsynModel::SUCCESS)
	{
		Log::writeError(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][接收消息]接收消息,向Seesion:%d,发送命令为:%d,失败",pasynHead->destsid,pasynHead->msgid);
	}
	else
	{
		Log::writeMessage(MCTRLAGENT_LOG,LOG_TYPE_ID,"[Host agent][接收消息]接收消息,向Seesion:%d,发送命令为:%d,成功",pasynHead->destsid,pasynHead->msgid);
	}

	return 0;
}

int MeetingHostAgentManagerSession::onResetUdpCmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	if(false == m_resetUdpTime)
	{
		m_resetUdpTime = true;
		SetTimer(MEETINGMANAGE_SEESION_RESETUDP_TIMER,10);
	}
	return 0;
}

int MeetingHostAgentManagerSession::OnSetUdpTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	m_resetUdpTime = false;

	//每次尝试10个端口
	if (m_curUdpPort < FREE_PORT_START)
	{
		m_curUdpPort = FREE_PORT_START;
	}

	unsigned short startPort = m_curUdpPort + 1;
	if(startPort >= FREE_PORT_END)
	{
		startPort = FREE_PORT_START;
	}
	unsigned short tempEnd  = startPort + 10;
	if (tempEnd > FREE_PORT_END)
	{
		tempEnd = FREE_PORT_END;
	}

	//下一轮设置端口从end开始
	m_curUdpPort = tempEnd;

	unsigned short tempPort = startPort;
	int ret = 0;
	for(;tempPort <= tempEnd;++tempPort)
	{
#ifdef __IOS__
		unsigned long ip = 0;
		U16 port = 0;
		int ret = GetDefaultLocalUdpAddr(ip,port);
		if(ret == 0 && port > 0)
		{
			m_curUdpPort = port;
			Log::writeWarning(MCTRLAGENT_LOG, 6000, "[host agent]GetDefaultLocalUdpAddr成功,ip:%d,port:%d",ip,port);
			return 0;
		}

		ret = SetDefaultLocalUdpAddr((char*)m_localIp.c_str(),tempPort,false);

		if( /*SetDefaultLocalUdpAddr((char*)m_localIp.c_str(),tempPort,false) ==*/ AsynModel::SUCCESS == ret)
#else
		ret = SetDefaultLocalUdpAddr((char*)m_localIp.c_str(),tempPort);
		if( /*SetDefaultLocalUdpAddr((char*)m_localIp.c_str(),tempPort) ==*/ AsynModel::SUCCESS == ret)
#endif
		{
			Log::writeWarning(MCTRLAGENT_LOG, 6000, "[host agent]重新设置默认UDP,成功,ip:%s,port:%d",m_localIp.c_str(),tempPort);
			m_curUdpPort = tempPort;
			return 0;
		}
		else
		{
			Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]重新设置默认UDP,失败,ip:%s,port:%d,ret:%d",m_localIp.c_str(),tempPort,ret);
		}
	}

	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]重新设置默认UDP,失败,从%d尝试到%d IP:%s 没有找到空闲端口",startPort,tempEnd,m_localIp.c_str());

	m_resetUdpTime = true;
	SetTimer(MEETINGMANAGE_SEESION_RESETUDP_TIMER,2000);
	return 0;
}

int MeetingHostAgentManagerSession::onResetTcpCmd(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	//客户端TCP发送,无需重复绑定
	if(false == m_resetTcpTime)
	{
		m_resetTcpTime = true;
		SetTimer(MEETINGMANAGE_SEESION_RESETTCP_TIMER,10);
	}
	return 0;
}

int MeetingHostAgentManagerSession::OnSetTcpTimer(GMCustomTimerCallBackType onTimerType, AsynModel::UserDataInfo* pUserTimeDataInfo)
{
	m_resetTcpTime = false;

	if (m_curTcpPort < FREE_PORT_START)
	{
		m_curTcpPort = FREE_PORT_START;
	}

	//每次尝试10个端口
	unsigned short tempPort = m_curTcpPort + 1;
	if(tempPort >= FREE_PORT_END)
	{
		tempPort = FREE_PORT_START;
	}
	unsigned short tempEnd  = tempPort + 10;
	if (tempEnd > FREE_PORT_END)
	{
		tempEnd = FREE_PORT_END;
	}

	//下一轮设置端口从end开始
	m_curTcpPort = tempEnd;

	for(;tempPort <= tempEnd;++tempPort)
	{
#ifdef __IOS__
		unsigned long ip = 0;
		U16 port = 0;
		int ret = GetDefaultLocalTcpAddr(ip,port);
		if(ret == 0 && port > 0)
		{
			m_curTcpPort = port;
			return 0;
		}

		if( SetDefaultLocalTcpAddr((char*)m_localIp.c_str(),tempPort,false) == AsynModel::SUCCESS)
#else
		if( SetDefaultLocalTcpAddr((char*)m_localIp.c_str(),tempPort) == AsynModel::SUCCESS)
#endif
		{
			Log::writeWarning(MCTRLAGENT_LOG, 6000, "[host agent]重新设置默认TCP,成功,ip:%s,port:%d",m_localIp.c_str(),tempPort);
			m_curTcpPort = tempPort;
			return 0;
		}
	}

	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]重新设置默认TCP,失败,从%d尝试到%d IP:%s 没有找到空闲端口",m_curTcpPort,tempEnd,m_localIp.c_str());

	m_resetTcpTime = true;
	SetTimer(MEETINGMANAGE_SEESION_RESETTCP_TIMER,2000);
	return 0;
}

MeetingHostAgentManagerSession::~MeetingHostAgentManagerSession()
{
	Log::writeError(MCTRLAGENT_LOG, 6000, "[host agent]MeetingHostAgentManagerSession析构");
}

int getStpRCUrl(std::string routeUrl,std::string & ip)
{
	int pos = routeUrl.find("|");
	if(pos < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]找不到关键字 |,routeUrl:%s,pos:%d",routeUrl.c_str(),pos);
		return -1;
	}

	std::string stpRC1 = routeUrl.substr(0,pos);
	int pos1 = stpRC1.find(":");
	if(pos1 < 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]在routeUrl:%s 里面找不到分隔符",stpRC1.c_str());
		return -1;
	}
	ip = stpRC1.substr(0,pos1);

	return 0;
}

int	MeetingHostAgentManager::manager_SendTo(std::string destAccountId,const char* buf,const int len,const long long userData)
{
	if (m_pclientSession == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::manager_SendTo] return -1,m_pclientSession == NULL");
		return -1;
	}
	meetingHost_trans_data datactx;
	if (len > sizeof(datactx.buf))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::manager_SendTo] return -2,send buf too big:%d",len);
		return -2;
	}
	strncpy(datactx.from_userid,m_pclientSession->m_userid.c_str(),sizeof(datactx.from_userid) - 1);
	strncpy(datactx.to_userid,destAccountId.c_str(),sizeof(datactx.to_userid) - 1);
	memcpy(datactx.buf,buf,len);
	datactx.trans_data_uid = userData;
	datactx.dataLen = len;
	char tempbuf[2048] = { 0 };
	int nserLen = datactx.Seralize(tempbuf,sizeof(tempbuf));
	
	AsynModel::AsynResultRet ret3 = PostMsg(m_pclientSession->GetSessionID(),MEETINGHOST_UI_TRANSDATA,tempbuf,nserLen);
	if(0 != ret3)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::manager_SendTo]接口被调用,向工作SESSION发送转发数据命令失败! err=%d",ret3);
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000,"[host agent][MeetingHostAgentManager::Start]接口被调用,向工作SESSION发送转发数据命令成功!");
	}
	return ret3;
}

int	MeetingHostAgentManager::manager_SetTransDataCB(OnRecvTransDataCB cbFunc)
{
	m_transCBLock.lock();
	m_commonTransDataCB = cbFunc;
	m_transCBLock.unlock();
	return 0;
}

int	MeetingHostAgentManager::manager_SetTransDataFromUserCB(const char* userId,OnRecvTransDataCB cbFunc)
{
	//if (NULL == userId)
	//{
	//	return -1;
	//}
	//if (cbFunc)
	//{
	//	m_transCBLock.lock();
	//	m_userTransDataCB[userId] = cbFunc;
	//	m_transCBLock.unlock();
	//}
	//else
	//{
	//	m_transCBLock.lock();
	//	if (m_userTransDataCB.find(userId) != m_userTransDataCB.end())
	//	{
	//		m_userTransDataCB.erase(userId);
	//	}
	//	m_transCBLock.unlock();
	//}
	return 0;
}

void MeetingHostAgentManager::OnRecvTransData(const char* fromUserId,const char* buf,const unsigned short len)
{
	if (NULL == fromUserId || NULL == buf)
	{
		return;
	}
	OnRecvTransDataCB cbFunc = NULL;
	m_transCBLock.lock();
	if (m_userTransDataCB.find(fromUserId) != m_userTransDataCB.end())
	{
		cbFunc = m_userTransDataCB[fromUserId];
	}
	else
	{
		cbFunc = m_commonTransDataCB;
	}
	m_transCBLock.unlock();

	if (cbFunc)
	{
		cbFunc(fromUserId,buf,len);
	}
}

int MeetingHostAgentManager::manager_SetTransDataResultCB(OnTransDataResultCB cbFunc)
{
	m_transCBLock.lock();
	m_transResultCB = cbFunc;
	m_transCBLock.unlock();
	return 0;
}

void MeetingHostAgentManager::OnTransDataResult(const long long userData,const int result)
{
	m_transCBLock.lock();
	OnTransDataResultCB cbFunc = m_transResultCB;
	m_transCBLock.unlock();

	if (cbFunc)
	{
		cbFunc(userData,result);
	}
}
