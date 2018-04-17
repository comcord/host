#include "stdafx.h"
#include "CommonData.h"
#include "pubfunction.h"



int NPSResponse::ParseResponseCmd(const char * buffer ,U32 bufflen)
{
	std::string tmpjson;
	tmpjson.append( buffer ,bufflen );
	//tmpjson = "{\"message\":\"成功\",\"paramList\":{\"X1MeetingCommon\":{    \"RC_URL\": \"10.130.60.132:10012|10.130.60.132:10012\",   \"MS_URL\": \"10.130.60.132:10013/\",  \"NewRCUrlPort\": 9044,     \"CfgPath\": \"/mnt/internal_sd/meeting/config\",      \"LogFileOutPath\": \"/mnt/internal_sd/meeting/log/logwriter\"     }}, \"status\":\"0\"   }";

	//"{\"message\":\"成功\",\"paramList\":{\"X1MeetingCommon\":\"{\r\n    \"RC_URL\": \"10.130.60.132:10012|10.130.60.132:10012\",\r\n    \"MS_URL\": \"10.130.60.132:10013/\",\r\n    \"NewRCUrlPort\": 9044,\r\n    \"CfgPath\": \"/mnt/internal_sd/meeting/config\",\r\n    \"LogFileOutPath\": \"/mnt/internal_sd/meeting/log/logwriter\"\r\n}\r\n\"},\"status\":\"0\"}";

	//Log::writeWarning( N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,[AUTO] [NPS] 收到 NPS 查询结果json数据，:%s",tmpjson.c_str());
	Json ::Reader freader;  
	Json::Value root;
	if(!freader.parse(tmpjson,root ) )
	{		
		//Log::writeError( N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析 NPS 查询结果json数据，:%s",tmpjson.c_str());
		return -1;		
	}

	Json::Value stusinfo = root["status"];
	if( stusinfo.empty()  )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 收到 NPS 查询响应命令，无法解析结果json数据[status]:%s",tmpjson.c_str() );
		return -2;
	}

	Json::Value msginfo = root["message"]	;
	if( msginfo.empty() )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 收到 NPS 查询响应命令，无法解析结果json数据[message]，结束:%s",tmpjson.c_str() );
		return -3;
	}
	m_message = msginfo.asString();

	m_status = atoi( stusinfo.asString().c_str() ) ;
	if( m_status != 0 )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 收到 NPS 查询响应命令，受到错误码:%d ,描述:%s",m_status , m_message.c_str() );
		return -5;

	}

	Json::Value parmlistinfo = root["paramList"]	;
	if( parmlistinfo.empty() )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 收到 NPS 查询响应命令，无法解析结果json数据[paramList]，结束:%s",tmpjson.c_str() );
		return -4;
	}

	int iret =  m_common.ParseResponseCmd(parmlistinfo);
	if( iret != 0)
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 收到 NPS 查询响应命令，无法解析结果json数据[NPSX1MeetingCommon]，结束:%s",tmpjson.c_str() );

	}

	//iret =  m_appup.ParseResponseCmd(parmlistinfo);
	/*if( iret != 0)
	{
	Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,
	"[AUTO] [NPS] 收到 NPS 查询响应命令，无法解析结果json数据[NPSX1MeetingAppUpdate]，结束:%s",tmpjson.c_str() );

	}*/


	return iret;

	return 0;
}


int NPSX1MeetingCommon::ParseResponseCmd(Json::Value & jsonv)
{
	Json::Value comminfo = jsonv[ NPSPARAM1 ]	;//jsonv["X1MeetingCommon"]	;
	if( comminfo.empty() )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析结果 NPSX1MeetingCommon 数据[X1MeetingCommon]"  );
		return -3;
	}
	if( comminfo.empty() || ! comminfo.isString() )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析结果 %s " ,NPSPARAM1 );
		return -5;
	}
	Json::Value rcurlinfo ;

	if( comminfo.type() == Json::objectValue )
	{
		rcurlinfo = comminfo["RC_URL"]	;
	}
	else if ( comminfo.type() == Json::stringValue )
	{
		std::string commcontent = comminfo.asString();
		char * outbuf = NULL;
		outbuf = strrep( const_cast<char*>( commcontent.c_str() ) , "\\r\\n",""  ) ;
		if( outbuf )
		{
			printf("repla result:%s \r\n",outbuf);

			char * outbuf2 = strrep (outbuf,"\\\"","\"");
			if( outbuf2 )
			{
				Json ::Reader freader;  
				Json::Value root;
				if(!freader.parse(outbuf2,root ) )
				{		
					//Log::writeError( N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析 NPS 内部结果json数据，:%s", outbuf2  );
					return -6;		
				}
				rcurlinfo = root["RC_URL"]	;				

				printf("relpal 2 :%s \r\n",outbuf2 );
				free ( outbuf2 );
			}
			free( outbuf );
		}
		else
		{
			printf("repl failed \r\n");

		}
	}


	if( rcurlinfo.empty() )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析结果 NPSX1MeetingCommon 数据[RC_URL]  ");
		return -4;
	}
	std::string urldata = rcurlinfo.asString();

	char tmpbuf[100]={0};
	strcpy(tmpbuf,urldata.c_str() );
	char * thesep = strchr (tmpbuf,'|');
	if( thesep )
		*thesep = 0;
	int iret = SeparateColonStr(tmpbuf , m_rcurl_ip1 ,m_rcurl_pt1);
	if(iret != 0 )
	{
		//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析结果 NPSX1MeetingCommon 数据[RC_URL ip1 pt1]  ");
		return iret;
	}
	memset(tmpbuf,0,sizeof(tmpbuf));
	strcpy(tmpbuf,urldata.c_str() );
	thesep = strchr (tmpbuf,'|');
	if( thesep )
	{
		char tmpbuf2[100]={0};
		strcpy(tmpbuf2,thesep+1);
		iret = SeparateColonStr(tmpbuf2 , m_rcurl_ip2 ,m_rcurl_pt2);
		if(iret != 0 )
		{
			//Log::writeError(  N8_AGENT_LOGTYPE , N8_AGENT_LOGSUBID,"[AUTO] [NPS] 无法解析结果 NPSX1MeetingCommon 数据[RC_URL ip2 pt2]  ");
			return iret;
		}
	}
	return 0;
}

void NPSX1MeetingCommon::operator=(const NPSX1MeetingCommon & rht )
{
	strcpy(m_rcurl_ip1,rht.m_rcurl_ip1);
	m_rcurl_pt1 = rht.m_rcurl_pt1 ;
	strcpy(m_rcurl_ip2 , rht.m_rcurl_ip2 );	
	m_rcurl_pt2 = rht.m_rcurl_pt2;
}