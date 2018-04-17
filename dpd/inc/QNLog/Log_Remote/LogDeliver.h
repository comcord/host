//LogDeliver.h
#ifndef LOGDELIVER_H
#define LOGDELIVER_H
class CLogImplement;
#include "BaseDataType.h"
#include "T_LogMsgEx.h"
class CRWLock;
#define MAX_PATH 512
#ifdef WIN32

#ifdef LOG_DLL_EXPORT
#define LOG_API __declspec(dllexport)
#else
#define LOG_API __declspec(dllimport)
#pragma comment(lib, "Log_writer")
#endif

#else
#define LOG_API __attribute__ ((visibility("default")))
#endif

enum LOGTYPE
{
	GM_LOG_MESSAGE = 1,
	GM_LOG_DEBUG,
	GM_LOG_WARNING,
	GM_LOG_ERROR,
};
struct LogFilter
{
	unsigned char iSubTypeCount;
	long long i64SumOfMsgSubType[4];
	long long i64SunOfDebugSubType[4];
	long long i64SumOfErrorSubType[4];
	long long i64SumOfWarningSubTye[4];
};

class LOG_API Log
{
public:
		static void  writeError  (long long subType, unsigned long nLogId,const char * f,...);
		static void  writeWarning(long long subType, unsigned long nLogId, const char * f,...);
		static void  writeMessage(long long subType, unsigned long nLogId,const char * f,...);
		static void  writeDebug  (long long subType, unsigned long nLogId,const char * f,...);

		static void  writeErrorEx  (long long subType, unsigned long nLogId, unsigned char subTypeCount,const char * f,...);
		static void  writeWarningEx(long long subType, unsigned long nLogId, unsigned char subTypeCount,const char * f,...);
		static void  writeMessageEx(long long subType, unsigned long nLogId, unsigned char subTypeCount,const char * f,...);
		static void  writeDebugEx  (long long subType, unsigned long nLogId, unsigned char subTypeCount,const char * f,...);

		static bool  isThisSubTypeOpen(long long subType,LOGTYPE logtype, unsigned char subTypeCount = 1);

		static void  setNewLogFilter( LogFilter *pStrLogFilter );
		static void  resumePreLogFilter();

		//static void  open(bool isShow = true,char* pLaunchParameter = NULL,bool isopencrypt= true);
		static void  close();
		
		/*
			logDirForLogWriter logwriter写入本地的日志路径 结尾不要包含\或者/
		*/
		//static void  open(char logDirForLogWriter[256]=NULL,bool isShow = true,char* pLaunchParameter = NULL,bool isopencrypt= true);
		static void  open(bool isShow = true, char* pLaunchParameter = NULL, bool isopencrypt= true, char logConfigFileName[MAX_PATH]=NULL, char logDirForLogWriter[MAX_PATH]=NULL);
};
#endif
