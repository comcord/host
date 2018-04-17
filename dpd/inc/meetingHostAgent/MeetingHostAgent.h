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

extern "C"
{
	/*单实例接口*/
	RCC_DLL_EXPORT int	   MHClient_Start(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,int remotePort,std::string logfile,std::string logOutDir);

	RCC_DLL_EXPORT int	   MHClient_Start2(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir,std::string token);

	RCC_DLL_EXPORT int     MHClient_getRecommendMsgAbstract(std::string contactlistid,char * outbuf,int &outlen);

	RCC_DLL_EXPORT int     MHClient_getRecommendMsgDetail(std::string contactlistid,char * outbuf);

	RCC_DLL_EXPORT int     MHClient_deleteRecommendMsg(std::string contactlistid);

	RCC_DLL_EXPORT void	   MHClient_Stop();
};

#endif //#ifndef __MEETING_MACH_MANAGER_EXPORT_H__
