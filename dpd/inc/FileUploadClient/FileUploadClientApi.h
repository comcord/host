#pragma once

#ifdef __cplusplus
#	ifdef WIN32
#		ifdef   FILE_UPLOAD_APIEXPORT
#			define  FILE_UPLOAD_APIEXPORT  extern "C" __declspec(dllexport)
#		else
#			define  FILE_UPLOAD_APIEXPORT  extern "C" __declspec(dllimport)
#			pragma comment(lib,"FileUpLoadAgent.lib")
#		endif
#	else
#		define FILE_UPLOAD_APIEXPORT extern "C"
#	endif
#else
#	define FILE_UPLOAD_APIEXPORT
#endif


FILE_UPLOAD_APIEXPORT int  startJNI(const char *userID, const char * hardwareid,
                                    const char *clientIP,unsigned short clientPort,
                                    const char *srvIP,unsigned short srvtPort,
                                    const char *zipFileUrl,const char *uppathsjson,
                                    const char *configpath,const char *logoutpath);

FILE_UPLOAD_APIEXPORT void stopJNI();

