#ifndef __MEETING_HOST_AGENT_EXPORT_H__
#define __MEETING_HOST_AGENT_EXPORT_H__

#ifdef WIN32
#ifndef DLLEXPORT
#define RCC_DLL_EXPORT __declspec(dllimport)
#pragma comment( lib, "MeetingHostAgentDll.lib" )
#else
#define RCC_DLL_EXPORT __declspec(dllexport)
#endif//DLLIMPORT
#else
#define RCC_DLL_EXPORT
#endif

#include <string>

typedef void (*OnRecvTransDataCB)(const char* fromUserId,const char* buf, const unsigned short len); 
typedef void (*OnTransDataResultCB)(const unsigned long long userData,int result);

extern "C"
{
	/*��ʵ���ӿ�*/
	RCC_DLL_EXPORT int	   MHClient_Start(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir);

	RCC_DLL_EXPORT int	   MHClient_Start2(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir,std::string token);

	/*����ͨѶ¼ID,��ѯ��Ӧ�Ƽ���Ϣ�ļ�Ҫ��Ϣ,����JSON���Լ�����*/
	RCC_DLL_EXPORT int     MHClient_getRecommendMsgAbstract(std::string contactlistid,char * outbuf,int &outlen);

	/*����ͨѶ¼ID,��ȡ�Ƽ���Ϣ,����JSON��*/
	RCC_DLL_EXPORT int     MHClient_getRecommendMsgDetail(std::string contactlistid,char * outbuf);

	/*�ܾ��Ƽ���Ϣ,���øýӿ�*/
	RCC_DLL_EXPORT int     MHClient_deleteRecommendMsg(std::string contactlistid);

	/*ɾ���Ѿ����ܵ�ͨѶ¼,���øýӿ�*/
	RCC_DLL_EXPORT int     MHClient_deleteRecommendMsgRecord(std::string contactlistid);

	RCC_DLL_EXPORT void	   MHClient_Stop();

	/*
	*	IPӳ��ӿ�
	*/
	RCC_DLL_EXPORT void    MHClient_UpdateIPPortMap(std::map<std::string,std::string> IPPortmap);

	/*����͸������*/
	RCC_DLL_EXPORT int	   MHClient_SendTo(std::string destAccountId,const char* buf,const int len,const long long userData);

	/*����ת�����ݻص�����*/
	RCC_DLL_EXPORT int	   MHClient_SetTransDataCB(OnRecvTransDataCB cbFunc);
	//RCC_DLL_EXPORT int	   MHClient_SetTransDataFromUserCB(const char* userId,OnRecvTransDataCB cbFunc);
	RCC_DLL_EXPORT int	   MHClient_SetTransDataResultCB(OnTransDataResultCB cbFunc);

};

#endif //#ifndef __MEETING_MACH_MANAGER_EXPORT_H__
