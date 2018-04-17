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
	* ���� ��ʼ��RouterClient��ͬ��
	*
	* @param szId						[IN] ·�ɽڵ�Id
	* @param nType						[IN] ·�ɽڵ�����,0:��ͨ�û���1:Relay
	* @param szLocalUdpIp				[IN] ����UDP��ַ
	* @param usLocalUdpPort				[IN] ����UDP�˿�
	* @param szMainRouterCenterIp		[IN] ��Router Center��ַ
	* @param usMainRouterCenterPort		[IN] ��Router Center�˿�
	* @param szBackupRouterCenterIp		[IN] ��Router Center��ַ
	* @param usBackupRouterCenterPort	[IN] ��Router Center�˿�
	* @param pReporter					[IN] �¼��ϱ���
	* @return �ɹ�����0,ʧ�ܷ�������������
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
