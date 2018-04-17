#pragma once

#define SER_I8(v) *(I8*)(pbuff+ser_len) = v; ser_len +=sizeof(I8)
#define SER_I16(v) *(I16*)(pbuff+ser_len) = v; ser_len +=sizeof(I16)
#define SER_I32(v) *(I32*)(pbuff+ser_len) = v; ser_len +=sizeof(I32)
#define SER_I64(v) *(I64*)(pbuff+ser_len) = v; ser_len +=sizeof(I64)
#define SER_U8(v) *(U8*)(pbuff+ser_len) = v; ser_len +=sizeof(U8)
#define SER_U16(v) *(U16*)(pbuff+ser_len) = v; ser_len +=sizeof(U16)
#define SER_U32(v) *(U32*)(pbuff+ser_len) = v; ser_len +=sizeof(U32)
#define SER_U64(v) *(U64*)(pbuff+ser_len) = v; ser_len +=sizeof(U64)
#define SER_STR(v,l) memcpy(pbuff+ser_len, v, l); ser_len +=l
#define SER_NOR(v,T) *(T*)(pbuff+ser_len) = v; ser_len +=sizeof(T)

#define USER_I8(v) v = *(I8*)(pbuff+unser_len); unser_len +=sizeof(I8)
#define USER_I16(v) v = *(I16*)(pbuff+unser_len); unser_len +=sizeof(I16)
#define USER_I32(v) v = *(I32*)(pbuff+unser_len); unser_len +=sizeof(I32)
#define USER_I64(v) v = *(I64*)(pbuff+unser_len); unser_len +=sizeof(I64)
#define USER_U8(v) v = *(U8*)(pbuff+unser_len; unser_len +=sizeof(U8)
#define USER_U16(v) v = *(U16*)(pbuff+unser_len); unser_len +=sizeof(U16)
#define USER_U32(v) v = *(U32*)(pbuff+unser_len); unser_len +=sizeof(U32)
#define USER_U64(v) v = *(U64*)(pbuff+unser_len); unser_len +=sizeof(U64)
#define USER_STR(v,l) memcpy(v, pbuff+unser_len, l); unser_len +=l
#define USER_NOR(v,T) v = *(T*)(pbuff+unser_len); unser_len +=sizeof(T)

#define  MEETINGHOST_CMD_REGISTER			1000
#define  MEETINGHOST_CMD_REGISTER_RESP		1001
#define  MEETINGHOST_CMD_INVITE 			1004
#define  MEETINGHOST_CMD_INVITED			1005
#define  MEETINGHOST_CMD_HEART				1006
#define  MEETINGHOST_CMD_HEART_RESP			1007
#define  MEETINGHOST_CMD_GETSHARE_REQ       1008    /*获得共享信息请求*/
#define  MEETINGHOST_CMD_GETSHARE_RESP      1009
#define  MEETINGHOST_CMD_TRANSINVITE        1010    /*其他host转发的邀请消息*/
#define  MEETINGHOST_CMD_LOCALINVITE        1011
#define  MEETINGHOST_CMD_DHTRESULT          1100
#define  MEETINGHOST_CMD_SETDHTHANDLE       1101
#define  MEETINGHOST_CMD_SHAREIP_REQ        1102    /*发布共享地址*/
#define  MEETINGHOST_CMD_SHAREIP_RESP       1103
//手机发布信息
#define  MEETINGHOST_CMD_SHAREPHONE_REQ     1104
#define  MEETINGHOST_CMD_SHAREPHONE_RESP    1105
//BUG分析引擎
#define  MEETINGHOST_TRANS_CMDID            2000  //这是一个通用的命令ID
#define  MEETINGHOST_TRANS_TRANSCMDID       2001
#define  MEETINGHOST_CMD_BUGENGIEN          2002  
//推荐联系人
#define  MEETINGHOST_CMD_LOCALRECOMMEND     2003   
#define  MEETINGHOST_CMD_TRANSRECOMMEND     2004

#define  MEETINGHOST_CMD_DOWNLINERESP       2005
#define  MEETINGHOST_TRANS_TRANSDOWNLINE    2006

//PC屏幕分享打洞
#define MEETINGHOST_CMD_DIGHOLE_REQUEST     2011
#define MEETINGHOST_CMD_DIGHOLE_RESP        2012
#define MEETINGHOST_CMD_NOTICEPCADDR	    2100

#define  MEETINGHOST_TRANS_DIGHOLE_REQUEST  2013
#define  MEETINGHOST_AGENT_CMD_DIGHOLE_REQ  2014

//邀请新命令
#define MEETINGHOST_CMD_INVITE_SINGLE             2015   
#define MEETINGHOST_CMD_INVITE_SINGLE_TRANS       2016
#define MEETINGHOST_CMD_LOCALINVITE_SINGLE        2017

#define MEETINGHOST_CMD_INVITED_SINGLE            2018
#define MEETINGHOST_CMD_INVITED_SINGLE_RESP       2019
#define MEETINGHOST_CMD_TRANSINVITE_SINGLE        2020
#define MEETINGHOST_CMD_INVITED_SINGLE_RESP_TRANS 2021
#define MEETINGHOST_CMD_INVITED_SINGLE_RESP_DEST  2022

#define  MEETINGHOST_CMD_TRANSRECOMMEND_NOTIFY    2115
#define  MEETINGHOST_CMD_LOCALRECOMMEND_NOTIFY    2116

#define MEETINGHOST_CMD_TRANSDATA_REQ			  2200
#define MEETINGHOST_CMD_TRANSDATA_RESP			  2201
#define MEETINGHOST_CMD_TRANSDATA_REQ_TRANS		  2202
#define MEETINGHOST_CMD_TRANSDATA_RESP_TRANS	  2203


#define ID_BUFF_SIZE  12
#define GUID_SIZE     37
#define SSIP_SIZE     16
#define TRANS_MAX_SIZE 1024

#define INVITE_ID_SIZE   32     
#define INVITE_NAME_SIZE 128
#define INVITED_SIZE     32
#define NAME_SIZE        128

#define TOKEN_SIZE       64

#define MEETINGHOST_CMD_DIGHOLE_REQ

struct meetingHost_cmd_reg_dht
{
public:
	meetingHost_cmd_reg_dht()
	{
		memset(_userid,0,sizeof(_userid));
		memset(_guid,0,sizeof(_guid));
		memset(_token,0,sizeof(_token));
	}

	int serialize(char * pbuff, int len) ;
	int unserialize(char * pbuff, int len);

	int GetSerialBuffSize()
	{
		int realSize = sizeof(_userid) + sizeof(_guid) + sizeof(_token);
		return realSize;
	}

	char _userid[ID_BUFF_SIZE];
	char _guid[GUID_SIZE];
	char _token[TOKEN_SIZE];
};

struct meetingHost_cmd_reg_old
{
public:
	meetingHost_cmd_reg_old()
	{
		memset(_userid,0,sizeof(_userid));
		memset(_guid,0,sizeof(_guid));
	}

	int serialize(char * pbuff, int len) ;
	int unserialize(char * pbuff, int len);

	int GetSerialBuffSize()
	{
		int realSize = sizeof(_userid) + sizeof(_guid);
		return realSize;
	}

	char _userid[ID_BUFF_SIZE];
	char _guid[GUID_SIZE];
};

/*agent向server注册命令*/
struct meetingHost_cmd_reg
{
public:
	meetingHost_cmd_reg()
	{
		memset(_userid,0,sizeof(_userid));
		memset(_guid,0,sizeof(_guid));
		_token = "";
	}

	int serialize(char * pbuff, int len) ;
	int unserialize(char * pbuff, int len);

	int GetSerialBuffSize()
	{
		int realSize = sizeof(_userid) + sizeof(_guid) + _token.length();
		return realSize;
	}

	char _userid[ID_BUFF_SIZE];
	char _guid[GUID_SIZE];
	std::string _token;
};

/*server向agent注册命令应答*/
class meetingHost_cmd_reg_resp
{
public:

	meetingHost_cmd_reg_resp()
	{
		memset(_guid,0,sizeof(_guid));
		status = 0;
	}

	int serialize(char * pbuff, int len) ;
	int unserialize(char * pbuff, int len);

	int GetSerialBuffSize()
	{
		int realSize = sizeof(_guid);
		return realSize;
	}

	char _guid[GUID_SIZE];
	int  status;
};

typedef struct meetingHost_cmd_HeartBeatReqMsg
{
public:
	meetingHost_cmd_HeartBeatReqMsg()
	{
		memset(_userid,0,sizeof(_userid));
		memset(_guid,0,sizeof(_guid));
		index = 0;
	}

	char _userid[ID_BUFF_SIZE];
	char _guid[GUID_SIZE];
	int index;

	int unSerialBuff(char* buff, int len);
	int SerialBuff(char* buff,int len);
	
	
	int GetSerialBuffSize()
	{
		int realSize = sizeof(_userid) + sizeof(_guid) +sizeof(index);
		return realSize;
	}
}meetingHost_cmd_HeartBeatReqMsg;

/*心跳应答*/
typedef struct meetingHost_cmd_HeartBeatReqMsg meetingHost_cmd_HeartBeatRespMsg;

/*被邀请命令*/
typedef struct meetingHost_invitedCmd
{
	meetingHost_invitedCmd()
	{
	  memset(inviteid,0,sizeof(inviteid));
	  memset(invitename,0,sizeof(invitename));
	  meetid = 0;
	  memset(invited,0,sizeof(invited));
	}

	char inviteid[INVITE_ID_SIZE];       //邀请人id
	char invitename[INVITE_NAME_SIZE];   //邀请人名称
	int  meetid ;                        //会议id
	char invited[INVITED_SIZE];          //被邀请人Id

	int Seralize(char* buff, size_t len);
	int UnSeralize(char * buff,size_t len);


	int GetRealSize()
	{
		int realSize = sizeof(inviteid) + sizeof(invitename) + sizeof(meetid) + sizeof(invited);
		return realSize;
	}
}meetingHost_invitedCmd;

typedef struct meetingHost_invitedCmd_Single
{
	meetingHost_invitedCmd_Single()
	{
		memset(invitename,0,sizeof(invitename));
		memset(inviteid,0,sizeof(inviteid));
		memset(invited,0,sizeof(invited));
		memset(guid,0,sizeof(guid));
		meetid = 0;
		
	}

	char  invitename[64];   //邀请人名称
	char  inviteid[32];      //邀请人id
	char  invited[32];       //被邀请人id
	char  guid[37];			 //邀请GUID
	int   meetid;            //会议id

	int Seralize(char* pbuff, size_t len)
	{
		if (pbuff == NULL || len < GetRealSize())
			return -1;

		int ser_len = 0;

		SER_STR(invitename,64);
		SER_STR(inviteid,32);
		SER_STR(invited,32);
		SER_STR(guid,37);
		SER_I32(meetid);
		
		return ser_len;
	}

	int UnSeralize(char * pbuff,size_t len)
	{
		if (pbuff == NULL || len < GetRealSize())
			return -1;

		int unser_len = 0;

		USER_STR(invitename,64);
		USER_STR(inviteid,32);
		USER_STR(invited,32);
		USER_STR(guid,37);
		USER_I32(meetid);

		return unser_len;
	}

	int GetRealSize()
	{
		int realSize = sizeof(inviteid) + sizeof(invitename) + sizeof(meetid) + sizeof(invited) + sizeof(guid);
		return realSize;
	}
}meetingHost_invitedCmd_Single;

typedef struct meetingHost_invitedCmd_Single_Resp
{
	meetingHost_invitedCmd_Single_Resp()
	{
		memset(invitid,0,sizeof(invitid));
		memset(guid,0,sizeof(guid));
		meetid = 0;

	}

	char  invitid[32];       //被邀请人id
	char  guid[37];			 //邀请GUID
	int   meetid;            //会议id

	int Seralize(char* pbuff, size_t len)
	{
		if (pbuff == NULL || len < GetRealSize())
			return -1;

		int ser_len = 0;

		SER_STR(invitid,32);
		SER_STR(guid,37);
		SER_I32(meetid);

		return ser_len;
	}

	int UnSeralize(char * pbuff,size_t len)
	{
		if (pbuff == NULL || len < GetRealSize())
			return -1;

		int unser_len = 0;

		USER_STR(invitid,32);
		USER_STR(guid,37);
		USER_I32(meetid);

		return unser_len;
	}

	int GetRealSize()
	{
		int realSize =  sizeof(meetid) + sizeof(invitid) + sizeof(guid);
		return realSize;
	}
}meetingHost_invitedCmd_Single_Resp;

/*发布分享地址请求*/
struct meetingHost_cmd_shareIpReq
{
	meetingHost_cmd_shareIpReq()
	{
	    memset(userid,0,sizeof(userid));
		memset(shareip,0,sizeof(shareip));
		shareport = 0;
		memset(_guid,0,sizeof(_guid));
		memset(_regGuid,0,sizeof(_regGuid));
		memset(name,0,sizeof(name));
	}

	char userid[ID_BUFF_SIZE];
	char shareip[SSIP_SIZE];
	int  shareport;
	char _guid[GUID_SIZE];
	char _regGuid[GUID_SIZE];
	char name[NAME_SIZE];

	int serialize(char * pbuff, int len) ;
	int unserialize(char * pbuff, int len);

	int GetSerialBuffSize()
	{
		int serial_buff_size = sizeof(userid) + sizeof(shareip) + sizeof(shareport) + sizeof(_guid) +sizeof(_regGuid) +sizeof(name);
		return serial_buff_size;
	}
};

/*发布分享地址应答*/
struct meetingHost_cmd_shareIpResp
{
	meetingHost_cmd_shareIpResp()
	{
	    memset(&_guid,0,sizeof(_guid));
	}

	int serialize(char * pbuff, int len);
	int unserialize(char * pbuff, int len);

	int GetRealSize()
	{
	    int realSize = sizeof(_guid);
		return realSize;
	}

	char _guid[GUID_SIZE];
};


/*请求分享地址命令*/
struct meetingHost_getShare_req
{
	char				m_userid[100];

	meetingHost_getShare_req()
	{
		memset(m_userid, 0 , sizeof(m_userid));
	}

	I32 Serialize(char * buffer ,U32  inbuflen);
	I32 UnSerialize(const char * buffer ,U32 inbufflen);
	unsigned int GetRealSize();
	std::string ToString();
};

/*请求分享地址命令的应答*/
struct meetingHost_getShare_resp
{
	char m_userid[100];
	char m_shareip[16];
	int  m_shareport;
	int  m_result;
	char m_name[128];

	meetingHost_getShare_resp()
	{
		memset(m_userid, 0 , sizeof(m_userid));
		memset(m_shareip, 0 , sizeof(m_shareip));
		m_shareport = 0;
		m_result = 0;
		memset(m_name,0,sizeof(m_name));
	}

	I32 Serialize(char * buffer ,U32  inbuflen);
	I32 UnSerialize(const char * buffer ,U32 inbufflen);
	unsigned int GetRealSize();
	std::string ToString();
};

/************************************************************************/
/*            BUG分析引擎新增命令                                       */
/************************************************************************/
struct  HostTransBaseCmd
{
	//hostServer只需要感知userId，下面的可以做黑盒传输，这样相当于一个新的通道，以后增加业务命令，可以复用该通道
	char                   userId[100];  //视讯号；
	int                    cmdId;        //内部命令Id（业务命令），目前只有一个7001，

	HostTransBaseCmd()
	{
		memset(userId,0,sizeof(userId));
		cmdId = 0;
	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetRealSize())
		{			
			return -1;
		}

		int serLen = 0;

		memcpy(pbuff+serLen,&userId,100);
		serLen += 100;

		memcpy(pbuff+serLen,&cmdId,sizeof(cmdId));
		serLen += sizeof(cmdId);

		return serLen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if(pbuff == NULL || len <  GetRealSize())
			return -1;

		int unSerLen = 0;

		memcpy(&userId, pbuff+unSerLen, 100);
		unSerLen += 100;

		memcpy(&cmdId, pbuff+unSerLen, sizeof(cmdId));
		unSerLen += sizeof(cmdId);

		return unSerLen;
	}

	unsigned int GetRealSize()
	{
		int realSize = sizeof(userId) + sizeof(cmdId);
		return realSize;
	}
};

/************************************************************************/
/*                          BUGENGIEN_001                           */
/************************************************************************/
struct  NotifyClientReg :public HostTransBaseCmd
{
	GMUint128             diagId;
	char         BugEngineIP[16];
	unsigned short BugEnginePort;
	int             BugEngineSid;

	NotifyClientReg()
	{
		memset(BugEngineIP,0,16);
		BugEnginePort = 0;
		BugEngineSid =  0;
	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetRealSize())
		{			
			return -1;
		}

		int serLen = 0;

		memcpy(pbuff+serLen,&userId,100);
		serLen += 100;

		memcpy(pbuff+serLen,&cmdId,sizeof(cmdId));
		serLen += sizeof(cmdId);

		memcpy(pbuff+serLen, diagId.GetData() , sizeof(GMUint128));
		serLen += sizeof(GMUint128);

		memcpy(pbuff+serLen, &BugEngineIP, 16);
		serLen += 16;

		memcpy(pbuff+serLen, &BugEnginePort, sizeof(BugEnginePort));
		serLen += sizeof(BugEnginePort);

		memcpy(pbuff+serLen, &BugEngineSid, sizeof(BugEngineSid));
		serLen += sizeof(BugEngineSid);

		return serLen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if(pbuff == NULL || len <  GetRealSize())
			return -1;

		int unSerLen = 0;
		memcpy(&userId, pbuff+unSerLen, 100);
		unSerLen += 100;

		memcpy(&cmdId, pbuff+unSerLen, sizeof(cmdId));
		unSerLen += sizeof(cmdId);

		memcpy(&diagId, pbuff+unSerLen, sizeof(diagId));
		unSerLen += sizeof(diagId);

		memcpy(&BugEngineIP,pbuff+unSerLen,16);
		unSerLen += 16;

		memcpy(&BugEnginePort, pbuff+unSerLen, sizeof(BugEnginePort));
		unSerLen += sizeof(BugEnginePort);

		memcpy(&BugEngineSid, pbuff+unSerLen, sizeof(BugEngineSid));
		unSerLen += sizeof(BugEngineSid);

		return unSerLen;
	}

	unsigned int GetRealSize()
	{
		int realSize = sizeof(diagId) + sizeof(BugEngineIP) + sizeof(BugEnginePort) + sizeof(BugEngineSid) + sizeof(userId) + sizeof(cmdId);
		return realSize;
	}

};

struct meetinghost_cmd_sharePhoneInfoReq
{
	std::string  token;
	//std::string  phonenum;
	std::string  userid;
	std::string  guid;
	std::string  regGuid;

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		int tokenLen = token.length();
		memcpy(pbuff+serlen,&tokenLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,token.c_str(),tokenLen);
		serlen += tokenLen;

		//int phonenumLen = phonenum.length();
		//memcpy(pbuff+serlen,&phonenumLen,sizeof(int));
		//serlen += sizeof(int);

		//memcpy(pbuff+serlen,phonenum.c_str(),phonenumLen);
		//serlen += phonenumLen;

		int useridLen = userid.length();
		memcpy(pbuff+serlen,&useridLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,userid.c_str(),useridLen);
		serlen += useridLen;

		int guidLen = guid.length();
		memcpy(pbuff+serlen,&guidLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,guid.c_str(),guidLen);
		serlen += guidLen;

		int regGuidLen = regGuid.length();
		memcpy(pbuff+serlen,&regGuidLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,regGuid.c_str(),regGuidLen);
		serlen += regGuidLen;

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unserLen = 0;

		int tokenlen = 0;
		memcpy(&tokenlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		token.assign(pbuff+unserLen,tokenlen);
		unserLen += tokenlen;

		//int phonenumlen = 0;
		//memcpy(&phonenumlen,pbuff+unserLen,sizeof(int));
		//unserLen += sizeof(int);

		//phonenum.assign(pbuff+unserLen,phonenumlen);
		//unserLen += phonenumlen;

		int useridlen = 0;
		memcpy(&useridlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		userid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		int regGuidlen = 0;
		memcpy(&regGuidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		regGuid.assign(pbuff+unserLen,regGuidlen);
		unserLen += regGuidlen;

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + token.length() +/* sizeof(int) + phonenum.length() +*/ sizeof(int) + userid.length() + sizeof(int) + guid.length() + sizeof(int) + regGuid.length();
	}
};

typedef struct meetinghost_cmd_sharePhoneInfoResp
{
	std::string uerid;
	std::string token;
	std::string guid;
	int         status;

	meetinghost_cmd_sharePhoneInfoResp()
	{
		status = 0;
	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		int useridLen = uerid.length();
		memcpy(pbuff+serlen,&useridLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,uerid.c_str(),useridLen);
		serlen += useridLen;

		int tokenLen = token.length();
		memcpy(pbuff+serlen,&tokenLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,token.c_str(),tokenLen);
		serlen += tokenLen;

		int guidLen = guid.length();
		memcpy(pbuff+serlen,&guidLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,guid.c_str(),guidLen);
		serlen += guidLen;

		memcpy(pbuff+serlen,&status,sizeof(int));
		serlen += sizeof(int);

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unserLen = 0;

		int useridlen = 0;
		memcpy(&useridlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		uerid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int tokenlen = 0;
		memcpy(&tokenlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		token.assign(pbuff+unserLen,tokenlen);
		unserLen += tokenlen;

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		memcpy(&status,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + uerid.length() + sizeof(int) + token.length() + sizeof(int) + guid.length() + sizeof(int);
	}
}meetinghost_cmd_downLineMsgResp; 


struct DhtInfo
{
	DhtInfo():sid(0),sender(NULL){}

	AsynModel::SptrISender	sender;
	int			            sid;
};

//屏幕分享客户端请求获取分享信息命令
struct getShareInfoAndDigHoleCmd
{
	getShareInfoAndDigHoleCmd()
	{
		memset(userid,0,12);
	}

	char         userid[12];
	std::string  recvData;

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		memcpy(pbuff+serlen,&userid,12);
		serlen += 12;

		int recvDataLen = recvData.length();
		memcpy(pbuff+serlen,&recvDataLen,sizeof(recvDataLen));
		serlen += sizeof(recvDataLen);

		memcpy(pbuff+serlen,recvData.c_str(),recvDataLen);
		serlen += recvDataLen;

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unSerLen = 0;
		memcpy(&userid, pbuff+unSerLen,12);
		unSerLen += 12;

		int recvDataLen = 0;
		memcpy(&recvDataLen,pbuff+unSerLen,sizeof(int));
		unSerLen += sizeof(int);

		recvData.assign(pbuff+unSerLen,recvDataLen);
		unSerLen += recvDataLen;

		return unSerLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(userid) + sizeof(int) + recvData.length();
	}
};


//Agent发送屏幕分享客户端打洞信息
struct digHoleCmdToClientResultMsg
{
	digHoleCmdToClientResultMsg()
	{
		memset(userid,0,12);
		memset(publicIp,0,16);
		publicPort = 0;
	}

	char           userid[12];
	std::string    data;
	char           publicIp[16];
	unsigned short publicPort;

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		memcpy(pbuff+serlen,&userid,12);
		serlen += 12;

		int dataLen = data.length();
		memcpy(pbuff+serlen,&dataLen,sizeof(dataLen));
		serlen += sizeof(dataLen);

		memcpy(pbuff+serlen,data.c_str(),dataLen);
		serlen += dataLen;

		memcpy(pbuff+serlen,&publicIp,16);
		serlen += 16;

		memcpy(pbuff+serlen,&publicPort,sizeof(publicPort));
		serlen += sizeof(publicPort);

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unSerLen = 0;

		memcpy(&userid, pbuff+unSerLen,12);
		unSerLen += 12;

		int dataLen = 0;
		memcpy(&dataLen,pbuff+unSerLen,sizeof(int));
		unSerLen += sizeof(int);

		data.assign(pbuff+unSerLen,dataLen);
		unSerLen += dataLen;

		memcpy(&publicIp,pbuff+unSerLen,16);
		unSerLen += 16;

		memcpy(&publicPort,pbuff+unSerLen,sizeof(publicPort));
		unSerLen += sizeof(publicPort);

		return unSerLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(userid) + sizeof(int) + data.length() + sizeof(publicIp) + sizeof(publicPort);
	}
};

/*请求分享地址命令的应答 punch 模式响应*/
struct getShareInfoAndDigHoleResp
{
	char m_userid[100];
	char m_shareip[16];
	int  m_shareport;
	int  m_result;     //0表注册并发布，1注册未发布，2未注册
	//返回查询用户参会信息，0：用户参会，查询成功；1：表示用户已注册，但未发布；2：表示用户未参会，不在列表中。
	char m_name[128];

	char m_pcwanip[16];
	int  m_pcwanport;

	getShareInfoAndDigHoleResp()
	{
		memset(m_userid, 0 , sizeof(m_userid));
		memset(m_shareip, 0 , sizeof(m_shareip));
		m_shareport = 0;
		m_result = 2;
		memset(m_name,0,sizeof(m_name));
		memset(m_pcwanip,0,sizeof(m_pcwanip));
		m_pcwanport = 0;
	}


	I32 Serialize(char * buffer ,U32  inbuflen)
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

		memcpy(buffer+serLen, &m_pcwanip, sizeof(m_pcwanip));
		serLen += sizeof(m_pcwanip);

		memcpy(buffer+serLen, &m_pcwanport, sizeof(m_pcwanport));
		serLen += sizeof(m_pcwanport);

		return serLen;
	}

	I32 UnSerialize(const char * buffer ,U32 inbufflen)
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

		memcpy(&m_shareip, buffer+unSerLen, sizeof(m_pcwanip));
		unSerLen += sizeof(m_pcwanip);

		memcpy(&m_pcwanport,buffer+unSerLen,sizeof(m_pcwanport));
		unSerLen += sizeof(m_pcwanport);

		return unSerLen;
	}

	unsigned int GetRealSize()
	{
		int realSize = sizeof(int) + strlen(m_userid) + sizeof(m_shareip) + sizeof(m_shareport) 
			+ sizeof(m_result) + sizeof(m_name)
			+ sizeof(m_pcwanip) + sizeof(m_pcwanport);
		return realSize;
	}

	std::string ToString()
	{
		char tmps[1024] = {0};
		char * tmpid = m_userid;
		char * tmpip = m_shareip;
		char * tmpname = m_name;
		sprintf(tmps , "[ userid: %s , useraddr : %s : %d, username : %s, pcwanip:%s ,pcwanport:%u ]",
			tmpid,tmpip,m_shareport,tmpname,m_pcwanip,m_pcwanport );
		return tmps;
	}

};

/************************************************************************/
/*     BUGENGIEN命令                                                 */
/************************************************************************/
struct baseCmd
{
	GMUint128   diagId;
	std::string userId;
};

struct notifyRegister :public baseCmd
{
	notifyRegister():BugEnginePort(0),BugEngineSid(0)
	{
		memset(BugEngineIP,0,16);
	}

	int Seralize(char* buff, size_t len)
	{
		if (NULL == buff || len < GetRealSize())
		{			
			return -1;
		}

		int serLen = 0;
		memcpy(buff+serLen, diagId.GetData() , sizeof(diagId));
		serLen += sizeof(diagId);

		int lenth = userId.length();
		memcpy(buff+serLen, &lenth , sizeof(lenth));
		serLen += sizeof(lenth);

		memcpy(buff+serLen, userId.c_str() , lenth);
		serLen += lenth;

		memcpy(buff+serLen, &BugEngineIP , sizeof(BugEngineIP));
		serLen += sizeof(BugEngineIP);

		memcpy(buff+serLen, &BugEnginePort , sizeof(BugEnginePort));
		serLen += sizeof(BugEnginePort);

		memcpy(buff+serLen, &BugEngineSid , sizeof(BugEngineSid));
		serLen += sizeof(BugEngineSid);

		return serLen;
	}

	int UnSeralize(const char* buff, size_t len)
	{
		if (NULL == buff || len < sizeof(diagId) )
		{
			return -1;
		}

		int unSerLen = 0;
		memcpy((void *)diagId.GetData(), buff+unSerLen, sizeof(diagId));
		unSerLen += sizeof(diagId);

		int lenth = 0;
		memcpy(&lenth, buff+unSerLen, sizeof(int));
		unSerLen += sizeof(int);

		userId.assign(buff+unSerLen, lenth);
		unSerLen += lenth;

		memcpy(&BugEngineIP, buff+unSerLen, sizeof(BugEngineIP));
		unSerLen += sizeof(BugEngineIP);

		memcpy(&BugEnginePort, buff+unSerLen, sizeof(BugEnginePort));
		unSerLen += sizeof(BugEnginePort);

		memcpy(&BugEngineSid, buff+unSerLen, sizeof(BugEngineSid));
		unSerLen += sizeof(BugEngineSid);

		return unSerLen;
	}

	unsigned int GetRealSize()
	{
		return (unsigned int)(sizeof(BugEngineIP) + sizeof(BugEnginePort) + sizeof(int)) + sizeof(GMUint128) + userId.length();
	}

	char            BugEngineIP[16];
	unsigned short  BugEnginePort;
	int             BugEngineSid;
};

struct  InviteCmdContext
{
	InviteCmdContext()
	{
		memset(InviteData,0,512);
		sendInviteMsgCount = 0;
	}

	char  InviteData[512];
	int   sendInviteMsgCount;
	int   meetingid;
};

/****************Host转发数据*******************/
struct meetingHost_trans_data
{
	char from_userid[ID_BUFF_SIZE];
	char to_userid[ID_BUFF_SIZE];
	unsigned long long trans_data_uid;
	unsigned short dataLen;
	char buf[TRANS_MAX_SIZE];

	meetingHost_trans_data()
	{
		memset(from_userid,0,sizeof(from_userid));
		memset(to_userid,0,sizeof(from_userid));
		memset(buf,0,sizeof(buf));
		dataLen = 0;
		trans_data_uid = 0;
	}

	int GetRealSize()
	{
		return sizeof(from_userid) + sizeof(to_userid) + sizeof(trans_data_uid) + sizeof(dataLen) + dataLen;
	}

	int Seralize(char* buff, size_t len)
	{
		if (NULL == buff || len < GetRealSize() || dataLen > TRANS_MAX_SIZE)
		{			
			return -1;
		}

		int serLen = 0;
		memcpy(buff+serLen, from_userid , sizeof(from_userid));
		serLen += sizeof(from_userid);

		memcpy(buff+serLen, to_userid , sizeof(to_userid));
		serLen += sizeof(to_userid);

		memcpy(buff+serLen, &trans_data_uid , sizeof(trans_data_uid));
		serLen += sizeof(trans_data_uid);

		memcpy(buff+serLen, &dataLen , sizeof(dataLen));
		serLen += sizeof(dataLen);

		memcpy(buff+serLen, buf , dataLen);
		serLen += dataLen;

		return serLen;
	}

	int UnSeralize(const char* buff, size_t len)
	{
		if (NULL == buff || len < ID_BUFF_SIZE )
		{
			return -1;
		}

		int unSerLen = 0;
		memcpy(from_userid, buff+unSerLen, sizeof(from_userid));
		unSerLen += sizeof(from_userid);

		memcpy(to_userid, buff+unSerLen, sizeof(to_userid));
		unSerLen += sizeof(to_userid);

		memcpy(&trans_data_uid, buff+unSerLen, sizeof(trans_data_uid));
		unSerLen += sizeof(trans_data_uid);

		memcpy(&dataLen, buff+unSerLen, sizeof(dataLen));
		unSerLen += sizeof(dataLen);

		if (dataLen <= TRANS_MAX_SIZE)
		{
			memcpy(buf, buff+unSerLen, dataLen);
			unSerLen += dataLen;
		}
		
		return unSerLen;
	}
};
