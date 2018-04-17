#include "stdafx.h"
#include "meetingHostCmd.h"

int meetingHost_cmd_reg_dht::serialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int ser_len = 0;

	SER_STR(_userid,ID_BUFF_SIZE);
	SER_STR(_guid,GUID_SIZE);
	SER_STR(_token,TOKEN_SIZE);

	return ser_len;
}


int meetingHost_cmd_reg_dht::unserialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int unser_len = 0;

	USER_STR(_userid,ID_BUFF_SIZE);
	USER_STR(_guid,GUID_SIZE);
	USER_STR(_token,TOKEN_SIZE);

	return unser_len;
}

int meetingHost_cmd_reg_old::serialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int ser_len = 0;

	SER_STR(_userid,ID_BUFF_SIZE);
	SER_STR(_guid,GUID_SIZE);

	return ser_len;
}


int meetingHost_cmd_reg_old::unserialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int unser_len = 0;

	USER_STR(_userid,ID_BUFF_SIZE);
	USER_STR(_guid,GUID_SIZE);

	return unser_len;
}

/************************************************************************/
/*agent向server注册命令*                                                */
/************************************************************************/
int meetingHost_cmd_reg::serialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
	return -1;

	int ser_len = 0;
	
	SER_STR(_userid,ID_BUFF_SIZE);
	SER_STR(_guid,GUID_SIZE);

	int tokenlen = _token.length();
	memcpy(pbuff+ser_len,&tokenlen,sizeof(int));
	ser_len += sizeof(int);

	memcpy(pbuff+ser_len,_token.c_str(),tokenlen);
	ser_len += tokenlen;

	return ser_len;
}


int meetingHost_cmd_reg::unserialize(char * pbuff, int len)
{
	if(pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int unser_len = 0;
	
	USER_STR(_userid,ID_BUFF_SIZE);
	USER_STR(_guid,GUID_SIZE);

	int tokenlen = 0;
	memcpy(&tokenlen,pbuff+unser_len,sizeof(int));
	unser_len += sizeof(int);

	_token.assign(pbuff+unser_len,tokenlen);
	unser_len += tokenlen;

	return unser_len;
}
/************************************************************************/
/*server向agent注册命令应答                                             */
/************************************************************************/
int meetingHost_cmd_reg_resp :: serialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
	  return -1;

	int ser_len = 0;
	SER_STR(_guid,GUID_SIZE);

	memcpy(pbuff+ser_len,&status,sizeof(int));
	ser_len += sizeof(int);

	return ser_len;
}

int meetingHost_cmd_reg_resp::unserialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
		return -1;
	
	int unser_len = 0;
	USER_STR(_guid,GUID_SIZE);

	memcpy(&status,pbuff+unser_len,sizeof(status));
	unser_len += sizeof(status);

	return unser_len;
}


/************************************************************************/
/*发布分享地址请求                                                      */
/************************************************************************/

int meetingHost_cmd_shareIpReq::serialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int ser_len = 0;

	SER_STR(userid,ID_BUFF_SIZE);
	SER_STR(shareip,SSIP_SIZE);
	SER_I32(shareport);
	SER_STR(_guid,GUID_SIZE);
	SER_STR(_regGuid,GUID_SIZE);
	SER_STR(name,NAME_SIZE);

	return ser_len;
}

int meetingHost_cmd_shareIpReq::unserialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
		return -1;
	
	int unser_len = 0;
	
	USER_STR(userid,ID_BUFF_SIZE);
	USER_STR(shareip,SSIP_SIZE);
	USER_I32(shareport);
	USER_STR(_guid,GUID_SIZE);
	USER_STR(_regGuid,GUID_SIZE);
	USER_STR(name,NAME_SIZE);

	return unser_len;
}


/************************************************************************/
/*发布分享地址应答                                                     */
/************************************************************************/
int meetingHost_cmd_shareIpResp:: serialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetRealSize())
		return -1;

	int ser_len = 0;
	
	SER_STR(_guid,GUID_SIZE);

	return ser_len;
}

int meetingHost_cmd_shareIpResp::unserialize(char * pbuff, int len)
{
	if (pbuff == NULL || len < GetRealSize())
		return -1;

	int unser_len = 0;

	USER_STR(_guid,GUID_SIZE);

	return unser_len;
}

/************************************************************************/
/* 心跳命令                                                                 */
/************************************************************************/
int meetingHost_cmd_HeartBeatReqMsg::SerialBuff(char* pbuff,int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int ser_len = 0;
	
	SER_STR(_userid,ID_BUFF_SIZE);
	SER_STR(_guid,GUID_SIZE);
	SER_I32(index);

	return ser_len;
}

int meetingHost_cmd_HeartBeatReqMsg::unSerialBuff(char* pbuff, int len)
{
	if (pbuff == NULL || len < GetSerialBuffSize())
		return -1;

	int unser_len = 0;

	USER_STR(_userid,ID_BUFF_SIZE);
	USER_STR(_guid,GUID_SIZE);
	USER_I32(index);

	return unser_len;
}

/************************************************************************/
/*  被邀请命令                                             */
/************************************************************************/
int meetingHost_invitedCmd::Seralize(char* pbuff, size_t len)
{
	if (pbuff == NULL || len < GetRealSize())
		return -1;

	int ser_len = 0;

	SER_STR(inviteid,INVITE_ID_SIZE);
	SER_STR(invitename,INVITE_NAME_SIZE);
	SER_I32(meetid);
	SER_STR(invited,INVITED_SIZE);

	return ser_len;
}

int meetingHost_invitedCmd::UnSeralize(char * pbuff,size_t len)
{
	if (pbuff == NULL || len < GetRealSize())
		return -1;
	
	int unser_len = 0;

	USER_STR(inviteid,INVITE_ID_SIZE);
	USER_STR(invitename,INVITE_NAME_SIZE);
	USER_I32(meetid);
	USER_STR(invited,INVITED_SIZE);

	return unser_len;
}


I32 meetingHost_getShare_req::Serialize(char * buffer ,U32  inbuflen)
{
	if( buffer == NULL || inbuflen < GetRealSize())
		return -1;

	int serLen = 0;

	int idlen = strlen(m_userid);

	memcpy(buffer, &idlen, sizeof(int));
	serLen += sizeof(int);

	memcpy(buffer+serLen, &m_userid, idlen);
	serLen += idlen;

	return serLen;
}
I32 meetingHost_getShare_req::UnSerialize(const char * buffer ,U32 inbufflen)
{

	if(buffer == NULL || inbufflen < GetRealSize())
		return -1;

	int unSerLen = 0;
	int idlen = 0;
	memcpy(&idlen , buffer , sizeof(int));
	unSerLen += sizeof(int);

	if (idlen > sizeof(m_userid))
		return -2;

	memcpy(&m_userid, buffer+unSerLen, idlen);
	unSerLen += idlen;

	return unSerLen;
}

unsigned int meetingHost_getShare_req::GetRealSize()
{
	int realSize = sizeof(int) + strlen(m_userid);

	return realSize;
}

std::string meetingHost_getShare_req::ToString()
{
	char tmps[1024] = {0};
	char * tmpid = m_userid;
	sprintf(tmps , "[ userid: %s ]",tmpid);
	return tmps;
}

/************************************************************************/
/*  请求分享地址命令的应答                                       */
/************************************************************************/
#if 0
int meetingHost_getShare_resp::SerialBuff(char *pbuff,int len)
{
	int ser_len = 0;

	SER_STR(userid,ID_BUFF_SIZE);
	SER_STR(shareip,SSIP_SIZE);
	SER_I32(shareport);

	return ser_len;
}

int meetingHost_getShare_resp::unSerialBuff(char *pbuff,int len)
{
	int unser_len = 0;

	USER_STR(userid,ID_BUFF_SIZE);
	USER_STR(shareip,SSIP_SIZE);
	USER_I32(shareport);

	return unser_len;
}
#endif
I32 meetingHost_getShare_resp::Serialize(char * buffer ,U32  inbuflen)
{
	if( buffer == NULL || inbuflen < GetRealSize())
		return -1;

	int serLen = 0;
	int idlen = strlen(m_userid);

	memcpy(buffer, &idlen, sizeof(int));
	serLen += sizeof(int);

	memcpy(buffer+serLen, &m_userid, idlen);
	serLen += idlen;

	memcpy(buffer+serLen, &m_shareip, sizeof(m_shareip));
	serLen += sizeof(m_shareip);

	memcpy(buffer+serLen, &m_shareport, sizeof(m_shareport));
	serLen += sizeof(m_shareport);

	memcpy(buffer+serLen,&m_result,sizeof(m_result));
	serLen += sizeof(m_result);

	memcpy(buffer+serLen,&m_name,sizeof(m_name));
	serLen += sizeof(m_name);

	return serLen;
}
I32 meetingHost_getShare_resp::UnSerialize(const char * buffer ,U32 inbufflen)
{
	if(buffer == NULL || inbufflen <  GetRealSize())
		return -1;

	int unSerLen = 0;
	int idlen = 0;
	memcpy(&idlen, buffer, sizeof(int));
	unSerLen += sizeof(int);

	if (idlen > sizeof(m_userid))
		return -2;

	memcpy(&m_userid, buffer+unSerLen, idlen);
	unSerLen += idlen;

	memcpy(&m_shareip, buffer+unSerLen, sizeof(m_shareip));
	unSerLen += sizeof(m_shareip);

	memcpy(&m_shareport,buffer+unSerLen,sizeof(m_shareport));
	unSerLen += sizeof(m_shareport);

	memcpy(&m_result,buffer+unSerLen,sizeof(m_result));
	unSerLen += sizeof(m_result);

	memcpy(&m_name,buffer+unSerLen,sizeof(m_name));
	unSerLen += sizeof(m_name);

	return unSerLen;
}

unsigned int meetingHost_getShare_resp::GetRealSize()
{
	int realSize = sizeof(int) + strlen(m_userid) + sizeof(m_shareip) + sizeof(m_shareport) + sizeof(m_result) + sizeof(m_name);

	return realSize;
}

std::string meetingHost_getShare_resp::ToString()
{
	char tmps[1024] = {0};
	char * tmpid = m_userid;
	char * tmpip = m_shareip;
	char * tmpname = m_name;
	sprintf(tmps , "[ userid: %s , useraddr : %s : %d, username : %s]",tmpid,tmpip,m_shareport,tmpname);
	return tmps;
}



