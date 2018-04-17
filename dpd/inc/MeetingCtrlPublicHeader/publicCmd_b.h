#pragma once

#define    MEETING_ID_BREAK_CHAR    "*"
#define    ENTERPRISE_ID_BREAK_CHAR   "%"
#define    MEETING_SPEAKER_BREAK_CHAR    "@"
#define    MEETING_ADMIN_STANDARD_BREAK_CHAR    "_"


//test chengyf
//�������ڣ���λms
#define   HEART_BEAT_TIME_SPAN    (1*1000)

//udp������
#define    UDP_MAX_PACKET_LEN   (50*1024)

//�������˷����ͻ��˵���ʾmsgId��������sessionͶ�ݵģ�ҵ��ID���������棬��Ҫ����
#define    SERVER_RESP_CMD             10000

//����
#define    HEART_BEAT_REQ_CMD         6000
#define    HEART_BEAT_RESP_CMD        6001

/*******************************************************************/
/*******************��ͨ�λ���Ա�������Ա��������******************/
/*******************************************************************/

//�λ�����
#define    ATTEND_MEETING_REQ_CMD      7001
#define    ATTEND_MEETING_RESP_CMD     7002

//�˳�����
#define    QUIT_MEETING_REQ_CMD        7003
#define    QUIT_MEETING_RESP_CMD       7004

//������֪ͨ����ռ
#define    SERVER_NOTIFY_GRAB_CMD      7005


/*******************************************************************/
/*************************��ͨ�λ���Ա �������*********************/
/*******************************************************************/

//��ͨ�λ���Ա�㱨״̬
#define    REPORT_STATUS_REQ_CMD            8001
#define    REPORT_STATUS_RESP_CMD           8002

//���뷢��
#define    APPLY_SPEAK_REQ_CMD              8007
#define    APPLY_SPEAK_RESP_CMD             8008

//ȡ�����뷢��
#define    CANCEL_APPLY_SPEAK_REQ_CMD       8009
#define    CANCEL_APPLY_SPEAK_RESP_CMD      8010

//��������
#define    TEMP_LEAVE_REQ_CMD               8011
#define    TEMP_LEAVE_RESP_CMD              8012

//ȡ����������
#define    CANCEL_TEMP_LEAVE_REQ_CMD        8013
#define    CANCEL_TEMP_LEAVE_RESP_CMD       8014


//������֪ͨ���鿪ʼ
#define    SERVER_NOTIFY_MEETING_START_CMD           8015
#define    SERVER_NOTIFY_MEETING_START_RESP_CMD      8016

//������֪ͨ����ֹͣ
#define    SERVER_NOTIFY_MEETING_STOP_CMD            8017
#define    SERVER_NOTIFY_MEETING_STOP_RESP_CMD       8018

//������֪ͨ�ͻ��˿�ʼ����
#define    SERVER_NOTIFY_START_SPEAK_CMD             8019
#define    SERVER_NOTIFY_START_SPEAK_RESP_CMD        8020

//������֪ͨ�ͻ���ֹͣ����
#define    SERVER_NOTIFY_STOP_SPEAK_CMD              8021
#define    SERVER_NOTIFY_STOP_SPEAK_RESP_CMD         8022

//������֪ͨ�л��᳡��ʾģʽ
#define    SERVER_NOTIFY_SHOW_MODE_CHANGE_CMD           8023
#define    SERVER_NOTIFY_SHOW_MODE_CHANGE_RESP_CMD      8024

//������֪ͨ�·����߲���
#define    SERVER_NOTIFY_DECIDE_SPEAKER_CMD             8025
#define    SERVER_NOTIFY_DECIDE_SPEAKER_RESP_CMD        8026

//������֪ͨ���з�����ʧЧ
#define    SERVER_NOTIFY_CANCEL_SPEAKER_CMD             8027
#define    SERVER_NOTIFY_CANCEL_SPEAKER_RESP_CMD        8028


/*******************************************************************/
/*****************************������Ա �������*********************/
/*******************************************************************/

//������Ա�㱨״̬
#define    MANAGER_REPORT_STATUS_REQ_CMD             9001
#define    MANAGER_REPORT_STATUS_RESP_CMD            9002

//������Ա���߷���������
#define    MANAGER_DECIDE_SPEAKER_CMD                9005
#define    MANAGER_DECIDE_SPEAKER_RESP_CMD           9006

//������Աȡ������������
#define    MANAGER_CANCEL_SPEAKER_CMD                9007
#define    MANAGER_CANCEL_SPEAKER_RESP_CMD           9008

//������Ա��ʼ��������
#define    MANAGER_START_MEETING_CMD                 9009
#define    MANAGER_START_MEETING_RESP_CMD            9010

//������Աֹͣ��������
#define    MANAGER_STOP_MEETING_CMD                  9011
#define    MANAGER_STOP_MEETING_RESP_CMD             9012

//������Ա��ȡ�λ��б�����
#define    MANAGER_GET_MEMBER_LIST_CMD               9013
#define    MANAGER_GET_MEMBER_LIST_RESP_CMD          9014

//������֪ͨ�����߼���
#define    SERVER_NOTIFY_MEMBER_ENTRY_CMD            9015
#define    SERVER_NOTIFY_MEMBER_ENTRY_RESP_CMD       9016

//������֪ͨ�������˳�
#define    SERVER_NOTIFY_MEMBER_LEAVE_CMD            9017
#define    SERVER_NOTIFY_MEMBER_LEAVE_RESP_CMD       9018

//������ת���Ĳ��������뷢������
#define    SERVER_NOTIFY_MEMBER_APPLY_SPEAK_CMD                    9019
#define    SERVER_NOTIFY_MEMBER_APPLY_SPEAK_RESP_CMD               9020

//������ת���Ĳ�����ȡ�����뷢������
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SPEAK_CMD              9021
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SPEAK_RESP_CMD         9022

//������ת���Ĳ�����������������
#define    SERVER_NOTIFY_MEMBER_TEMP_LEAVE_CMD                      9023
#define    SERVER_NOTIFY_MEMBER_TEMP_LEAVE_RESP_CMD                 9024

//������ת���Ĳ�����ȡ��������������
#define    SERVER_NOTIFY_MEMBER_CANCEL_TEMP_LEAVE_CMD                9025
#define    SERVER_NOTIFY_MEMBER_CANCEL_TEMP_LEAVE_RESP_CMD           9026

//����Ա�л��᳡��ʾģʽ
#define    MANAGER_SHOW_MODE_CHANGE_CMD                              9027
#define    MANAGER_SHOW_MODE_CHANGE_RESP_CMD                         9028

#define    MANAGER_REQUEST_MEETING_LIST_CMD                          9031
#define    MANAGER_REQUEST_MEETING_LIST_RESP_CMD                     9032

#define    MANAGER_REQUEST_MIC_INFO_CMD                              9033
#define    MANAGER_REQUEST_MIC_INFO_RESP_CMD                         9034


//��������
//���ն�(ý��)֮�������
#define    TERMINATE_GET_UPRELAY_CMD                                 5001
#define    TERMINATE_GET_UPRELAY_RESP_CMD                            5002
#define    TERMINATE_GET_DOWNRELAY_CMD                               5003
#define    TERMINATE_GET_DOWNRELAY_RESP_CMD                          5004
//���ն�(����)֮�������
#define    TERMINATE_CHANGE_CTRL_SERVER_CMD                          5005

//��ý�������֮�������
#define    MS_NOTIFY_UPRELAY_CHANGE_CMD                              5101

//��ý��·��֮�������
#define    MSRC_NEW_TOPO_REQ_CMD_A                                    5201
#define    MSRC_NEW_TOPO_REQ_CMD_B                                    5202
#define    MSRC_NEW_TOPO_RESP_CMD                                     5203

//DHT�������
#define    DHT_MIC_TOPO_CMD                                            5301
#define    DHT_MEETING_INFO_CMD                                        5302
#define    DHT_NUM_OF_SQUARES_CMD                                      5303
#define    DHT_SPEAKER_INFO_CMD                                        5304
#define    DHT_ADMIN_STANDARD_CMD                                      5305

//�������Ʒ�����·��֮�������
#define    STPRC_HEART_BEAT_CMD                                           5401

//�������·�����ն˵�����
#define    STPRC_TERMINATE_QUERY_REQ_CMD                                   5402
#define    STPRC_TERMINATE_QUERY_RESP_CMD                                  5403

//�����������������֤ÿһ���û����ܸ�֪�������ߵı��
#define    CURR_SPEAKER_HEART_BEAT_CORRECT                                 5501

//����Agent��ý��Agent֮������
#define    NOTIFY_SPEAKER_CHANGE_CMD                                       5601
#define    NOTIFY_SELF_TO_SPEAKE_CMD                                       5602
#define    NOTIFY_MEETING_SERVER_CHANGE_CMD                                5603
#define    MEDIA_AGENT_REG_REQ_CMD                                         5604
#define    MEDIA_AGENT_REG_RESP_CMD                                        5605

//�������·�����ն˵�����(��ȡ������Ʒ�����)
#define  QUERY_RC_REQ_CMD       6101
#define  QUERY_RC_RESP_CMD      6102

//�������·�����ն˵�����(��ȡý��·�ɷ�����)
#define  QUERY_RC_MEDIA_REQ_CMD        6201
#define  QUERY_RC_MEDIA_RESP_CMD       6202

//�������� ������Ʒ����������Ա���͸����û�״̬������
#define  NOTIFY_ADMIN_MEMBER_STATUS_CMD   9029

//�������� ������Ʒ�����֪ͨ����������Ʒ������ͻ��˼���Ǩ�Ƶ�����
#define  NOTIFY_OTHER_STP_ACCEPT_NEW_USERS_CMD        6301
#define  NOTIFY_OTHER_STP_ACCEPT_NEW_USERS_RESP_CMD   6401

//��������  �������·���������Ʒ�����֮�䣬���ڵ���stp��ȡ����stp���������
#define  GET_STP_LIST_REQ_CMD        6501
#define  GET_STP_LIST_RESP_CMD       6502

//��������  �������·����host�ͻ���֮�������
#define  GET_HOST_SERVER_REQ_CMD        6601
#define  GET_HOST_SERVER_RESP_CMD       6602

//��������  �������server������˹���Ա֮�������
#define  NOTIFY_ROBOT_CMD        6701

//X1�����������
#define  X1_APPLY_NO_LONGER_SPEAK_REQ_CMD              8031
#define  X1_APPLY_NO_LONGER_SPEAK_RESP_CMD             8032

#define  MANAGE_X1_APPLY_NO_LONGER_SPEAK_REQ_CMD        9031
#define  MANAGE_X1_APPLY_NO_LONGER_SPEAK_RESP_CMD       9032


//�ͻ��˴�������
#define    TRANSFER_MIC_REQ_CMD                         8035
//stp��Ӧ��������
#define    TRANSFER_MIC_RESP_CMD                        8036

//������ת��������Ա�Ĵ�������
#define    SERVER_NOTIFY_MEMBER_TRANSFER_MIC_REQ_CMD                9035
#define    SERVER_NOTIFY_MEMBER_TRANSFER_MIC_RESULT_RESP_CMD        9036

//�ͻ��˼ӽ�������
#define    LOCK_MEETING_REQ_CMD                         8037
//stp��Ӧ��������
#define    LOCK_MEETING_RESP_CMD                        8038

//������֪ͨ�������״̬����
#define    SERVER_NOTIFY_MEETING_LOCKINFO_CMD             8039
#define    SERVER_NOTIFY_MEETING_LOCKINFO_RESP_CMD        8040










   