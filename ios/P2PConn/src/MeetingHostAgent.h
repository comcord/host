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
	/*单实例接口*/
	RCC_DLL_EXPORT int	   MHClient_Start(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir);

	RCC_DLL_EXPORT int	   MHClient_Start2(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir,std::string token);

	/*根据通讯录ID,查询对应推荐信息的简要信息,返回JSON串以及长度*/
	RCC_DLL_EXPORT int     MHClient_getRecommendMsgAbstract(std::string contactlistid,char * outbuf,int &outlen);

	/*根据通讯录ID,获取推荐信息,返回JSON串*/
	RCC_DLL_EXPORT int     MHClient_getRecommendMsgDetail(std::string contactlistid,char * outbuf);

	/*拒绝推荐信息,调用该接口*/
	RCC_DLL_EXPORT int     MHClient_deleteRecommendMsg(std::string contactlistid);

	/*删除已经接受的通讯录,调用该接口*/
	RCC_DLL_EXPORT int     MHClient_deleteRecommendMsgRecord(std::string contactlistid);

	RCC_DLL_EXPORT void	   MHClient_Stop();

	/*
	*	IP映射接口
	*/
	RCC_DLL_EXPORT void    MHClient_UpdateIPPortMap(std::map<std::string,std::string> IPPortmap);

	/*发送透传数据*/
	RCC_DLL_EXPORT int	   MHClient_SendTo(std::string destAccountId,const char* buf,const int len,const long long userData);

	/*设置转发数据回调函数*/
	RCC_DLL_EXPORT int	   MHClient_SetTransDataCB(OnRecvTransDataCB cbFunc);
	//RCC_DLL_EXPORT int	   MHClient_SetTransDataFromUserCB(const char* userId,OnRecvTransDataCB cbFunc);
	RCC_DLL_EXPORT int	   MHClient_SetTransDataResultCB(OnTransDataResultCB cbFunc);

};

#endif //#ifndef __MEETING_MACH_MANAGER_EXPORT_H__
