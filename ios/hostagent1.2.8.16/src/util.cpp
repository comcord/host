#include "stdafx.h"
#include "util.h"

bool Json_Util_Write(Json::Value &root,std::string &str)
{
	Json::FastWriter fwriter;
	str = fwriter.write(root);
	return true;
}
bool Json_Util_Open(Json::Value &root,const char * buff)
{
	if(buff == NULL)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," 待解析的buff 为空，或者len无效 --JsonParser");
		return false;
	}

	Json::Reader read;
	try
	{
		bool bSucc = read.parse(buff,root,false);
		if(!bSucc)
		{
			//日志
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," 解析buff:%s失败 --JsonParser",buff);

			return false;
		}
	}
	catch (...)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," 解析buff:%s 异常 --JsonParser",buff);
		return false;		
	}
	return true;
}