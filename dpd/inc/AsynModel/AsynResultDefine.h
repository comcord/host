/****************************************************************************                                                                          
* AsynResultDefine.h                       
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction ϵͳ��API����ֵ����
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009
#define ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009

namespace AsynModel
{
	/**
	*ϵͳ�з����ķ���ֵ���壬�ɽ����־��Ѱ���⣬��Ҫ��������
	*/
	enum AsynResultRet
	{
		SUCCESS = 0,///<�ɹ�
		
		ASYNMODEL_ALREADY_INIT = 1,///<�첽ģ���Ѿ�����ʼ��
		ASYNMODEL_COMPORTCONTROLER_INIT_ERROR = 2,///<��ɶ˿ڳ�ʼ������
		ASYNMODEL_CREATE_COMPUTEWORKERS_ERROR = 3,///<����ComputeWorkerʱ����
		ASYNMODEL_TCPIOPROCESSOR_INIT_ERROR = 4,///<TCPIOProcessor��ʼ��ʧ��
		ASYNMODEL_UDPIOPROCESSOR_INIT_ERROR = 5,///<UDPIOProcessor��ʼ��ʧ��
		ASYNMODEL_UNINIT = 6,///<�첽ģ��û�г�ʼ��
		
		IP_LENGTH_INVALID = 7,///�����IP���ȷǷ�
		IPPORT_INVALID = 8,///<�����IPPORT�Ƿ�
		MANAGER_INVALID = 9,///<AsynManager��Ч
		SENDER_INVALID = 10,///<Sender��Ч
		UNKNOWN_SENDER_TYPE = 11,///<δ֪��Sender����
		MANAGER_NOT_RUNNING = 12,///<AsynManagerû������
		TCPPROCESSOR_NOT_ENABLE = 13,///<TCPProcessorû������
		MSG_DATAPACKET_TOO_BIG = 14,///<�û����ݹ���
		UNKNOWN_PROTOCOL = 15,///<δ֪��Э������
		//UNKNOWN_PERIO_TYPE,///<δ֪��PerIO����
		MSG_SERALIZE_ERROR = 16,///<Msg ���л�����

		CONNECT_NOT_ESTABLISHED = 17,///<tcp ����û�н���
		POST_USERDATA_ERROR = 18,///<�û�Ͷ��TCP�������ݳ���

		TCPSOCKETITEM_GET_FALSE = 19,///<TCPSocketItem��ȡʧ��
		TCPSOCKETITEM_INVALID = 20,///<TCPSocketItem��Ч
		ASYNCONNECT_POST_FALSE = 21,///<Ͷ���첽����ʧ��
		TCPSOCKET_CANNOT_RECONNECT = 22,///<TCP Socket��������

		CONNECT_ALSO_ESTABLISHED = 23,///<TCP�����Ѿ�������

		DESTSESSION_TERMINATED = 24,///<Ҫ���͵�Ŀ��Session�Ѿ�������

		MEMALLOC_ERROR = 25,///<MemStream����ʧ��
		MSG_UNSERIALIZE_ERROR = 26,///<��Ϣ�����л�ʧ��
		//COMPLETATION_ERROR,///<Ͷ����ɶ˿ڳ���
		NONSUPPORT_CONNID_TYPE = 27,///<��֧�ֵ� ConnID����

		UDP_SENDER_HAS_NOT_INIT = 28,///<UDP Senderû�г�ʼ��
		UDP_SENDER_CONSTRUCT_SOCKET_ERROR = 29,///<UDP Sender����Socketʧ��
		UDP_SENDER_HAS_INITED = 30,///<UDP Sender�Ѿ�����ʼ��
		UDP_SOCKET_ITEM_INVALID = 31,///<UDPSocketItem��Ч
		UDP_SOCKET_ERROR = 32,///<UDP Socket ����
		UDP_ADD_SOCKET_ERROR = 33,///< UDP���Socketʧ��
		UDP_REMOVE_DEFAULTPORT_SUCCESS = 34,///<UDP�Ƴ�Ĭ�϶˿ڳɹ���why ������SUCCESS

		POSTTO_COMPLETATIONPORT_ERROR = 35,///<Ͷ����ɶ˿�ʧ��
		COMPLETATIONPORT_UNKNOW_PERIO_TYPE = 36,///δ֪��PerIO����
		COMPLETATIONPORT_DESTROY = 37,///<��ɶ˿��ѱ�����
		ASYNMODEL_SOCKET_ERROR = 38,///<Socket����
		RECV_PARAM_INVALID = 39,///<���ղ�����Ч
		SEND_PARAM_INVALID = 40,///<���Ͳ�����Ч
		PARAM_INVALID = 41,///������Ч����ô��Ĳ�����Ч�����Ƿ���Ҫ��������һ��

		TCP_RECV_FULL_PACKET = 42,///<TCP���յ������İ�
		TCP_RECV_PART_PACKET = 43,///<TCP���յ���� û��ʹ����TCP_RECV_PART_HEAD_SUCCESS��TCP_RECV_PART_BODY_SUCCESS�����ظ�
		TCP_SEND_FULL_PACKET = 44,///<TCP�����������İ�
		TCP_SEND_PART_PACKET = 45,///<TCP���е�һ���ֱ����� epollʹ��
		TCP_RECV_ERROR = 46,///<TCP�������ݳ���
		//TCP_SEND_ERROR = 47,///<TCP�������ݳ���
		TCP_ERROR_PACKET = 48,///<TCP���
		TCP_SOCKET_ERROR=49,///<TCP Socket����
		PERIO_TYPE_ERROR=50,///<��TCP�н��յ���TCP��PerIO��������
		TCP_RECV_PART_HEAD_SUCCESS = 51,///<�ɹ�������һ���ֵ�TCPͷ
		TCP_RECV_PART_BODY_SUCCESS = 52,///<�ɹ�������һ���ֵ�TCP��
		TCP_ADDDEFAULTLISTEN_FAILURE = 53,///<���TCPĬ�ϼ���ʧ��
		TCP_ADD_LISTEN_IP_PORT_ALREADYEXIST = 54,///<TCP��Ӽ���ʧ�ܣ������IP��Port�Ѿ�������
		TCP_CREATE_SOCKET_ERROR = 55,///<����TCPSocket����
		TCP_CREATE_TCPSOCKETITEM_FAILURE = 56,///<����TCPSocketItemʧ��
		TCP_BIND_LISTEN_FAILURE = 57,///<��Listenʧ��
		TCP_BIND_COMPLETEPORT_FAILURE = 58,///<TCP Socket����ɶ˿ڰ�ʧ��
		TCP_POST_ACCEPT_FAILURE = 59,///<TCP Ͷ��acceptʧ��
		TCP_REMOVE_LISTEN_IP_PORT_FAILURE = 60,///<�Ƴ�Ĭ�ϵ�TCP����ʧ��
		TCP_USER_CANCEL_DEFAULT_LISTEN =61,///<�û�ȡ��Ĭ�ϼ��� û��ʹ��
		TCP_REMOVE_DEFAULT_LISTEN_SUCCESS = 62,///<�Ƴ�Ĭ�ϵ�TCP�����ɹ�
		TCP_ERROR_PACKET_SIZE = 63,///<�����TCP����С

		NEW_OBJECT_ERROR = 64,///<new ʵ��ʧ��
		UNKNOWN_ERROR = 65,///<λ�ô���
		TCP_SEND_PACKET_TIMEOUT = 66, ///<�����ݷ��ͳ�ʱ����

		INSTANT_COMPLET = 67, //����������perio�����꣬���п����ǽ�����һ�μ�������
		POST_MSG_FAILED = 68,
		FATAL_ERROR = 69,
		SYSTEM_BUSY = 70, //ϵͳ��æ
		WSA_STARTUP_ERROR = 71, //WsaStartup����
		UDP_ADD_PORT_ALREADYEXIST = 72,///< UDP��Ӷ˿�ʧ�ܣ������IP��Port�Ѿ�����
	};
}
#endif //ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009
