#pragma once
#include "stdafx.h"
#include <string>
#include <list>

using namespace std;

//客户端发给HOST的推荐请求命令
#define RECOMMEND_HOST_REQUEST 4009
//HOST回复给客户端的推荐命令回复
#define RECOMMEND_HOST_RESP 5000
//Server发送Agent推荐请求命令
#define RECOMMEND_CMD_REQUEST   5001
//Agent发送给Server的推荐命令回应
#define RECOMMEND_CMD_RESP      5002
//Server本地推荐联系人命令
#define MEETINGHOST_CMD_LOCAL_RECOMMEND 5003
//Server转发推荐联系人命令
#define MEETINGHOST_CMD_TRANS_RECOMMEND 5004
//Server发送Agent的推荐通知命令
#define MEETINGHOST_CMD_AGENT_RECOMMEND_REQUEST  5005
//Agent发送Server的推荐请求命令
#define MEETINGHOST_CMD_AGENT_RECOMMEND_RESP     5006
//Agent获取数据
#define MEETINGHOST_CMD_GET_RECOMMENDMSG_REQUEST 5020
//server发送数据
#define MEETINGHOST_CMD_GET_RECOMMENDMSG_RESP    5021

//获取HOST的地址
#define RECOMMEND_GETHOST_REQUEST 5007
#define RECOMMEND_GETHOST_RESP    5008
//向HOST请求推荐
#define  RECOMMEND_CMD_RECOMMENDREQUEST 5009
#define  RECOMMEND_CMD_RECOMMENDRESP    5010

//HOST与客户端交互
#define  MEETINGHOST_RECOMMEND_REQUEST 5011
#define  MEETINGHOST_RECOMMEND_RESP    5012

#define  MEETINGHOST_RECOMMEND_DELMEM      5013
#define  MEETINGHOST_RECOMMNED_SETMSGLOCAL 5014

#define  MEETINGHOST_RECOMMEND_MANAGE_REC  5015
#define  MEETINGHOST_RECOMMEND_MANAGE_REC_TRANS 5016


//单个联系人信息
struct Contact
{
	std::string  userid;		//视讯号
	std::string  name;			//姓名
	std::string  department;    //部门
	std::string  phonenum;		//手机号
	std::string  serilnum;		//设备号后四位

	Contact()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if(NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serLen = 0;

		unsigned short useridLen = userid.length();
		memcpy(pbuff+serLen,&useridLen,sizeof(unsigned short));
		serLen += sizeof(unsigned short);

		memcpy(pbuff+serLen,userid.c_str(),useridLen);
		serLen += useridLen;

		unsigned short nameLen = name.length();
		memcpy(pbuff+serLen,&nameLen,sizeof(unsigned short));
		serLen += sizeof(unsigned short);

		memcpy(pbuff+serLen,name.c_str(),nameLen);
		serLen += nameLen;

		unsigned short departmentLen = department.length();
		memcpy(pbuff+serLen,&departmentLen,sizeof(unsigned short));
		serLen += sizeof(unsigned short);

		memcpy(pbuff+serLen,department.c_str(),departmentLen);
		serLen += departmentLen;

		unsigned short phonenumLen = phonenum.length();
		memcpy(pbuff+serLen,&phonenumLen,sizeof(unsigned short));
		serLen += sizeof(unsigned short);

		memcpy(pbuff+serLen,phonenum.c_str(),phonenumLen);
		serLen += phonenumLen;

		unsigned short serilnumLen = serilnum.length();
		memcpy(pbuff+serLen,&serilnumLen,sizeof(unsigned short));
		serLen += sizeof(unsigned short);

		memcpy(pbuff+serLen,serilnum.c_str(),serilnumLen);
		serLen += serilnumLen;

		return serLen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unserLen = 0;

		unsigned short useridLen = 0;
		memcpy(&useridLen,pbuff+unserLen,sizeof(unsigned short));
		unserLen += sizeof(unsigned short);

		userid.assign(pbuff+unserLen,useridLen);
		unserLen += useridLen;

		unsigned short nameLen = 0;
		memcpy(&nameLen,pbuff+unserLen,sizeof(unsigned short));
		unserLen += sizeof(unsigned short);

		name.assign(pbuff+unserLen,nameLen);
		unserLen += nameLen;

		unsigned short departmentLen = 0;
		memcpy(&departmentLen,pbuff+unserLen,sizeof(unsigned short));
		unserLen += sizeof(unsigned short);

		department.assign(pbuff+unserLen,departmentLen);
		unserLen += departmentLen;

		unsigned short phonenumLen = 0;
		memcpy(&phonenumLen,pbuff+unserLen,sizeof(unsigned short));
		unserLen += sizeof(unsigned short);

		phonenum.assign(pbuff+unserLen,phonenumLen);
		unserLen += phonenumLen;

		unsigned short serilnumLen = 0;
		memcpy(&serilnumLen,pbuff+unserLen,sizeof(unsigned short));
		unserLen += sizeof(unsigned short);

		serilnum.assign(pbuff+unserLen,serilnumLen);
		unserLen += serilnumLen;

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(unsigned short) + userid.length() + sizeof(unsigned short) + name.length() + sizeof(unsigned short) + department.length() + sizeof(unsigned short) + phonenum.length() + sizeof(unsigned short) + serilnum.length();
	}
};

//多个目标推荐人的信息
struct RecommendRequestMsg
{
	std::string               contactListName;    //通讯录名称
	std::string               contactListId;      //通讯录的UID
	unsigned int              contactListVersion; //通讯录的版本号
	std::string               guid;               //每次推荐的GUID
	std::string				  sourceId;			  //推荐原始人
	std::list<std::string>    destUserid;		  //推荐目标的视讯号
	std::list<Contact>        recContacters;      //（多个）推荐人的信息

	RecommendRequestMsg()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		int contactListNameLen = contactListName.length();
		memcpy(pbuff+serlen,&contactListNameLen,sizeof(contactListNameLen));
		serlen += sizeof(contactListNameLen);

		memcpy(pbuff+serlen,contactListName.c_str(),contactListNameLen);
		serlen += contactListNameLen;

		int contactListIdLen = contactListId.length();
		memcpy(pbuff+serlen,&contactListIdLen,sizeof(contactListIdLen));
		serlen += sizeof(contactListIdLen);

		memcpy(pbuff+serlen,contactListId.c_str(),contactListIdLen);
		serlen += contactListIdLen;

		memcpy(pbuff+serlen,&contactListVersion,sizeof(contactListVersion));
		serlen += sizeof(contactListVersion);

		int guidLen = guid.length();
		memcpy(pbuff+serlen,&guidLen,sizeof(guidLen));
		serlen += sizeof(guidLen);

		memcpy(pbuff+serlen,guid.c_str(),guidLen);
		serlen += guidLen;

		int sourceIdLen = sourceId.length();
		memcpy(pbuff+serlen,&sourceIdLen,sizeof(sourceIdLen));
		serlen += sizeof(sourceIdLen);

		memcpy(pbuff+serlen,sourceId.c_str(),sourceIdLen);
		serlen += sourceIdLen;

		int destUseridLen = destUserid.size();
		memcpy(pbuff+serlen,&destUseridLen,sizeof(destUseridLen));
		serlen += sizeof(destUseridLen);

		for(std::list<std::string>::iterator iter = destUserid.begin();iter != destUserid.end();iter++)
		{
			int tmpdestUseridLen = iter->length();
			memcpy(pbuff+serlen,&tmpdestUseridLen,sizeof(tmpdestUseridLen));
			serlen += sizeof(tmpdestUseridLen);

			memcpy(pbuff+serlen,iter->c_str(),tmpdestUseridLen);
			serlen += tmpdestUseridLen;
		}

		int recContactersLen = recContacters.size();
		memcpy(pbuff+serlen,&recContactersLen,sizeof(int));
		serlen += sizeof(int);

		for(std::list<Contact>::iterator it = recContacters.begin();it != recContacters.end();it++)
		{
			int contactLen = it->GetSerialBuffSize();
			memcpy(pbuff+serlen,&contactLen,sizeof(int));
			serlen += sizeof(int);

			int lserlen = it->Serialize(pbuff+serlen,contactLen);
			serlen += lserlen;
		}

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize()) 
		{
			return -1;
		}

		int unserLen = 0;

		int contactListNamelen = 0;
		memcpy(&contactListNamelen,pbuff+unserLen,sizeof(contactListNamelen));
		unserLen += sizeof(contactListNamelen);

		contactListName.assign(pbuff+unserLen,contactListNamelen);
		unserLen += contactListNamelen;

		int contactListIdlen = 0;
		memcpy(&contactListIdlen,pbuff+unserLen,sizeof(contactListIdlen));
		unserLen += sizeof(contactListIdlen);

		contactListId.assign(pbuff+unserLen,contactListIdlen);
		unserLen += contactListIdlen;

		memcpy(&contactListVersion,pbuff+unserLen,sizeof(contactListVersion));
		unserLen += sizeof(contactListVersion);

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(guidlen));
		unserLen += sizeof(guidlen);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		int sourceIdLen = 0;
		memcpy(&sourceIdLen,pbuff+unserLen,sizeof(sourceIdLen));
		unserLen += sizeof(sourceIdLen);

		sourceId.assign(pbuff+unserLen,sourceIdLen);
		unserLen += sourceIdLen;

		int destUseridLen = 0;
		memcpy(&destUseridLen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		for(int i = 0;i < destUseridLen;i++)
		{
			std::string tmp = "";

			int useridlen = 0;
			memcpy(&useridlen,pbuff+unserLen,sizeof(int));
			unserLen += sizeof(int);

			tmp.assign(pbuff+unserLen,useridlen);
			unserLen += useridlen;

			destUserid.push_back(tmp);
		}

		int recContactersLen = 0;
		memcpy(&recContactersLen,pbuff+unserLen,sizeof(recContactersLen));
		unserLen += sizeof(recContactersLen);

		for(int i = 0;i < recContactersLen;i++)
		{
			Contact tmp;

			int contactLen = 0;
			memcpy(&contactLen,pbuff+unserLen,sizeof(int));
			unserLen += sizeof(int);

			tmp.Unserialize(pbuff+unserLen,contactLen);
			unserLen += contactLen;

			recContacters.push_back(tmp);
		}

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		int destUseridSize = 0;
		for (std::list<string>::iterator it1 = destUserid.begin();it1 != destUserid.end();it1++)
		{
			destUseridSize += sizeof(int);
			destUseridSize += it1->length();
		}

		int recContactersSize = 0;
		for (std::list<Contact>::iterator it2 = recContacters.begin();it2 != recContacters.end();it2++)
		{
			recContactersSize += sizeof(int);
			recContactersSize += it2->GetSerialBuffSize();
		}

		return sizeof(int) + contactListName.length() + sizeof(int) + contactListId.length() + sizeof(unsigned int) + sizeof(int) + guid.length() + sizeof(int) + sourceId.length() + destUseridSize +sizeof(int) + recContactersSize + sizeof(int);
	}
};

//hostserver收到客户端推荐后，回复给客户端信息
struct RecommendResp
{
	int          status;        // 0表示成功，-1失败
	std::string  guid;          //客户端推荐的guid

	RecommendResp()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;
		memcpy(pbuff+serlen,&status,sizeof(status));
		serlen += sizeof(status);

		int guidlen = guid.length();
		memcpy(pbuff+serlen,&guidlen,sizeof(guidlen));
		serlen += sizeof(guidlen);

		memcpy(pbuff+serlen,guid.c_str(),guidlen);
		serlen += guidlen;

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int unserlen = 0;
		memcpy(&status,pbuff+unserlen,sizeof(status));
		unserlen += sizeof(status);

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserlen,sizeof(guidlen));
		unserlen += sizeof(guidlen);

		guid.assign(pbuff+unserlen,guidlen);
		unserlen += guidlen;

		return unserlen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + sizeof(int) +guid.length();
	}
};

//hostserver发给各个目标推荐人的信息
struct PerUserRecommendMsg:public GMMemAlloc<PerUserRecommendMsg,GMPrivateHeapMemAllocMethod>,public GMRefCounterBase<PerUserRecommendMsg>
{
	std::string           contactListName;    //通讯录名称
	std::string           contactListId;
	unsigned int          contactListVersion;
	std::string           guid;
	std::string			  sourceId;			  //推荐原始人
	std::string           userid;
	std::list<Contact>    contacters;

	PerUserRecommendMsg(){}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

		int contactListNameLen = contactListName.length();
		memcpy(pbuff+serlen,&contactListNameLen,sizeof(contactListNameLen));
		serlen += sizeof(contactListNameLen);

		memcpy(pbuff+serlen,contactListName.c_str(),contactListNameLen);
		serlen += contactListNameLen;

		int contactListIdLen = contactListId.length();
		memcpy(pbuff+serlen,&contactListIdLen,sizeof(contactListIdLen));
		serlen += sizeof(contactListIdLen);

		memcpy(pbuff+serlen,contactListId.c_str(),contactListIdLen);
		serlen += contactListIdLen;

		memcpy(pbuff+serlen,&contactListVersion,sizeof(contactListVersion));
		serlen += sizeof(contactListVersion);

		int guidLen = guid.length();
		memcpy(pbuff+serlen,&guidLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,guid.c_str(),guidLen);
		serlen += guidLen;

		int sourceIdLen = sourceId.length();
		memcpy(pbuff+serlen,&sourceIdLen,sizeof(sourceIdLen));
		serlen += sizeof(sourceIdLen);

		memcpy(pbuff+serlen,sourceId.c_str(),sourceIdLen);
		serlen += sourceIdLen;

		int useridLen = userid.length();
		memcpy(pbuff+serlen,&useridLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,userid.c_str(),useridLen);
		serlen += useridLen;

		int contactersLen = contacters.size();
		memcpy(pbuff+serlen,&contactersLen,sizeof(int));
		serlen += sizeof(int);

		for(std::list<Contact>::iterator it = contacters.begin();it != contacters.end();it++)
		{
			int contactLen = it->GetSerialBuffSize();
			memcpy(pbuff+serlen,&contactLen,sizeof(int));
			serlen += sizeof(int);

			int lserlen = it->Serialize(pbuff+serlen,contactLen);
			serlen += lserlen;
		}

		return serlen;
	}

	int Unserialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize()) 
		{
			return -1;
		}

		int unserLen = 0;

		int contactListNamelen = 0;
		memcpy(&contactListNamelen,pbuff+unserLen,sizeof(contactListNamelen));
		unserLen += sizeof(contactListNamelen);

		contactListName.assign(pbuff+unserLen,contactListNamelen);
		unserLen += contactListNamelen;

		int contactListIdlen = 0;
		memcpy(&contactListIdlen,pbuff+unserLen,sizeof(contactListIdlen));
		unserLen += sizeof(contactListIdlen);

		contactListId.assign(pbuff+unserLen,contactListIdlen);
		unserLen += contactListIdlen;

		memcpy(&contactListVersion,pbuff+unserLen,sizeof(contactListVersion));
		unserLen += sizeof(contactListVersion);

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		int sourceIdLen = 0;
		memcpy(&sourceIdLen,pbuff+unserLen,sizeof(sourceIdLen));
		unserLen += sizeof(int);

		sourceId.assign(pbuff+unserLen,sourceIdLen);
		unserLen += sourceIdLen;

		int useridlen = 0;
		memcpy(&useridlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		userid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int contactersLen = 0;
		memcpy(&contactersLen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		for(int i = 0;i < contactersLen;i++)
		{
			Contact tmp;

			int contactLen = 0;
			memcpy(&contactLen,pbuff+unserLen,sizeof(int));
			unserLen += sizeof(int);

			tmp.Unserialize(pbuff+unserLen,contactLen);
			unserLen += contactLen;

			contacters.push_back(tmp);
		}

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		int contactersSize = 0;
		for (std::list<Contact>::iterator it2 = contacters.begin();it2 != contacters.end();it2++)
		{
			contactersSize += sizeof(int);
			contactersSize += it2->GetSerialBuffSize();
		}

		return sizeof(int) + contactListName.length() + sizeof(int) + contactListId.length() + sizeof(unsigned int) + sizeof(int) + guid.length() + sizeof(int) + sourceId.length() + sizeof(int) + userid.length() + sizeof(int) + contactersSize;
	}
};

//封装智能指针,单个推荐联系人
typedef GMEmbedSmartPtr<PerUserRecommendMsg>   RecommendMsgToPerUserSPTR;

struct NotifyRecommendMsg
{
	std::string userid;        //目标推荐人userid
	std::string guid;  	       //一次请求的guid

	NotifyRecommendMsg()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

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

		userid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + userid.length() + sizeof(int) + guid.length();
	}
};

typedef NotifyRecommendMsg GetRecommendMsg;

//hostagent收到UI推荐回复后，返回给hostserver的信息
struct PerUserRecommendMsgResp
{
	std::string userid;        //目标推荐人userid
	std::string guid;  	       //一次请求的guid
	int         status;        //结果，0表示成功

	PerUserRecommendMsgResp()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

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

		memcpy(pbuff+serlen,&status,sizeof(status));
		serlen += sizeof(status);

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

		userid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		memcpy(&status,pbuff+unserLen,sizeof(status));
		unserLen += sizeof(status);

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + userid.length() + sizeof(int) + guid.length() + sizeof(int);
	}
};


//struct findRecuserCmd
//{
//	findRecuserCmd()
//	{
//		memset(userid,0,12);
//		memset(guid,0,37);
//		memset(hostip,0,16);
//		hostport = 0;
//		m_srcsid = 0;
//	}
//
//	char                   userid[12];
//	char                   guid[37];
//	char                   hostip[16];
//	int                    hostport;
//	int                    m_srcsid;
//	AsynModel::SptrISender m_sender;
//};


struct findRecuserCmd
{
	std::string userid;        //目标推荐人userid
	std::string guid;  	       //一次请求的guid
	std::string hostip;
	int         hostport;

	findRecuserCmd()
	{

	}

	int Serialize(char * pbuff, int len)
	{
		if (NULL == pbuff || len < GetSerialBuffSize())
		{
			return -1;
		}

		int serlen = 0;

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

		int ipLen = hostip.length();
		memcpy(pbuff+serlen,&ipLen,sizeof(int));
		serlen += sizeof(int);

		memcpy(pbuff+serlen,hostip.c_str(),ipLen);
		serlen += ipLen;

		memcpy(pbuff+serlen,&hostport,sizeof(hostport));
		serlen += sizeof(hostport);

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

		userid.assign(pbuff+unserLen,useridlen);
		unserLen += useridlen;

		int guidlen = 0;
		memcpy(&guidlen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		guid.assign(pbuff+unserLen,guidlen);
		unserLen += guidlen;

		int iplen = 0;
		memcpy(&iplen,pbuff+unserLen,sizeof(int));
		unserLen += sizeof(int);

		hostip.assign(pbuff+unserLen,iplen);
		unserLen += iplen;

		memcpy(&hostport,pbuff+unserLen,sizeof(hostport));
		unserLen += sizeof(hostport);

		return unserLen;
	}

	int GetSerialBuffSize()
	{
		return sizeof(int) + userid.length() + sizeof(int) + guid.length() + sizeof(int) + hostip.length() + sizeof(int);
	}
};







