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
#ifdef __cplusplus
extern "C"
#endif
{
	/*单实例接口*/
	RCC_DLL_EXPORT int		MHClient_Start(std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		                                 std::string UICmdIp,int  UICmdPort,std::string  rcAddr,std::string logfile,std::string logOutDir);
	RCC_DLL_EXPORT void		MHClient_Stop();


	/*多实例接口*/
	RCC_DLL_EXPORT int		MHClient_init();
	RCC_DLL_EXPORT void*	MHClient_create();
	RCC_DLL_EXPORT void		MHClient_destroy(void* manage);
	RCC_DLL_EXPORT int		MHClient_start(void* manage,std::string accounted,std::string localip,int localport,std::string localCmdIp,int  localCmdPort,
		std::string UICmdIp,int  UICmdPort,std::string  rcAddr);
	RCC_DLL_EXPORT void		MHClient_stop(void* manage);
	RCC_DLL_EXPORT int      MHClient_test(std::string UICmdIp,int  UICmdPort,std::string test);
};



#endif //#ifndef __MEETING_MACH_MANAGER_EXPORT_H__
