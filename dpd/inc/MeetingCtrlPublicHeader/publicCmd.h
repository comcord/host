#pragma once

#define    MEETING_ID_BREAK_CHAR    "*"
#define    ENTERPRISE_ID_BREAK_CHAR   "%"
#define    MEETING_SPEAKER_BREAK_CHAR    "@"
#define    MEETING_ADMIN_STANDARD_BREAK_CHAR    "_"
#define    MEETING_SPEAKER_NUM_BREAK_CHAR       "+"
#define    MEETING_SPEAKER_ID_BREAK_CHAR        "&"
#define    MEETING_SPEAKER_TOPO_BREAK_CHAR        "!"

//test chengyf
//�������ڣ���λms
#define   HEART_BEAT_TIME_SPAN    (1*1000)

//udp������
#define    UDP_MAX_PACKET_LEN   (50*1024)

//�������˷����ͻ��˵���ʾmsgId��������sessionͶ�ݵģ�ҵ��ID���������棬��Ҫ����
#define    SERVER_RESP_CMD             10000

//��������
#define    HEART_BEAT_REQ_CMD         6000
#define    HEART_BEAT_RESP_CMD        6001

//����Ϣ����
#define   STREAMINFO_HEART_BEAT_REQ_CMD 6002
#define   STREAMINFO_HEART_BEAT_RESP_CMD 6003

#define   HEART_BEAT_NEW_REQ_CMD       6004
#define   HEART_BEAT_NEW_RESP_CMD      6005

#define   STREAMINFO_HEART_BEAT_NEW_REQ_CMD  6006
#define   STREAMINFO_HEART_BEAT_NEW_RESP_CMD 6007

#define   BLACK_PIPE_REQ_CMD                 6008
#define   KEEP_PIPE_ALIVE_CMD				 6009
#define   SERVER_NOTIFY_START_UDP_CONN       6010

#define   REPORT_RELAY_LIST_CMD				 6011
#define   REPORT_RELAY_LIST_RESP_CMD         6012

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

#define    GET_MEMBER_LIST_REQ_CMD                                    9113
#define    GET_MEMBER_LIST_RESP_CMD                                   9114

//��������
//���ն�(ý��)֮�������
#define    TERMINATE_GET_UPRELAY_CMD                                 5001
#define    TERMINATE_GET_UPRELAY_RESP_CMD                            5002
#define    TERMINATE_GET_DOWNRELAY_CMD                               5003
#define    TERMINATE_GET_DOWNRELAY_RESP_CMD                          5004

#define    TERMINATE_GET_UPRELAY_NEW_CMD                             5005
#define    TERMINATE_GET_UPRELAY_NEW_RESP_CMD                        5006
#define    TERMINATE_GET_DOWNRELAY_NEW_CMD                           5007
#define    TERMINATE_GET_DOWNRELAY_NEW_RESP_CMD                      5008  
#define    TERMINATE_REPORT_BLACKLIST_CMD                            5105        //��������������
#define    TERMINATE_REPORT_BLACKLIST_RESP_CMD                       5106        //����������Ӧ������
#define	   TERMINATE_REPORT_BLACKLIST_NEW_CMD                        5107		 //�ºں�����,STPAgentת��
#define	   TERMINATE_REPORT_BLACKLIST_RESP_NEW_CMD                   5108

#define	   TERMINATE_REPORT_BLACKLIST_DIR_CMD                        5109		 //�㱨�İ���������������
#define	   TERMINATE_REPORT_BLACKLIST_RESP_DIR_CMD                   5110


//���ն�(����)֮�������
#define    TERMINATE_CHANGE_CTRL_SERVER_CMD                          5005

//��ý�������֮�������
#define    MS_NOTIFY_UPRELAY_CHANGE_CMD                              5101

//��ý��·��֮�������
#define    MSRC_NEW_TOPO_REQ_CMD_A                                    5221
#define    MSRC_NEW_TOPO_REQ_CMD_B                                    5222
#define    MSRC_NEW_TOPO_RESP_CMD                                     5223
#define    MSRC_NEW_TOPOON4G_REQ_CMD_A                                5204
#define    MSRC_NEW_TOPOON4G_REQ_CMD_B                                5205
#define    MSRC_NEW_TOPOON4G_RESP_CMD                                 5206
#define	   MSRC_NEW_TOPO_MIX_REQ_CMD_A								  5211
#define	   MSRC_NEW_TOPO_MIX_REQ_CMD_B								  5212
#define	   MSRC_NEW_TOPO_MIX_RESP_CMD								  5213
//DHT�������
#define    DHT_MIC_TOPO_CMD                                            5301
#define    DHT_MEETING_INFO_CMD                                        5302
#define    DHT_NUM_OF_SQUARES_CMD                                      5303
#define    DHT_SPEAKER_INFO_CMD                                        5304
#define    DHT_ADMIN_STANDARD_CMD                                      5305
//���İ�
#define    DHT_SPEAKER_NUM_CMD                                         5306
#define    DHT_SINGLE_SPEAKER_INFO_CMD                                 5307

//�������Ʒ�����·��֮�������
#define    STPRC_HEART_BEAT_CMD                                           5401
//�������·�����ն˵�����
#define    STPRC_TERMINATE_QUERY_REQ_CMD                                   5402
#define    STPRC_TERMINATE_QUERY_RESP_CMD                                  5403

//˫����������
#define    STPRC_HEART_BEAT_CONN_CMD                                       5404

#define    STPRC_HEART_BEAT_REQ_CMD                                        5405
#define    STPRC_HEART_BEAT_RESP_CMD                                       5406

//�����������������֤ÿһ���û����ܸ�֪�������ߵı��
#define    CURR_SPEAKER_HEART_BEAT_CORRECT                                 5501

//����Agent��ý��Agent֮������
#define    NOTIFY_SPEAKER_CHANGE_CMD                                       5601
#define    NOTIFY_SELF_TO_SPEAKE_CMD                                       5602
#define    NOTIFY_MEETING_SERVER_CHANGE_CMD                                5603
#define    MEDIA_AGENT_REG_REQ_CMD                                         5604
#define    MEDIA_AGENT_REG_RESP_CMD                                        5605

//MPU��� ��MediaAgent���� �������쳣����
#define	   NOTIFY_SEVER_EXCEPTION_CMD									   5613
//NAT��
#define    NAT_REQ_CMD													   5701
#define    NAT_RESP_CMD													   5702

//MPU�����������
#define	   MPU_HEART_BEAT_REQ_CMD										   5801
#define	   MPU_HEART_BEAT_RESP_CMD                                         5802
#define    GET_MPU_LIST_REQ_CMD                                            5803
#define	   GET_MPU_LIST_RESP_CMD										   5804
#define	   TRANSFER_MEDIA_REQ_CMD                                          5805
#define    TRANSFER_MEDIA_RESP_CMD                                         5806
#define	   CANCEL_TRANSFER_MEDIA_REQ_CMD                                   5807
#define    CANCEL_TRANSFER_MEDIA_RESP_CMD                                  5808
#define    SERVER_NOTIFY_MPU_TRANSFER_MEDIA_REQ_CMD                        5809
#define    SERVER_NOTIFY_MPU_TRANSFER_MEDIA_RESP_CMD                       5810
#define    SERVER_NOTIFY_MPU_CANCEL_TRANSFER_MEDIA_REQ_CMD                 5811
#define    SERVER_NOTIFY_MPU_CANCEL_TRANSFER_MEDIA_RESP_CMD                5812

#define    SERVER_NOTIFY_MPU_ATTEND_MEETING_REQ_CMD                        5813
#define    SERVER_NOTIFY_MPU_ATTEND_MEETING_RESP_CMD                       5814

#define    MPU_GET_MEETING_TRANSMEDIA_STATUS_REQ_CMD    5815
#define    MPU_GET_MEETING_TRANSMEDIA_STATUS_RESP_CMD   5816

#define    ALL_MPUS_EXCEPTION_NOTIFY_CLIENT_CMD         7801

//�������·�����ն˵�����(��ȡ������Ʒ�����)
#define  QUERY_RC_REQ_CMD       6101
#define  QUERY_RC_RESP_CMD      6102

//�������·�����ն˵�����(��ȡ������Ʒ�����)
#define  QUERY_RC_REQ_BLACK_CMD 6103
#define  QUERY_RC_RESP_BLACK_CMD 6104

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

//��������  �������·������־�ϴ�����֮�������
#define GET_QOSINFO_SERVER_REQ_CMD      6801
#define GET_QOSINFO_SERVER_RESP_CMD     6802
#define SERVER_REPORT_MEETINGINFO_CMD   6803
#define  GET_RELAYLIST_REQ_CMDID                  6901
#define  GET_RELAYLIST_RESP_CMDID                 6902 
#define  SET_USER_UPRELAY_REQ_CMDID             6903
#define  SET_USER_UPRELAY_RESP_CMDID             6904 
#define  SET_USER_DOWNRELAY_REQ_CMDID        6905
#define  SET_USER_DOWNRELAY_RESP_CMDID       6906
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

//���İ���������
#define    SUBSCRIBE_USER_REQ_CMD             8041
#define    SUBSCRIBE_USER_RESP_CMD            8042

#define    CANCEL_SUBSCRIBE_USER_REQ_CMD                     8043
#define    CANCEL_SUBSCRIBE_USER_RESP_CMD                    8044

#define    SPEAKER_CLOSE_STREAM_REQ_CMD                      8045
#define    SPEAKER_CLOSE_STREAM_RESP_CMD                     8046

#define    SERVER_NOTIFY_SPEAKER_CLOSE_STREAM_CMD            8047

#define    SPEAKER_OPEN_STREAM_REQ_CMD                       8049
#define    SPEAKER_OPEN_STREAM_RESP_CMD                      8050

#define    SERVER_NOTIFY_SPEAKER_OPEN_STREAM_CMD             8051

#define    SERVER_TRANSFER_CLIENT_CMD                        8052

#define    CIENT_TRANSFER_AUDIO_REQ_CMD                      8053
#define    CIENT_TRANSFER_AUDIO_RESP_CMD                     8054

#define    SERVER_SEND_AUDIO_CMD                             8055

//����ָ�����뷢��
#define    ASSIGN_APPLY_SPEAK_REQ_CMD                        8057
#define    ASSIGN_APPLY_SPEAK_RESP_CMD                       8058

//����ָ������ȡ������
#define    ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD                 8059
#define    ASSIGN_CANCEL_APPLY_SPEAK_RESP_CMD                8060

//�廰�������
#define    START_CHIPIN_REQ_CMD                              8071
#define    START_CHIPIN_RESP_CMD                             8072
#define    SERVER_NOTIFY_START_CHIPIN_CMD                    8073

#define    STOP_CHIPIN_REQ_CMD                               8075
#define    STOP_CHIPIN_RESP_CMD                              8076
#define    SERVER_NOTIFY_STOP_CHIPIN_CMD                     8077

#define    CLIENT_SEND_TOSERVER_CHIPIN_DATA_CMD              8078
#define    SERVER_SEND_TOCLIENT_CHIPIN_DATA_CMD              8079

#define    CLIENT_OPERATE_CAMERA_REQ_CMD                     8081
#define    CLIENT_OPERATE_CAMERA_RESP_CMD                    8082

#define    SERVER_NOTIFY_CAMERA_OPERATE_CMD                  8083

#define    NOTIFY_MCU_START_MEETING_CMD                      2001
#define    NOTIFY_MCU_START_MEETING_RESP_CMD                 2002

#define    NOTIFY_MCU_STOP_MEETING_CMD                       2003
#define    NOTIFY_MCU_STOP_MEETING_RESP_CMD                  2004

#define    HTTP_POST_RESULT_RESP_CMD                         2005

#define    SERVER_NOTIFY_MCU_START_RECORD_CMD                2101
#define    SERVER_NOTIFY_MCU_START_RECORD_RESP_CMD           2102
#define    SERVER_NOTIFY_MCU_STOP_RECORD_CMD                 2103
#define    SERVER_NOTIFY_MCU_STOP_RECORD_RESP_CMD            2104
#define    SERVER_NOTIFY_MCU_STOP_MEETING_CMD                2105
#define    SERVER_NOTIFY_MCU_STOP_MEETING_RESP_CMD           2106
//����������  
#define    MASTER_CHANGE_MEETING_STYLE_REQ_CMD               8085
#define    MASTER_CHANGE_MEETING_STYLE_RESP_CMD              8086
#define    SERVER_NOTIFY_MEETING_STYLE_CHANGE_CMD            8087

#define    MASTER_ASSIGN_APPLY_SPEAK_REQ_CMD                 8089
#define    MASTER_ASSIGN_APPLY_SPEAK_RESP_CMD                8090

#define    MASTER_ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD          8091           
#define    MASTER_ASSIGN_CANCEL_APPLY_SPEAK_RESP_CMD         8092

#define    CLIENT_HANDLEUP_REQ_CMD                           8093
#define    CLIENT_HANDLEUP_RESP_CMD                          8094
#define    SERVER_TRANSMIT_HANDLEUP_CMD                      8095

#define    SERVER_NOTIFY_MASTER_APPLY_SPEAK_CMD              9037
#define    SERVER_NOTIFY_MASTER_APPLY_CANCEL_SPEAK_CMD       9039

//����ֱ������
#define    MASTER_START_LIVE_MEETING_REQ_CMD                 8097
#define    MASTER_START_LIVE_MEETING_RESP_CMD                8098
#define    SERVER_NOTIFY_LIVE_MEETING_CMD                    8099

#define    SERVER_NOTIFY_ADMIN_MEETING_START_LIVE_CMD        9041
#define    SERVER_NOTIFY_ADMIN_MEETING_START_LIVE_RESP_CMD   9042


//�����ķ�����
#define    CLIENT_SCREEN_SHARE_REQ_CMD                       8101
#define    CLIENT_SCREEN_SHARE_RESP_CMD                      8102
#define    SERVER_NOTIFY_DECIDE_SCREEN_SHARER_CMD            8103
#define    SERVER_NOTIFY_START_SHARE_CMD                     8104

#define    CLIENT_NO_LONGER_SCREEN_SHARE_REQ_CMD             8105
#define    CLIENT_NO_LONGER_SCREEN_SHARE_RESP_CMD            8106
#define    SERVER_NOTIFY_CANCEL_SCREEN_SHARER_CMD            8107
#define    SERVER_NOTIFY_STOP_SHARE_CMD                      8108

#define    SERVER_NOTIFY_MEMBER_APPLY_SHARE_CMD              9043
#define    SERVER_NOTIFY_MEMBER_APPLY_SHARE_RESP_CMD         9044

#define    MANAGER_DECIDE_SHARER_CMD                         9045
#define    MANAGER_DECIDE_SHARER_RESP_CMD                    9046

#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SHARE_CMD       9047
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SHARE_RESP_CMD  9048

#define    MANAGER_CANCEL_SHARER_CMD                         9049
#define    MANAGER_CANCEL_SHARER_RESP_CMD                    9050

#define    SERVER_NOTIFY_MEMBER_APPLY_DATASHARE_CMD              9051
#define    SERVER_NOTIFY_MEMBER_APPLY_DATASHARE_RESP_CMD         9052

#define    MANAGER_DECIDE_DATASHARER_CMD                         9053
#define    MANAGER_DECIDE_DATASHARER_RESP_CMD                    9054

#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_DATASHARE_CMD       9055
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_DATASHARE_RESP_CMD  9056

#define    MANAGER_CANCEL_DATASHARER_CMD                         9057
#define    MANAGER_CANCEL_DATASHARER_RESP_CMD                    9058

#define    MASTER_ASSIGN_MAIN_SPEAKER_REQ_CMD                8111
#define    MASTER_ASSIGN_MAIN_SPEAKER_RESP_CMD               8112
#define    SERVER_NOTIFY_MAIN_SPEAKER_CMD                    8113
#define    SERVER_NOTIFY_CANCEL_MAIN_SPEAKER_CMD             8115

#define    SPEAKER_OPERATE_STREAM_REQ_CMD                    8117
#define    SPEAKER_OPERATE_STREAM_RESP_CMD                   8118
#define    SERVER_NOTIFY_SPEAKER_OPERATE_STREAM_CMD          8119

#define    MASTER_KICK_CLIENT_REQ_CMD                        8121
#define    MASTER_KICK_CLIENT_RESP_CMD                       8122
#define    SERVER_NOTIFY_CLIENT_BEKICEED_CMD                 8123

#define    CLIENT_UPDATE_STREAM_INFO_REQ_CMD                 8131
#define    CLIENT_UPDATE_STREAM_INFO_RESP_CMD                8132
#define    SERVER_NOTIFY_CLIENT_STREAM_INFO_CHANGED_CMD      8133

#define    MASTER_ASSIGN_CHANGE_LOUDSPE_STATUS_REQ_CMD       8135
#define    MASTER_ASSIGN_CHANGE_LOUDSPE_STATUS_RESP_CMD      8136
#define    SERVER_NOTIFY_CLIENT_CHANGE_LOUDSPE_STATUS_CMD    8137

#define    CLIENT_NOTIFY_LOUDSPEAKER_STAUTS_REQ_CMD          8139
#define    CLIENT_NOTIFY_LOUDSPEAKER_STAUTS_RESP_CMD         8140
#define    SERVER_NOTIFY_LOUDSPEAKER_STAUTS_CMD              8141

#define    MASTER_ASSIGN_CHANGE_MEDIA_STATUS_REQ_CMD         8143
#define    MASTER_ASSIGN_CHANGE_MEDIA_STATUS_RESP_CMD        8144
#define    SERVER_NOTIFY_MEDIA_STATUS_CMD                    8145

#define    CLIENT_NOTIFY_IVUS_STATUS_REQ_CMD                 8147
#define    CLIENT_NOTIFY_IVUS_STATUS_RESP_CMD                8148
#define    SERVER_NOTIFY_IVUS_STATUS_CMD                     8149

#define    CLIENT_DATA_SHARE_REQ_CMD                       8150
#define    CLIENT_DATA_SHARE_RESP_CMD                      8151
#define    SERVER_NOTIFY_DECIDE_DATA_SHARER_CMD            8152
#define    SERVER_NOTIFY_START_DATA_SHARE_CMD              8153

#define    CLIENT_NO_LONGER_DATA_SHARE_REQ_CMD             8154
#define    CLIENT_NO_LONGER_DATA_SHARE_RESP_CMD            8155
#define    SERVER_NOTIFY_CANCEL_DATA_SHARER_CMD            8156
#define    SERVER_NOTIFY_STOP_DATA_SHARE_CMD               8157

#define    MASTER_RECORD_REQ_CMD                             8197
#define    MASTER_RECORD_RESP_CMD                            8198
#define    SERVER_NOTIFY_RECORD_STATUS_CMD                   8199

//��ȡsesssion �λ���
#define    SET_MEETING_INFORMATION_CMD						 8200

enum  CameraStatus
{
	CAMERA_INVALID = 0,
	CAMERA_OPEN,
	CAMERA_CLOSE,
	CAMERA_UNDEFINED,
};

//��������  �������·����host�ͻ���֮�������(hostList)	
#define  GET_HOST_SERVER_REQ_NEW_CMD        6611
#define  GET_HOST_SERVER_RESP_NEW_CMD       6612

//�������� Զ�̿������� ����㵱�ں�ת��
#define  BLACK_BOX_CMD                      100


/*******************4Gģ�������б�*****************************************************/

//�λ�����
#define    ATTEND_MEETING_REQ_CMD_4G            27001
#define    HEART_BEAT_REQ_CMD_4G                26000
#define    STREAMINFO_HEART_BEAT_REQ_CMD_4G     26002
#define    QUIT_MEETING_REQ_CMD_4G              27003
#define    REPORT_STATUS_REQ_CMD_4G             28001
#define    APPLY_SPEAK_REQ_CMD_4G               28007
#define    MANAGER_GET_MEMBER_LIST_CMD_4G       29013
#define    GET_MEMBER_LIST_REQ_CMD_4G           29113
#define    TERMINATE_GET_UPRELAY_CMD_4G         25001
#define    TERMINATE_GET_DOWNRELAY_CMD_4G       25003
#define    TERMINATE_REPORT_BLACKLIST_CMD_4G    25105        
#define    QUERY_RC_REQ_CMD_4G                  26101
#define    QUERY_RC_MEDIA_REQ_CMD_4G            26201
#define    X1_APPLY_NO_LONGER_SPEAK_REQ_CMD_4G  28031
#define    TRANSFER_MIC_REQ_CMD_4G              28035
#define    LOCK_MEETING_REQ_CMD_4G              28037
//���İ���������
//#define    SUBSCRIBE_USER_REQ_CMD             8041
//#define    SUBSCRIBE_USER_RESP_CMD            8042
//
//#define    CANCEL_SUBSCRIBE_USER_REQ_CMD                     8043
//#define    CANCEL_SUBSCRIBE_USER_RESP_CMD                    8044

//#define    SPEAKER_CLOSE_STREAM_REQ_CMD                      8045
//#define    SPEAKER_CLOSE_STREAM_RESP_CMD                     8046
//
//#define    SERVER_NOTIFY_SPEAKER_CLOSE_STREAM_CMD            8047
//
//#define    SPEAKER_OPEN_STREAM_REQ_CMD                       8049
//#define    SPEAKER_OPEN_STREAM_RESP_CMD                      8050
//
//#define    SERVER_NOTIFY_SPEAKER_OPEN_STREAM_CMD             8051
//
//#define    SERVER_TRANSFER_CLIENT_CMD                        8052
//
//#define    CIENT_TRANSFER_AUDIO_REQ_CMD                      8053
//#define    CIENT_TRANSFER_AUDIO_RESP_CMD                     8054
//
//#define    SERVER_SEND_AUDIO_CMD                             8055

////����ָ�����뷢��
//#define    ASSIGN_APPLY_SPEAK_REQ_CMD                        8057
//#define    ASSIGN_APPLY_SPEAK_RESP_CMD                       8058
//
////����ָ������ȡ������
//#define    ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD                 8059
//#define    ASSIGN_CANCEL_APPLY_SPEAK_RESP_CMD                8060
//
////�廰�������
//#define    START_CHIPIN_REQ_CMD                              8071
//#define    START_CHIPIN_RESP_CMD                             8072
//#define    SERVER_NOTIFY_START_CHIPIN_CMD                    8073
//
//#define    STOP_CHIPIN_REQ_CMD                               8075
//#define    STOP_CHIPIN_RESP_CMD                              8076
//#define    SERVER_NOTIFY_STOP_CHIPIN_CMD                     8077
//
//#define    CLIENT_SEND_TOSERVER_CHIPIN_DATA_CMD              8078
//#define    SERVER_SEND_TOCLIENT_CHIPIN_DATA_CMD              8079

#define    CLIENT_OPERATE_CAMERA_REQ_CMD_4G                  28081 
#define    MASTER_CHANGE_MEETING_STYLE_REQ_CMD_4G            28085
#define    MASTER_ASSIGN_APPLY_SPEAK_REQ_CMD_4G              28089
#define    MASTER_ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD_4G       28091           
#define    CLIENT_HANDLEUP_REQ_CMD_4G                        28093
#define    MASTER_START_LIVE_MEETING_REQ_CMD_4G              28097
#define    CLIENT_SCREEN_SHARE_REQ_CMD_4G                    28101
#define    CLIENT_NO_LONGER_SCREEN_SHARE_REQ_CMD_4G          28105
#define    MASTER_ASSIGN_MAIN_SPEAKER_REQ_CMD_4G             28111
#define    SPEAKER_OPERATE_STREAM_REQ_CMD_4G                 28117
#define    MASTER_KICK_CLIENT_REQ_CMD_4G                     28121
#define    CLIENT_UPDATE_STREAM_INFO_REQ_CMD_4G              28131
#define    MASTER_RECORD_REQ_CMD_4G                          28197
#define    GET_HOST_SERVER_REQ_CMD_4G                        26601


/***************�ƶ�¼�� STP���ն˽������� begin******************/
#define    MASTER_NOTIFY_MEETING_START_RECORD_REQ_CMD        8241
#define    MASTER_NOTIFY_MEETING_START_RECORD_RESP_CMD       8242
#define    SERVER_NOTIFY_MEETING_START_RECORD_CMD            8243

#define    MASTER_NOTIFY_MEETING_STOP_RECORD_REQ_CMD         8245
#define    MASTER_NOTIFY_MEETING_STOP_RECORD_RESP_CMD        8246
#define    SERVER_NOTIFY_MEETING_STOP_RECORD_CMD             8247

#define    MASTER_NOTIFY_DEDIVER_USER_REQ_CMD                8249
#define    MASTER_NOTIFY_DEDIVER_USER_RESP_CMD               8250
#define    SERVER_NOTIFY_DEDIVER_USER_CMD                    8251
/***************************end*********************************/

/***************�ƶ�¼�� STP��¼�Ʒ������������� begin************/
#define    STP_NOTIFY_MEETING_START_RECORD_REQ_CMD           8253
#define    STP_NOTIFY_MEETING_START_RECORD_RESP_CMD          8254
#define    STP_NOTIFY_MEETING_STOP_RECORD_REQ_CMD            8255
#define    STP_NOTIFY_MEETING_STOP_RECORD_RESP_CMD           8256
/***************************end*********************************/
   