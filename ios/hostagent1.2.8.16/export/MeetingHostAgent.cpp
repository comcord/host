#include "stdafx.h"
#include "MeetingHostAgentManager.h"
#include "MeetingHostAgent.h"
#include "configParser.h"

#ifdef _ANDROID_NDK_
#include <android/log.h>
#define __AND_LOG_TAG "meetingHost"
#define __AND_LOGE(...) __android_log_print(ANDROID_LOG_ERROR   , __AND_LOG_TAG, __VA_ARGS__)
#else
#define __AND_LOGE(...)
#endif

//#define TEST

std::string g_recMsglocalpath;	       //�����Ƽ���Ϣ�洢·�� 
std::string g_recRecordlocalpath;      //�����Ƽ���ʷ��¼��Ϣ�洢·�� 
std::string g_inviteRecordlocalpath;   //���������¼��洢·�� 
std::string g_shareScreenConfigPath;   //��Ļ����Agent�����˿ڴ洢·�� 

static int g_bInit = 0;
static int g_bStart  = 0;
MeetingHostAgentManager *g_pmanage = NULL;

CConfigParser_STPIPMAP g_ConfigParser;

extern "C"
{

RCC_DLL_EXPORT int  MHClient_Start(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
	std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir)
{
#if 1
	if(NULL != g_pmanage && 1 == g_bStart)
	{
		g_bStart = 0;
		g_pmanage->Stop();

		//����ֹͣ�첽ģ�ͽӿں���
		AsynModel::AsynResultRet r1 = StopAsynModel();
		if( r1 != AsynModel::SUCCESS)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Start]ֹͣ�첽ģ��ʧ��,ret:%d",r1);
		}
		else
		{
			Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Start]ֹͣ�첽ģ�ͳɹ�");
		}
		ReleaseAsynModel();

		g_bInit = 0;
	}
#endif

	int ret;
	if(0 == g_bInit)
	{   
		ret = MeetingHostAgentManager::init(logfile,logOutDir);
		if(0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]������־ʧ��");
			return ret;
		}
		g_bInit = 1;
	}

	if(NULL == g_pmanage)
	{
		g_pmanage = new MeetingHostAgentManager;
		if(NULL == g_pmanage)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]����MeetingHostAgentManagerʧ��");
			return -902;
		}
	}

#ifdef TEST
	g_recMsglocalpath       += "d:/recMsgAgent.xml";
	g_recRecordlocalpath    += "d:/recRecordMsg.xml";
	g_inviteRecordlocalpath += "d:/";
	g_shareScreenConfigPath += "c:/test";
#else
	//�Ƽ���Ϣ�洢·��
	g_recMsglocalpath    = logfile;
	g_recMsglocalpath   += "/recMsgAgent.xml";

	//�Ƽ���ʷ��¼�洢·��
	g_recRecordlocalpath  = logfile;
	g_recRecordlocalpath += "/recRecordMsg.xml";

	//�����¼��洢·��
	g_inviteRecordlocalpath = logOutDir;

	//��Ļ����Agent�˿ڴ洢·��
	g_shareScreenConfigPath = logfile;
	g_shareScreenConfigPath += "/shareScreenAgentAddr.txt";
#endif

	if(1 == g_bStart)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]�ظ�Start");
		return -903;
	}

	bool bSucc = false;
	unsigned short freePortOut = 0;
	bSucc =  g_pmanage->Start(accounted,localip,localport,localCmdIp,localCmdPort,UICmdIp,UICmdPort,rcAddr,"",freePortOut);
	if(bSucc)
	{
		g_bStart = 1;
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]����Start�ӿ�,���ر��ؼ���UI�˿�:%u",freePortOut);
		return freePortOut;
	}
	else
		return -1;
}

RCC_DLL_EXPORT int  MHClient_Start2(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
	std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir,std::string token)
{
#if 1
	if(NULL != g_pmanage && 1 == g_bStart)
	{
		g_bStart = 0;
		g_pmanage->Stop();

		//����ֹͣ�첽ģ�ͽӿں���
		AsynModel::AsynResultRet r1 = StopAsynModel();
		if( r1 != AsynModel::SUCCESS)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Start2]ֹͣ�첽ģ��ʧ��,ret:%d",r1);
		}
		else
		{
			Log::writeWarning (MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Start2]ֹͣ�첽ģ�ͳɹ�");
		}
		ReleaseAsynModel();

		g_bInit = 0;
	}
#endif
	
	int ret;
	if(0 == g_bInit)
	{   
		ret = MeetingHostAgentManager::init(logfile,logOutDir);
		if(0 != ret)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]������־ʧ��");
			return ret;
		}
		g_bInit = 1;
	}
	if(NULL == g_pmanage)
	{
		g_pmanage = new MeetingHostAgentManager;
		if(NULL == g_pmanage)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]����MeetingHostAgentManagerʧ��");
			return -901;
		}
	}
	if(1 == g_bStart)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]�ظ�Start");
		return -902;
	}

#ifdef TEST
	g_recMsglocalpath    += "d:/recMsg2.xml";
	g_recRecordlocalpath += "d:/recRecordMsg.xml";
#else
	//�Ƽ���Ϣ�洢·��
	g_recMsglocalpath    = logfile;
	g_recMsglocalpath   += "/recMsgAgent.xml";

	//�����¼��洢·��
	g_recRecordlocalpath = logfile;
	g_recRecordlocalpath += "/recRecordMsg.xml";

	//�����¼��洢·��
	g_inviteRecordlocalpath = logOutDir;

	//��Ļ����Agent�˿ڴ洢·��
	g_shareScreenConfigPath = logfile;
	g_shareScreenConfigPath += "/shareScreenAgentAddr.txt";
#endif

	bool bSucc = false;
	unsigned short freePortOut = 0;
	bSucc =  g_pmanage->Start2(accounted,localip,localport,localCmdIp,localCmdPort,UICmdIp,UICmdPort,rcAddr,token,freePortOut);
	if(bSucc)
	{

		g_bStart = 1;
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent]Start2��������:%u",freePortOut);

		return freePortOut;
	}
	else
		return -1;
}

RCC_DLL_EXPORT int MHClient_getRecommendMsgAbstract(std::string contactlistid,char * outbuf,int &outlen)
{
	g_pmanage->getRecommendMsgAbstractMange(contactlistid,outbuf,outlen);

	return 0;
}

RCC_DLL_EXPORT int MHClient_getRecommendMsgDetail(std::string contactlistid,char * outbuf)
{
	
	g_pmanage->getRecommendMsgMange(contactlistid,outbuf);

	return 0;
}

RCC_DLL_EXPORT int MHClient_deleteRecommendMsg(std::string contactlistid)
{
	g_pmanage->deleteRecommendMsgMange(contactlistid);

	return 0;
}

RCC_DLL_EXPORT int MHClient_deleteRecommendMsgRecord(std::string contactlistid)
{
	g_pmanage->deleteRecommendMsgMangeRecord(contactlistid);

	return 0;
}

RCC_DLL_EXPORT void  MHClient_Stop()
{
	//if(NULL != g_pmanage && 1 == g_bStart)
	//{
		//g_bStart = 0;
		//g_pmanage->Stop();

		//����ֹͣ�첽ģ�ͽӿں���
		//AsynModel::AsynResultRet r1 = StopAsynModel();
		//if( r1 != AsynModel::SUCCESS)
		//{
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Stop]ֹͣ�첽ģ��ʧ��,ret:%d",r1);
		//}
		//else
		//{
		//	Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 6000, "[host agent][Stop]ֹͣ�첽ģ�ͳɹ�");
		//}

		//ReleaseAsynModel();

	//	g_bInit = 0;
	//}
	
	return;
}

RCC_DLL_EXPORT void MHClient_UpdateIPPortMap( std::map<std::string,std::string> IPPortmap )
{
	return g_pmanage->manager_UpdateIPPortMap(IPPortmap);
}

RCC_DLL_EXPORT int MHClient_SendTo(std::string destAccountId,const char* buf,const int len,const long long userData)
{
	return g_pmanage->manager_SendTo(destAccountId,buf,len,userData);
}

RCC_DLL_EXPORT int	MHClient_SetTransDataCB(OnRecvTransDataCB cbFunc)
{
	return g_pmanage->manager_SetTransDataCB(cbFunc);
}

RCC_DLL_EXPORT int	MHClient_SetTransDataFromUserCB(const char* userId,OnRecvTransDataCB cbFunc)
{
	return g_pmanage->manager_SetTransDataFromUserCB(userId,cbFunc);
}

RCC_DLL_EXPORT int MHClient_SetTransDataResultCB(OnTransDataResultCB cbFunc)
{
	return g_pmanage->manager_SetTransDataResultCB(cbFunc);
}
    
}
