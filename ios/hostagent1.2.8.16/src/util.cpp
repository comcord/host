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
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," ��������buff Ϊ�գ�����len��Ч --JsonParser");
		return false;
	}

	Json::Reader read;
	try
	{
		bool bSucc = read.parse(buff,root,false);
		if(!bSucc)
		{
			//��־
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," ����buff:%sʧ�� --JsonParser",buff);

			return false;
		}
	}
	catch (...)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000," ����buff:%s �쳣 --JsonParser",buff);
		return false;		
	}
	return true;
}