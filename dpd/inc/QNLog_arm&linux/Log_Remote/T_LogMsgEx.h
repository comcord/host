#ifndef T_LOGMSG_EX_H
#define T_LOGMSG_EX_H
#include "T_LogMsg.h"
enum LOG_CMD_TYPE
{
	DW_SETFILTER= 1,
	DW_LOG,
	DW_REG,
	DW_UNREG,
	DW_STR_FILTER,
	DW_STR_NO_FILTER,
	DW_NO_ADD_ID,
	DW_ADD_ID,
	DW_THREAD_ID_FILTER,
	DW_GET_MY_FILTER,
	DW_NOT_SHOW,
	DW_SHOW,
};

enum 
{
	TYPE_ERROR = 1 << 0,
	TYPE_WARNING = 1 << 1,
	TYPE_MESSAGE =  1 << 2,
	TYPE_DEBUG = 1 << 3,
};

struct LogSetting
{
	unsigned char nSubTypeCount[4];
	unsigned long long msgSubLogType[4] ;	
	unsigned long long debugSubLogType[4];
	unsigned long long errorSubLogType[4];
	unsigned long long warningSubLogType[4];
};
struct LogSettingEx
{
	LOG_CMD_TYPE m_Cmdtype;
	LogSetting m_LogSetting;
};
#define MXA_LEN 2048
#define RECV_PORT 63333

struct T_LogMsgEx
{
	bool	 isLittleEndian;
	LOG_CMD_TYPE cmdType;
	T_LogMsg msg ;
};

struct LogSettingID
{
	LOG_CMD_TYPE m_CmdType;
	char idBuff[1024];
};

struct FilterThreadID
{
	LOG_CMD_TYPE m_CmdType;
	char threadIDBuff[1024];
};

#endif
