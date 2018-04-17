/*******************************************************************
* Copyright (C), 2008-2011,GOUMANG Corporation. All rights reserved.
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction �첽ģ�Ϳ��ƽӿڷ���
* @email   liuju@goumang.com
* @warning ���̰߳�ȫ
* �첽ģ��С��  2009/2/13     2.2     ���� 
*******************************************************************/
#ifndef ASYNMODEL_ASYNMODELPUB_H_GOUMANG_2009
#define ASYNMODEL_ASYNMODELPUB_H_GOUMANG_2009

#include "IOMsgPreProcessor.h"
#include "AsynResultDefine.h"
#include "AsynModelDataDefine.h"
#include "SubscribeEventInterface.h"


enum AsynModelSystemStatus
{
	AsynModel_System_Distroy = 0,	/**< enum value �첽ģ������. */ 
	AsynModel_System_Inited,		///<�첽ģ�ͳ�ʼ��
	AsynModel_System_Running,		///<�첽ģ������
	AsynModel_System_Stoping,		///<�첽ģ������ֹͣ
	asynModel_System_Starting		///<�첽ģ����������
};
/**
/**
* SetLocalIPV6Addr() ����ipv6��ַ
* @param sIP ipv6��ַ
* @return ��
*/
void SetLocalIPV6Addr(const char* sIP);
/**
/**
* clearLocalIPV6Addr() ����첽ģ�͵�ַ
* @param ��
* @return ��
*/
void clearLocalIPV6Addr();


/**
* InitAsynModel() �첽ģ�ͳ�ʼ��
* @param threadCount �����߳��������Ϊ0ʹ��Ĭ�ϵ��߳�����������õ�ֵ���������ϵͳ���������߳�������ǰʹ�õ�����߳���Ϊ50.
* @return AsynResultRet ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet InitAsynModel(unsigned int threadCount = 0);

/** 
* StartAsynModel() InitAsynModel()����ô˷��������첽���
* @param ��
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet StartAsynModel();

/** 
* StopAsynModel() ֹͣ�첽��ܣ�ȡ�����еļ���Tcp�˿ڣ�Udp��Tcp Socket
* @param ��
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet StopAsynModel();

/** 
* ReleaseAsynModel() ͬ�������첽��ܣ�����InitAsynModel()������Ϸ�������
* @param ��
* @return ��
*/
void ReleaseAsynModel();

/** 
* GetConnID() ��ȡConnID��
* @param ��
* @return ����ConnID����ʵ��
*/
AsynModel::ConnID GetConnID();

/** 
* GetAsynModelStatus() ��ȡ�첽ģ�͵�ǰ״̬��
* @param ��
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModelSystemStatus GetAsynModelStatus();

/** 
* AddListenPort() ���TCP����
* @param ip char[16]
* @param port U16
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet AddListenPort(char ip[16],U16 port);

/** 
* RemoveListenPort() ɾ��TCP����
* @param ip char[16] 
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet RemoveListenPort(char ip[16],U16 port);

void RemoveAllConnectSocket();

/** 
* AddListenPort() ���TCP����
* @param ip unsigned long  ������
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet AddListenPort(unsigned long ip,U16 port);

/** 
* RemoveListenPort() ɾ��TCP����
* @param ip unsigned long  ������
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet RemoveListenPort(unsigned long ip,U16 port);

/** 
* AddUDPPort() ��ӱ���UDP��ض˿�
* @param ip ip[16]  
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ��������� ��
			SUCCESS��
			UDP_ADD_PORT_ALREADYEXIST��
			UDP_ADD_SOCKET_ERROR
*/
AsynModel::AsynResultRet AddUDPPort(char ip[16],U16 port);

/** 
* RemoveUDPPort() ɾ������UDP��ض˿�
* @param ip ip[16]  
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet RemoveUDPPort(char ip[16],U16 port);

/** 
* AddUDPPort() ��ӱ���UDP��ض˿�
* @param ip unsigned long  ������
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet AddUDPPort(unsigned long ip ,U16 port);

/** 
* RemoveUDPPort() ɾ������UDP��ض˿�
* @param ip unsigned long  ������
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet RemoveUDPPort(unsigned long ip, U16 port);

#ifdef __IOS__
/**
 * SetDefaultLocalUdpAddr() ����Ĭ�ϵ�TCP���ؼ���
 * @param ip char[16]
 * @param port U16 ������
 * @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
 */

AsynModel::AsynResultRet SetDefaultLocalUdpAddr(char ip[16], U16 port,bool IsAdd = true);

/**
 * SetDefaultLocalTcpAddr() ����Ĭ�ϵ�UDP���ؼ�ض˿�
 * @param ip char[16]
 * @param port U16 ������
 * @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
 */
AsynModel::AsynResultRet SetDefaultLocalTcpAddr(char ip[16], U16 port,bool IsAdd = true);
#else
/** 
* SetDefaultLocalUdpAddr() ����Ĭ�ϵ�TCP���ؼ���
* @param ip char[16] 
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet SetDefaultLocalUdpAddr(char ip[16], U16 port);

/** 
* SetDefaultLocalTcpAddr() ����Ĭ�ϵ�UDP���ؼ�ض˿�
* @param ip char[16] 
* @param port U16 ������
* @return ����AsynResultRet���͸�֪�Ƿ�ɹ��Լ���������
*/
AsynModel::AsynResultRet SetDefaultLocalTcpAddr(char ip[16], U16 port);
#endif

/** 
* RegistIOPreProcessor() ע��Ԥ������,process �ⲿά��
* @param processor synModel::IOMsgPreProcessor����ָ�� 
* @return bool���͸�֪�Ƿ�ɹ�
*/
bool RegistIOPreProcessor(AsynModel::IOMsgPreProcessor *processor);

/** 
* UnregistIOPreProcessor() ע��Ԥ������,process �ⲿά��
* @param processor synModel::IOMsgPreProcessor����ָ�� 
* @return bool���͸�֪�Ƿ�ɹ�
*/
bool UnregistIOPreProcessor(AsynModel::IOMsgPreProcessor *processor);

#ifdef __IOS__

/**
 * RegistEventCallBackInterface() ע���첽����¼��۲��ߣ�ֻ����һ��
 * @param SubScribeEventCallBack����ָ��
 * @return ��
 */
void RegistEventCallBackInterface(char*localIp,unsigned short port,AsynModel::SocketType type,AsynModel::SubScribeEventCallBack * pCallback);

void RegistEventCallBackInterface_New(AsynModel::SubScribeEventCallBack * pCallback);

void UnregistEventCallBackInterface_New(AsynModel::SubScribeEventCallBack * pCallback);

#endif
/**
 * RegistEventCallBackInterface() ע���첽����¼��۲��ߣ�ֻ����һ��
 * @param SubScribeEventCallBack����ָ��
 * @return ��
 */
void RegistEventCallBackInterface(AsynModel::SubScribeEventCallBack * pCallback);


/**
* UnregistEventCallBackInterface() ע���첽����¼��۲���
* @param ��
* @return ��
*/
void UnregistEventCallBackInterface();

/** 
* PostMsg() ��Session�ⲿͶ����Ϣ��Session
* @param destSid U32���� Ŀ��SessionID
* @param msgid U32���� ���͵���ϢID
* @param msgData char����ָ�� ���͵���Ϣ����
* @param dataLen U32����  ���͵���Ϣ����
* @param isShareMsgData bool���� �Ƿ���Ҫ�����ڴ洫�ݣ�Ĭ��Ϊ��
* @return bool���͸�֪�Ƿ�ɹ�
*/
AsynModel::AsynResultRet PostMsg(U32 destSid,U32 msgid,char* msgData,U32 dataLen,
								 bool isShareMsgData = false);

/** 
* PostMsg() ��Session�ⲿͶ��������Ϣ��Session
* @param sid const U32���� Ŀ��SessionID
* @return bool���͸�֪�Ƿ�ɹ�
*/
AsynModel::AsynResultRet PostCancelSessionEvent(const U32 sid);


/** 
* GetFluxStatInfo() ��ȡ����ͳ����Ϣ,ÿ��ȡһ���ڲ������������һ��
* @param sendUdpPacketNum  U32���� ���͵�UDP������
* @param sendUdpPacketSize U32���� ���͵�UDP����С
* @param recvUdpPacketNum  U32���� ���յ�UDP������
* @param recvUdpPacketSize U32���� ���յ�UDP����С
* @param sendTcpPacketNum  U32���� ���͵�Tcp������
* @param sendTcpPacketSize U32���� ���͵�Tcp����С
* @param recvTcpPacketNum  U32���� ���յ�Tcp������
* @param recvTcpPacketSize U32���� ���յ�Tcp����С
* @return ��
*/
#ifdef WIN32
void GetFluxStatInfo(U32& sendUdpPacketNum, U32& sendUdpPacketSize, 
					 U32& recvUdpPacketNum, U32& recvUdpPacketSize,
					 U32& sendTcpPacketNum, U32& sendTcpPacketSize, 
					 U32& recvTcpPacketNum, U32& recvTcpPacketSize );
#endif

#ifdef __IOS__

AsynModel::AsynResultRet GetDefaultLocalUdpAddr(unsigned long &ip, U16& port);

AsynModel::AsynResultRet GetDefaultLocalTcpAddr(unsigned long &ip, U16& port);

void AsynModelRemoveAllConnectSocket();
void AsynModelRemoveAllConnectSocketUDP();
#endif

void addrIPv4ToIPv6(sockaddr_in& ipv4, sockaddr_in6& ipv6, int proto = SOCK_DGRAM);

std::string GetLocalIPV6Addr();
extern bool g_bIPV6;
extern const U32 g_ulocal_ipv4 ;

#endif //ASYNMODEL_ASYNMODELPUB_H_GOUMANG_2009
