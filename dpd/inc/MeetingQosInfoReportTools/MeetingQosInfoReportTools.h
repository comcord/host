#ifndef __ROUTER_CLIENT_H_20150731_
#define __ROUTER_CLIENT_H_20150731_


#ifdef WIN32
#ifndef DLLEXPORT
#define RCC_DLL_EXPORT __declspec(dllimport)
	#pragma comment( lib, "MeetingQosInfoReportTools" )
#else
#define RCC_DLL_EXPORT __declspec(dllexport)
#endif//DLLIMPORT
#else
#define RCC_DLL_EXPORT
#endif

extern "C" 
{
	
	/**
	* 描述 初始化RouterClient，同步
	*
	* @param szId						[IN] 路由节点Id
	* @param nType						[IN] 路由节点类型,0:普通用户，1:Relay
	* @param szLocalUdpIp				[IN] 本地UDP地址
	* @param usLocalUdpPort				[IN] 本地UDP端口
	* @param szMainRouterCenterIp		[IN] 主Router Center地址
	* @param usMainRouterCenterPort		[IN] 主Router Center端口
	* @param szBackupRouterCenterIp		[IN] 备Router Center地址
	* @param usBackupRouterCenterPort	[IN] 备Router Center端口
	* @param pReporter					[IN] 事件上报者
	* @return 成功返回0,失败返回正数错误码
	*/
	RCC_DLL_EXPORT int InitMeetingQosInfoReportTools(
		char szId[64],
		int nType,
		const char szLocalUdpIp[16],
		unsigned short usLocalUdpPortbegin,
		const char ipbak[16],
		unsigned short usLocalUdpPortend,
		const char stprcIp[16],
		unsigned short stprcPort);
	
	RCC_DLL_EXPORT int ReportMeetingQosInfo(const char* pCallInfo,int len);

	//RCC_DLL_EXPORT int ReportCallInfo(const char* pCallInfo);
	RCC_DLL_EXPORT void SetMeetingQosInfoReportToolsDir(const char* dir);
} //extern "C" 

#endif //#ifndef __ROUTER_CLIENT_H__
