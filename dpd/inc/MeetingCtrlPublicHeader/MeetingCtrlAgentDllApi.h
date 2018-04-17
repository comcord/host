#pragma once

#ifdef __cplusplus
#	ifdef WIN32
#		ifdef   MEETING_CTRL_APIEXPORT
#			define  MEETING_CTRL_APIEXPORT  extern "C" __declspec(dllexport)
#		else
#			define  MEETING_CTRL_APIEXPORT  extern "C" __declspec(dllimport)
#			pragma comment(lib,"MeetingCtrlAgent.lib")
#		endif
#	else
#		define MEETING_CTRL_APIEXPORT extern "C"
#	endif
#else
#	define MEETING_CTRL_APIEXPORT
#endif


MEETING_CTRL_APIEXPORT int  Start(char routeURL[128],char localIP[16],unsigned short localPort,char targetIP[16],unsigned short targetPort,char remoteCommIP[16],unsigned short remoteCommPort);

MEETING_CTRL_APIEXPORT void  Stop();

MEETING_CTRL_APIEXPORT int SetInitParam(const char logcfgpath[128], const char logfileoutpath[128]);

