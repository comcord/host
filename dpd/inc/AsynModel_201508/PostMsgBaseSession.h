/****************************************************************************
* TCPSender.h                       
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction ��װSession ������Ϣ���߼�
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef GF_POSTMSG_BASESESSION_H_2008_GOUMANG
#define GF_POSTMSG_BASESESSION_H_2008_GOUMANG

#include "AsynResultDefine.h"
#include "AsynModelDataDefine.h"
#include "LinkBuff.h"
#include "ConnID.h"
#include "ISender.h"

#define SENMSGDEFAULTTIMROUT	0xFFFFFFFF

namespace AsynModel
{
	class PostMsgContext;
	class AsynManager;
	class Session;

	/**
	* Session �̳д��࣬��װ������Ϣ���߼�
	*/
	class PostMsgBaseSession
	{
	public:
		/**
		* ���� ��ȡ����Sessionָ�룬�ڷ�����Ϣʱ��Ҫ֪������Session��ָ���Է�װSession����ָ��
		*      ������Sessionʵ��
		* @param ��
		* @return Sessionָ������ 
		*/
		virtual Session* GetSession() = 0;
		/**
		* ���� ���AsynManager�Ƿ�������״̬�����򷵻���ָ��
		* @param result AsynResultRet���ʹ������� ��֪�ɹ���ʧ��ԭ��
		* @return AsynManager����ָ��
		*/
		AsynManager* CheckAsynModelStatus(AsynResultRet& result);

		/**
		* ���� ��ʹ��Sender�ı�����Ϣ���� 
		* @param destSid U32���� Ŀ��Session��ID��
		* @param msgid  U32���� ���͵�msg ID��
		* @param msgData charָ������ ���͵���Ϣ�����׵�ַ
		* @param dataLen U32���� ������Ϣ�����ݳ���
		* @param isShareMsgData bool���� Ĭ��Ϊfalse ָ���Ƿ���Ҫ��������
		* ��������ֱ��ʹ���û������msgData���� �û��ڴ����ڼ䲻��ɾ��msgData�����������û�msgData����ʹ�� �û��ڵ��øú������������ɾ��msgData��
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
		AsynResultRet PostMsg(U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isShareMsgData = false);

		AsynResultRet PostMsg(U32 destSid,U32 msgid,
							   UserDataInfo* msgData);
		#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender, U32 destSid, U32 msgid,
								UserDataInfo* msgData);
		#else
		AsynResultRet PostMsg(ISender* sender, U32 destSid, U32 msgid,
								UserDataInfo* msgData);	
		#endif
		/**
		* ���� ʹ��Sender�ı�����Ϣ����
		* @param sender SptrISender����ָ������ 
		* @param destSid U32���� Ŀ��Session��ID��
		* @param msgid  U32���� ���͵�msg ID��
		* @param isShareMsgData bool���� Ĭ��Ϊfalse ָ���Ƿ���Ҫ��������
		* @param msgData charָ������ ���͵���Ϣ�����׵�ַ
		* @param dataLen U32���� ������Ϣ�����ݳ���
		* ��������ֱ��ʹ���û������msgData���� �û��ڴ����ڼ䲻��ɾ��msgData�����������û�msgData����ʹ�� �û��ڵ��øú������������ɾ��msgData�� 
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender, U32 destSid, U32 msgid,bool isShareMsgData,
							  char* msgData,U32 dataLen);
#else
		AsynResultRet PostMsg(ISender* sender, U32 destSid, U32 msgid,bool isShareMsgData,
							  char* msgData, U32 dataLen);
#endif


		/**
		* ���� Զ����Ϣ���� ʹ��ConnID��ʽ�������û������msgData��char*����
		* @param connID ConnID���� ����ID
		* @param destSid U32���� Ŀ���Session ID
		* @param msgid U32���� ��ϢID
		* @param msgData char����ָ�� ��Ϣ�����׵�ַ
		* @param dataLen U32���� ��Ϣ���ݳ���
		* @param isReliable bool���� �Ƿ�ɿ�
		* @param timeout U32���� ��ʱʱ�䣬���ͳ�����ʱ�佫�������ʱ�¼��ص��û����û������˷���ʧ�ܣ�.�������Ĭ�ϵ�SENMSGDEFAULTTIMROUTʱ�����UDP�ɿ����ͣ�ϵͳ������ط�5�Σ���Լ��ʱ��Ϊ3��
		* @param subscribeMsg SubScribeMsgDefine���� �ص��¼����ģ�Ĭ�ϲ�����
		* @see SubScribeMsgDefine���Ͷ���
		* @param protocal Protocol���� ���͵�Э������
		* @see Protocol���Ͷ���
		* @param userPara UserDataInfo����ָ�� �û��Զ���Ļص����������Session���ٽ����漰���ͷ����⣩
		* @param localIP unsigned long���� ָ�����͵ı���IP��������
		* @param localPort unsigned short���� ָ�����͵ı���Port��������
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
		AsynResultRet PostMsg(ConnID& connID,U32 destSid,U32 msgid, 
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  Protocol protocal = ANYONE,
							  UserDataInfo* userPara = NULL,
							  unsigned long localIP = 0,
							  unsigned short localPort = 0);
		/**
		* ���� Զ����Ϣ���� ʹ��ConnID��ʽ�������û������msgData��LinkBuff_SmartPtr����
		* @param connID ConnID���� ����ID
		* @param destSid U32���� Ŀ���Session ID
		* @param msgid U32���� ��ϢID
		* @param msgData LinkBuff_SmartPtr��������ָ�� ��Ϣ����ʹ��Linkbuff��װ
		* @param isReliable bool���� �Ƿ�ɿ�
		* @param timeout U32���� ��ʱʱ�䣬���ͳ�����ʱ�佫�������ʱ�¼��ص��û����û������˷���ʧ�ܣ�..�������Ĭ�ϵ�SENMSGDEFAULTTIMROUTʱ�����UDP�ɿ����ͣ�ϵͳ������ط�5�Σ���Լ��ʱ��Ϊ3��
		* @param subscribeMsg SubScribeMsgDefine���� �ص��¼����ģ�Ĭ�ϲ�����
		* @see SubScribeMsgDefine���Ͷ���
		* @param protocal Protocol���� ���͵�Э������
		* @see Protocol���Ͷ���
		* @param userPara UserDataInfo����ָ�� �û��Զ���Ļص����������Session���ٽ����漰���ͷ����⣩
		* @param localIP unsigned long���� ָ�����͵ı���IP��������
		* @param localPort unsigned short���� ָ�����͵ı���Port��������
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
		AsynResultRet PostMsg(ConnID& connID, U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData, 
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								Protocol protocal = ANYONE,
								UserDataInfo* userPara = NULL,
								unsigned long localIP = 0,
								unsigned short localPort = 0);

	
		/**
		* ���� Զ����Ϣ���� ʹ��Sender��ʽ�������û������msgData��char*����
		* @param sender SptrISender����ָ�� 
		* @param destSid U32���� Ŀ���Session ID
		* @param msgid U32���� ��ϢID
		* @param msgData char����ָ�� ��Ϣ�����׵�ַ
		* @param dataLen U32���� ��Ϣ���ݳ���
		* @param isReliable bool���� �Ƿ�ɿ�
		* @param timeout U32���� ��ʱʱ�䣬���ͳ�����ʱ�佫�������ʱ�¼��ص��û����û������˷���ʧ�ܣ�..�������Ĭ�ϵ�SENMSGDEFAULTTIMROUTʱ�����UDP�ɿ����ͣ�ϵͳ������ط�5�Σ���Լ��ʱ��Ϊ3��
		* @param subscribeMsg SubScribeMsgDefine���� �ص��¼����ģ�Ĭ�ϲ�����
		* @see SubScribeMsgDefine���Ͷ���
		* @see Protocol���Ͷ���
		* @param userPara UserDataInfo����ָ�� �û��Զ���Ļص����������Session���ٽ����漰���ͷ����⣩
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender,U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  UserDataInfo* userPara = NULL);
#else
		AsynResultRet PostMsg(ISender* sender,U32 destSid,U32 msgid,
							  char* msgData,U32 dataLen,
							  bool isReliable,
							  U32 timeout = SENMSGDEFAULTTIMROUT,
							  SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
							  UserDataInfo* userPara = NULL);
#endif
		/**
		* ���� Զ����Ϣ���� ʹ��sender��ʽ�������û������msgData��LinkBuff_SmartPtr����
		* @param sender SptrISender����ָ�� 
		* @param destSid U32���� Ŀ���Session ID
		* @param msgid U32���� ��ϢID
		* @param msgData LinkBuff_SmartPtr��������ָ�� ��Ϣ����ʹ��Linkbuff��װ
		* @param isReliable bool���� �Ƿ�ɿ�
		* @param timeout U32���� ��ʱʱ�䣬���ͳ�����ʱ�佫�������ʱ�¼��ص��û����û������˷���ʧ�ܣ�..�������Ĭ�ϵ�SENMSGDEFAULTTIMROUTʱ�����UDP�ɿ����ͣ�ϵͳ������ط�5�Σ���Լ��ʱ��Ϊ3��
		* @param subscribeMsg SubScribeMsgDefine���� �ص��¼����ģ�Ĭ�ϲ�����
		* @see SubScribeMsgDefine���Ͷ���
		* @see Protocol���Ͷ���
		* @param userPara UserDataInfo����ָ�� �û��Զ���Ļص����������Session���ٽ����漰���ͷ����⣩
		* @return AsynResultRet���� �����ɹ�����ʧ��ԭ��
		*/
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet PostMsg(SptrISender& sender,U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData,
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								UserDataInfo* userPara = NULL);
#else
		AsynResultRet PostMsg(ISender* sender,U32 destSid,U32 msgid,
								LinkBuff_SmartPtr& msgData,
								bool isReliable,
								U32 timeout = SENMSGDEFAULTTIMROUT,
								SubScribeMsgDefine subscribeMsg = POSTMSG_NO_SUBSCRIBE,
								UserDataInfo* userPara = NULL);
#endif
		AsynResultRet SendDataStream(ConnID& connID,
									 uint32 destSid,
									 uint32 msgid,
									 LinkBuff_SmartPtr& msgData,
									 unsigned long localIP,
									 unsigned short localPort,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//�����ӿڣ�ֻ֧��TCP����

//#ifdef WIN32
#ifndef ASYNMODEL_NOTUSESPTRAPT
		AsynResultRet SendDataStream(SptrISender& sender,
									 uint32 destSid,
									 uint32 msgid,
									 LinkBuff_SmartPtr& msgData,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//�����ӿڣ�ֻ֧��TCP����
#else
		AsynResultRet SendDataStream(ISender* sender,
									 uint32 destSid,
									 uint32 msgid,
								     LinkBuff_SmartPtr& msgData,
									 uint32 timeout = SENMSGDEFAULTTIMROUT,
									 SubScribeMsgDefine subscribeMsg = POSTMSG_ALL,
									 UserDataInfo* userPara = NULL);//�����ӿڣ�ֻ֧��TCP����
#endif
//#endif
	
	private:

		AsynResultRet PostMsg(SptrISender& destSession,PostMsgContext* context);

	};
}

#endif //GF_POSTMSG_BASESESSION_H_2008_GOUMANG
