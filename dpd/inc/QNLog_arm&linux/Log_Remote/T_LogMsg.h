//T_LogMsg.h

#ifndef T_LOGMSG_H
#define T_LOGMSG_H

#include <time.h>
#include <string.h>
#define MAX_BUFF_LEN 1024
class T_LogMsg 
{
public:
	T_LogMsg():nLogType(0),nSubTypeCount(1),nSubLogType(0),nLogId(0),msgtime(0),msgLength(0),bIsopenCrypt(false)
	{
		memset(msg, 0, MAX_BUFF_LEN);
		nThreadId = 0;
	}
	T_LogMsg(const T_LogMsg &log)
	{
		*this = log;
	}

	int nLogType;
	unsigned char nSubTypeCount;//��ʾ�����ڵڼ�����λ��Ĭ��Ϊ1����ʾֻ��һ��64λ�����ࣻ
	long long nSubLogType;
	unsigned long  nLogId;
	unsigned long  nThreadId;
	time_t msgtime;
	unsigned int msgLength;
	char msg[MAX_BUFF_LEN];
	bool bIsopenCrypt;//���ܿ��ز���

	T_LogMsg & operator=(const T_LogMsg& log)
	{
		nLogType    = log.nLogType;
		nSubTypeCount = log.nSubTypeCount;
		nSubLogType = log.nSubLogType;
		nLogId    = log.nLogId;
		nThreadId = log.nThreadId;
		msgtime   = log.msgtime;
		msgLength = log.msgLength;
		memcpy(msg, log.msg,sizeof(msg));
		bIsopenCrypt = log.bIsopenCrypt;
		return *this;
	}   
};

#endif
