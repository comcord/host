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
		 * 消息预处理函数
		 *
		 * 输入参数:
		 *		destSid			目标Session ID
		 *		srcSid			源Session ID
		 *		msgID			msg类型ID
		 *		pbuff			需要预处理的buff
		 *		len				buff的长度
		 *		sender			发送者sender
		 *		sptrSession		如果需要创建新的Session则将其指针包装成智能指针返回
		 * 
		 * 输出参数:
		 *		
		 * 返回值:
		 *		-1		错误数据
		 *		 0		数据可传入到下一个预处理器
		 *		 1		数据已经成功预处理，不需要要传递到下一个预处理器
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
