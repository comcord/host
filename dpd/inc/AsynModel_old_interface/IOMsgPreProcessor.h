/****************************************************************************
*                                                                           *
* IOMsgCollector.h --	constant and macros definitions	                    *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/
#ifndef GF_IOMSGCOLLECTOR_H_2008_GOUMANG
#define GF_IOMSGCOLLECTOR_H_2008_GOUMANG

#include "ISender.h"
#include "Session.h"

namespace AsynModel
{
	#define BEGIN_CREATESESSION_MAP()\
	public:\
		virtual int PreProc( unsigned int destSid,\
							unsigned int srcSid,\
							unsigned int msgID,\
							char * pbuff, \
							const int len, \
							AsynModel::SptrISender sender,\
							AsynModel::SptrSession& ppSession)\
		{\

	#define CREATESESSION_HANDLER(msgID, func)\
		if(msgid == msgID)\
		{\
		return func(destSid,srcSid,msgID,pbuff,len,sender,ppSession);\
		}

	#define END_CREATESESSION_MAP()\
		return -1;\
		}

	#define CreateSessionRegist(pCreateSessionProcessInstance)   		AsynModel::RegistIOPreProcessor(CreateSessionProcessInstance)
	#define CreateSessionUnRegist(pCreateSessionProcessInstance)   		AsynModel::UnRegistIOPreProcessor(pCreateSessionProcessInstance)

	class IOMsgPreProcessor
	{
	public:
	
		/* PreProc()
		 * ��ϢԤ������
		 *
		 * �������:
		 *		destSid			Ŀ��Session ID
		 *		srcSid			ԴSession ID
		 *		msgID			msg����ID
		 *		pbuff			��ҪԤ�����buff
		 *		len				buff�ĳ���
		 *		sender			������sender
		 *		sptrSession		�����Ҫ�����µ�Session����ָ���װ������ָ�뷵��
		 * 
		 * �������:
		 *		
		 * ����ֵ:
		 *		-1		��������
		 *		 0		���ݿɴ��뵽��һ��Ԥ������
		 *		 1		�����Ѿ��ɹ�Ԥ��������ҪҪ���ݵ���һ��Ԥ������
		 */
		virtual int PreProc(unsigned int destSid,
							unsigned int srcSid,
							unsigned int msgID,
							char * pbuff, 
							const int len, 
							AsynModel::SptrISender sender,
							AsynModel::SptrSession& sptrSession) = 0;

	};
}
#endif
