/****************************************************************************
*                                                                           *
* ConnID.h --	constant and macros definitions	                    *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/
//GF��ʾGridFramework����д
//�淶 GF_����_H_GOUMANG_2008

#ifndef GF_CONNID_H_GOUMANG_2008_
#define GF_CONNID_H_GOUMANG_2008_

#include <GMUInt.h>
#include <string.h>
//#include "ISender.h"

namespace AsynModel
{
#define MAX_PUNCH_ID_SIZE 128
	/*
	*	���ӵ�ID
	*/
	struct ConnID
	{
		struct OPENINFO
		{
			char openUDPIP[16];
			char openTCPIP[16];
			U16 openUdpPort;
			U16 openTcpPort;
			char pad[MAX_PUNCH_ID_SIZE+2 - (16 + 16 + 2 + 2)];
		};

		struct PUNCHINFO
		{
			U16 punchidlen;
			char punchid[MAX_PUNCH_ID_SIZE];
		};

		enum hosttype
		{
			CONNID_OPEN_ = 0,
			CONNID_NEEDPUNCH,
		};

		ConnID();
		ConnID(const ConnID & connid);
		ConnID & operator = (const ConnID & connid);

		U32 hosttype;//�������ͣ������Ƿ����
		union
		{
			struct OPENINFO openhost;
			struct PUNCHINFO punchinfo;
		};
		/*
		�ж�����ID�Ƿ���Ч
		*/
		bool IsValid() const ;
		/*-
		>0����ֵΪconnid�����л���ĳ��ȣ������Ե��ú���ó��ȵĻ����������л�
		-1 ����ID�ṹ�д���
		*/
		int GetConnIdLen();
		/*-
		����ֵ��
		-1 ��������л�buff�ĳ��Ȳ���
		-2 ���л�ʱ��������ID�ṹ����
		>0 ��ʵ���л������ݳ���
		*/
		int SeralizeConnId(char * buff,int len);
		/*-
		����ֵ��
		-1 ����ķ����л�buff�ĳ��Ȳ���
		-2 ���л�ʱ��������ID�ṹ����
		>0 ��ʵ�����л������ݳ���
		*/
		int UnSeralizeConnId(const char * buff,int len);

		::std::string ToString() const ;
	};

inline ConnID::ConnID()
{
	hosttype = CONNID_OPEN_;
	memset(&openhost,0, sizeof(openhost));
}

inline ConnID::ConnID( const ConnID & connid)
{
	if(&connid != this)
	{
		hosttype = connid.hosttype;
		openhost = connid.openhost;
	}
	//memcpy(punchinfo.punchid, connid.punchinfo.punchid,MAX_PUNCH_ID_SIZE);
}

inline ConnID & ConnID::operator = (const ConnID & connid)
{
	if(&connid != this)
	{
		hosttype = connid.hosttype;
		openhost = connid.openhost;
		//memcpy(punchinfo.punchid, connid.punchinfo.punchid,MAX_PUNCH_ID_SIZE);
	}
	return *this;
}

inline bool ConnID::IsValid() const 
{
	if(hosttype == CONNID_OPEN_)
	{
		if((ISIPVALID(openhost.openUDPIP) && (openhost.openUdpPort != 0))
			|| ( ISIPVALID(openhost.openTCPIP) && (openhost.openTcpPort != 0)))
			return true;
	}
	else if (hosttype == CONNID_NEEDPUNCH)
	{
		return (punchinfo.punchidlen > 0);
	}
	
	return false;
}

inline int ConnID::GetConnIdLen()
{
	if(hosttype == CONNID_OPEN_)			
		return (4 + 4 + 4 + 2 + 2);

	else if(hosttype == CONNID_NEEDPUNCH)
		return (4 +2+  punchinfo.punchidlen);
	else
		return -1;
}

inline int ConnID::SeralizeConnId(char * pbuff,int len)
{
	if ( pbuff == NULL )
		return -1;
	if(hosttype == CONNID_OPEN_)
	{
		if(len < 16)
			return -1;

		int ptrMove = 0;
		memcpy(pbuff,&hosttype,4);
		ptrMove += 4;

		unsigned long tempip = inet_addr(openhost.openUDPIP);
		memcpy(pbuff+ptrMove,&tempip,4);
		ptrMove += 4;

		tempip = inet_addr(openhost.openTCPIP);
		memcpy(pbuff+ptrMove,&tempip,4);
		ptrMove += 4;

		memcpy(pbuff+ptrMove,&(openhost.openUdpPort),2);
		ptrMove += 2;

		memcpy(pbuff+ptrMove,&(openhost.openTcpPort),2);
		ptrMove += 2;

		return ptrMove;
	}
	else if(hosttype == CONNID_NEEDPUNCH)
	{
		if(len < 4 + 2 + punchinfo.punchidlen)
			return -1;

		int ptrMove = 0;
		memcpy(pbuff,&hosttype,4);
		ptrMove += 4;

		memcpy(pbuff+ptrMove, &(punchinfo.punchidlen),2);
		ptrMove+=2;

		memcpy(pbuff+ptrMove, punchinfo.punchid,punchinfo.punchidlen);
		ptrMove += punchinfo.punchidlen;
		return ptrMove;
	}
	else
		return -2;
}

inline int ConnID::UnSeralizeConnId(const char * buff,int len)
{
	if(len < 4)
		return -1;

	int ptrMove = 0;
	memcpy(&hosttype,buff,4);
	ptrMove = 4;

	if(hosttype == CONNID_OPEN_)
	{
		if(len < 16)
			return -1;

		in_addr tempaddr;

		memcpy(&(tempaddr.s_addr) ,buff+ptrMove,4);
		ptrMove += 4; 
		memcpy(openhost.openUDPIP, inet_ntoa(tempaddr),16);

		memset(&tempaddr,0,sizeof(in_addr));
		memcpy(&(tempaddr.s_addr) ,buff+ptrMove,4);
		ptrMove += 4; 
		memcpy(openhost.openTCPIP,inet_ntoa(tempaddr),16);

		memcpy( &(openhost.openUdpPort),buff+ptrMove,2);
		ptrMove += 2;

		memcpy( &(openhost.openTcpPort),buff+ptrMove,2);
		ptrMove += 2;

		return ptrMove;
		
	}
	else if(hosttype == CONNID_NEEDPUNCH)
	{
		if(len <  4 + 2)
			return -1;

		memcpy(&(punchinfo.punchidlen), (buff+ptrMove), 2);
		punchinfo.punchidlen = gmmin(punchinfo.punchidlen, MAX_PUNCH_ID_SIZE);
		ptrMove += 2;

		if(len <  4 + 2 + punchinfo.punchidlen)
			return -1;

		memcpy( punchinfo.punchid,  buff+ptrMove,  punchinfo.punchidlen);
		ptrMove += punchinfo.punchidlen;
		return ptrMove;
	}
	else
	{
		return -2;
	}
}

inline std::string ConnID::ToString() const 
{
	char buff[256] = {0};
	if(hosttype == CONNID_OPEN_)
	{
		_snprintf(buff,255,"HostType: OPEN UDP IP: %s  OPEN UDP IP: %s  UDPPort: %u  TCPPort: %u"
			, openhost.openUDPIP,openhost.openTCPIP,openhost.openUdpPort,openhost.openTcpPort);
	}
	else if (hosttype == CONNID_NEEDPUNCH)
	{
		_snprintf(buff,255,"HostType: NeedPunch");
	}
	return std::string( buff);
}
}
#endif//GF_CONNID_H_GOUMANG_2008
