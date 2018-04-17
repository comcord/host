#pragma once

#define    MEETING_ID_BREAK_CHAR    "*"
#define    ENTERPRISE_ID_BREAK_CHAR   "%"
#define    MEETING_SPEAKER_BREAK_CHAR    "@"
#define    MEETING_ADMIN_STANDARD_BREAK_CHAR    "_"
#define    MEETING_SPEAKER_NUM_BREAK_CHAR       "+"
#define    MEETING_SPEAKER_ID_BREAK_CHAR        "&"
#define    MEETING_SPEAKER_TOPO_BREAK_CHAR        "!"

//test chengyf
//心跳周期，单位ms
#define   HEART_BEAT_TIME_SPAN    (1*1000)

//udp包上限
#define    UDP_MAX_PACKET_LEN   (50*1024)

//服务器端发给客户端的显示msgId，是用于session投递的，业务ID在命令里面，需要解析
#define    SERVER_RESP_CMD             10000

//基础心跳
#define    HEART_BEAT_REQ_CMD         6000
#define    HEART_BEAT_RESP_CMD        6001

//流信息心跳
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
/*******************普通参会人员与管理人员共用命令******************/
/*******************************************************************/

//参会请求
#define    ATTEND_MEETING_REQ_CMD      7001
#define    ATTEND_MEETING_RESP_CMD     7002

//退出会议
#define    QUIT_MEETING_REQ_CMD        7003
#define    QUIT_MEETING_RESP_CMD       7004

//服务器通知被抢占
#define    SERVER_NOTIFY_GRAB_CMD      7005


/*******************************************************************/
/*************************普通参会人员 相关命令*********************/
/*******************************************************************/

//普通参会人员汇报状态
#define    REPORT_STATUS_REQ_CMD            8001
#define    REPORT_STATUS_RESP_CMD           8002

//申请发言
#define    APPLY_SPEAK_REQ_CMD              8007
#define    APPLY_SPEAK_RESP_CMD             8008

//取消申请发言
#define    CANCEL_APPLY_SPEAK_REQ_CMD       8009
#define    CANCEL_APPLY_SPEAK_RESP_CMD      8010

//暂离请求
#define    TEMP_LEAVE_REQ_CMD               8011
#define    TEMP_LEAVE_RESP_CMD              8012

//取消暂离请求
#define    CANCEL_TEMP_LEAVE_REQ_CMD        8013
#define    CANCEL_TEMP_LEAVE_RESP_CMD       8014


//服务器通知会议开始
#define    SERVER_NOTIFY_MEETING_START_CMD           8015
#define    SERVER_NOTIFY_MEETING_START_RESP_CMD      8016

//服务器通知会议停止
#define    SERVER_NOTIFY_MEETING_STOP_CMD            8017
#define    SERVER_NOTIFY_MEETING_STOP_RESP_CMD       8018

//服务器通知客户端开始发言
#define    SERVER_NOTIFY_START_SPEAK_CMD             8019
#define    SERVER_NOTIFY_START_SPEAK_RESP_CMD        8020

//服务器通知客户端停止发言
#define    SERVER_NOTIFY_STOP_SPEAK_CMD              8021
#define    SERVER_NOTIFY_STOP_SPEAK_RESP_CMD         8022

//服务器通知切换会场显示模式
#define    SERVER_NOTIFY_SHOW_MODE_CHANGE_CMD           8023
#define    SERVER_NOTIFY_SHOW_MODE_CHANGE_RESP_CMD      8024

//服务器通知新发言者产生
#define    SERVER_NOTIFY_DECIDE_SPEAKER_CMD             8025
#define    SERVER_NOTIFY_DECIDE_SPEAKER_RESP_CMD        8026

//服务器通知现有发言者失效
#define    SERVER_NOTIFY_CANCEL_SPEAKER_CMD             8027
#define    SERVER_NOTIFY_CANCEL_SPEAKER_RESP_CMD        8028


/*******************************************************************/
/*****************************管理人员 相关命令*********************/
/*******************************************************************/

//管理人员汇报状态
#define    MANAGER_REPORT_STATUS_REQ_CMD             9001
#define    MANAGER_REPORT_STATUS_RESP_CMD            9002

//管理人员决策发言者命令
#define    MANAGER_DECIDE_SPEAKER_CMD                9005
#define    MANAGER_DECIDE_SPEAKER_RESP_CMD           9006

//管理人员取消发言者命令
#define    MANAGER_CANCEL_SPEAKER_CMD                9007
#define    MANAGER_CANCEL_SPEAKER_RESP_CMD           9008

//管理人员开始会议命令
#define    MANAGER_START_MEETING_CMD                 9009
#define    MANAGER_START_MEETING_RESP_CMD            9010

//管理人员停止会议命令
#define    MANAGER_STOP_MEETING_CMD                  9011
#define    MANAGER_STOP_MEETING_RESP_CMD             9012

//管理人员获取参会列表命令
#define    MANAGER_GET_MEMBER_LIST_CMD               9013
#define    MANAGER_GET_MEMBER_LIST_RESP_CMD          9014

//服务器通知参与者加入
#define    SERVER_NOTIFY_MEMBER_ENTRY_CMD            9015
#define    SERVER_NOTIFY_MEMBER_ENTRY_RESP_CMD       9016

//服务器通知参与者退出
#define    SERVER_NOTIFY_MEMBER_LEAVE_CMD            9017
#define    SERVER_NOTIFY_MEMBER_LEAVE_RESP_CMD       9018

//服务器转发的参与者申请发言命令
#define    SERVER_NOTIFY_MEMBER_APPLY_SPEAK_CMD                    9019
#define    SERVER_NOTIFY_MEMBER_APPLY_SPEAK_RESP_CMD               9020

//服务器转发的参与者取消申请发言命令
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SPEAK_CMD              9021
#define    SERVER_NOTIFY_MEMBER_CANCEL_APPLY_SPEAK_RESP_CMD         9022

//服务器转发的参与者暂离请求命令
#define    SERVER_NOTIFY_MEMBER_TEMP_LEAVE_CMD                      9023
#define    SERVER_NOTIFY_MEMBER_TEMP_LEAVE_RESP_CMD                 9024

//服务器转发的参与者取消暂离请求命令
#define    SERVER_NOTIFY_MEMBER_CANCEL_TEMP_LEAVE_CMD                9025
#define    SERVER_NOTIFY_MEMBER_CANCEL_TEMP_LEAVE_RESP_CMD           9026

//管理员切换会场显示模式
#define    MANAGER_SHOW_MODE_CHANGE_CMD                              9027
#define    MANAGER_SHOW_MODE_CHANGE_RESP_CMD                         9028

#define    MANAGER_REQUEST_MEETING_LIST_CMD                          9031
#define    MANAGER_REQUEST_MEETING_LIST_RESP_CMD                     9032

#define    MANAGER_REQUEST_MIC_INFO_CMD                              9033
#define    MANAGER_REQUEST_MIC_INFO_RESP_CMD                         9034

#define    GET_MEMBER_LIST_REQ_CMD                                    9113
#define    GET_MEMBER_LIST_RESP_CMD                                   9114

//新增命令
//与终端(媒体)之间的命令
#define    TERMINATE_GET_UPRELAY_CMD                                 5001
#define    TERMINATE_GET_UPRELAY_RESP_CMD                            5002
#define    TERMINATE_GET_DOWNRELAY_CMD                               5003
#define    TERMINATE_GET_DOWNRELAY_RESP_CMD                          5004

#define    TERMINATE_GET_UPRELAY_NEW_CMD                             5005
#define    TERMINATE_GET_UPRELAY_NEW_RESP_CMD                        5006
#define    TERMINATE_GET_DOWNRELAY_NEW_CMD                           5007
#define    TERMINATE_GET_DOWNRELAY_NEW_RESP_CMD                      5008  
#define    TERMINATE_REPORT_BLACKLIST_CMD                            5105        //黑名单报告命令
#define    TERMINATE_REPORT_BLACKLIST_RESP_CMD                       5106        //黑名单报告应答命令
#define	   TERMINATE_REPORT_BLACKLIST_NEW_CMD                        5107		 //新黑盒命令,STPAgent转发
#define	   TERMINATE_REPORT_BLACKLIST_RESP_NEW_CMD                   5108

#define	   TERMINATE_REPORT_BLACKLIST_DIR_CMD                        5109		 //汇报的白名单区分上下行
#define	   TERMINATE_REPORT_BLACKLIST_RESP_DIR_CMD                   5110


//与终端(控制)之间的命令
#define    TERMINATE_CHANGE_CTRL_SERVER_CMD                          5005

//与媒体服务器之间的命令
#define    MS_NOTIFY_UPRELAY_CHANGE_CMD                              5101

//与媒体路由之间的命令
#define    MSRC_NEW_TOPO_REQ_CMD_A                                    5221
#define    MSRC_NEW_TOPO_REQ_CMD_B                                    5222
#define    MSRC_NEW_TOPO_RESP_CMD                                     5223
#define    MSRC_NEW_TOPOON4G_REQ_CMD_A                                5204
#define    MSRC_NEW_TOPOON4G_REQ_CMD_B                                5205
#define    MSRC_NEW_TOPOON4G_RESP_CMD                                 5206
#define	   MSRC_NEW_TOPO_MIX_REQ_CMD_A								  5211
#define	   MSRC_NEW_TOPO_MIX_REQ_CMD_B								  5212
#define	   MSRC_NEW_TOPO_MIX_RESP_CMD								  5213
//DHT相关命令
#define    DHT_MIC_TOPO_CMD                                            5301
#define    DHT_MEETING_INFO_CMD                                        5302
#define    DHT_NUM_OF_SQUARES_CMD                                      5303
#define    DHT_SPEAKER_INFO_CMD                                        5304
#define    DHT_ADMIN_STANDARD_CMD                                      5305
//订阅版
#define    DHT_SPEAKER_NUM_CMD                                         5306
#define    DHT_SINGLE_SPEAKER_INFO_CMD                                 5307

//与会议控制服务器路由之间的命令
#define    STPRC_HEART_BEAT_CMD                                           5401
//会议控制路由与终端的命令
#define    STPRC_TERMINATE_QUERY_REQ_CMD                                   5402
#define    STPRC_TERMINATE_QUERY_RESP_CMD                                  5403

//双向心跳新增
#define    STPRC_HEART_BEAT_CONN_CMD                                       5404

#define    STPRC_HEART_BEAT_REQ_CMD                                        5405
#define    STPRC_HEART_BEAT_RESP_CMD                                       5406

//新增心跳矫正命令，保证每一个用户都能感知到发言者的变更
#define    CURR_SPEAKER_HEART_BEAT_CORRECT                                 5501

//会议Agent与媒体Agent之间命令
#define    NOTIFY_SPEAKER_CHANGE_CMD                                       5601
#define    NOTIFY_SELF_TO_SPEAKE_CMD                                       5602
#define    NOTIFY_MEETING_SERVER_CHANGE_CMD                                5603
#define    MEDIA_AGENT_REG_REQ_CMD                                         5604
#define    MEDIA_AGENT_REG_RESP_CMD                                        5605

//MPU相关 对MediaAgent发送 服务器异常命令
#define	   NOTIFY_SEVER_EXCEPTION_CMD									   5613
//NAT打洞
#define    NAT_REQ_CMD													   5701
#define    NAT_RESP_CMD													   5702

//MPU需求相关命令
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

//会议控制路由与终端的命令(获取会议控制服务器)
#define  QUERY_RC_REQ_CMD       6101
#define  QUERY_RC_RESP_CMD      6102

//会议控制路由与终端的命令(获取会议控制服务器)
#define  QUERY_RC_REQ_BLACK_CMD 6103
#define  QUERY_RC_RESP_BLACK_CMD 6104

//会议控制路由与终端的命令(获取媒体路由服务器)
#define  QUERY_RC_MEDIA_REQ_CMD        6201
#define  QUERY_RC_MEDIA_RESP_CMD       6202

//新增命令 会议控制服务器向管理员发送更改用户状态的命令
#define  NOTIFY_ADMIN_MEMBER_STATUS_CMD   9029

//新增命令 会议控制服务器通知其他会议控制服务器客户端即将迁移的命令
#define  NOTIFY_OTHER_STP_ACCEPT_NEW_USERS_CMD        6301
#define  NOTIFY_OTHER_STP_ACCEPT_NEW_USERS_RESP_CMD   6401

//新增命令  会议控制路由与会议控制服务器之间，用于单个stp拉取所有stp的相关命令
#define  GET_STP_LIST_REQ_CMD        6501
#define  GET_STP_LIST_RESP_CMD       6502

//新增命令  会议控制路由与host客户端之间的命令
#define  GET_HOST_SERVER_REQ_CMD        6601
#define  GET_HOST_SERVER_RESP_CMD       6602

//新增命令  会议控制server与机器人管理员之间的命令
#define  NOTIFY_ROBOT_CMD        6701

//新增命令  会议控制路由与日志上传工具之间的命令
#define GET_QOSINFO_SERVER_REQ_CMD      6801
#define GET_QOSINFO_SERVER_RESP_CMD     6802
#define SERVER_REPORT_MEETINGINFO_CMD   6803
#define  GET_RELAYLIST_REQ_CMDID                  6901
#define  GET_RELAYLIST_RESP_CMDID                 6902 
#define  SET_USER_UPRELAY_REQ_CMDID             6903
#define  SET_USER_UPRELAY_RESP_CMDID             6904 
#define  SET_USER_DOWNRELAY_REQ_CMDID        6905
#define  SET_USER_DOWNRELAY_RESP_CMDID       6906
//X1相关新增命令
#define  X1_APPLY_NO_LONGER_SPEAK_REQ_CMD              8031
#define  X1_APPLY_NO_LONGER_SPEAK_RESP_CMD             8032

#define  MANAGE_X1_APPLY_NO_LONGER_SPEAK_REQ_CMD        9031
#define  MANAGE_X1_APPLY_NO_LONGER_SPEAK_RESP_CMD       9032


//客户端传麦命令
#define    TRANSFER_MIC_REQ_CMD                         8035
//stp回应命令受理
#define    TRANSFER_MIC_RESP_CMD                        8036

//服务器转发给管理员的传麦命令
#define    SERVER_NOTIFY_MEMBER_TRANSFER_MIC_REQ_CMD                9035
#define    SERVER_NOTIFY_MEMBER_TRANSFER_MIC_RESULT_RESP_CMD        9036

//客户端加解锁命令
#define    LOCK_MEETING_REQ_CMD                         8037
//stp回应命令受理
#define    LOCK_MEETING_RESP_CMD                        8038

//服务器通知会议加锁状态命令
#define    SERVER_NOTIFY_MEETING_LOCKINFO_CMD             8039
#define    SERVER_NOTIFY_MEETING_LOCKINFO_RESP_CMD        8040

//订阅版新增命令
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

//新增指定申请发言
#define    ASSIGN_APPLY_SPEAK_REQ_CMD                        8057
#define    ASSIGN_APPLY_SPEAK_RESP_CMD                       8058

//新增指定申请取消发言
#define    ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD                 8059
#define    ASSIGN_CANCEL_APPLY_SPEAK_RESP_CMD                8060

//插话相关命令
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
//新增主持人  
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

//新增直播会议
#define    MASTER_START_LIVE_MEETING_REQ_CMD                 8097
#define    MASTER_START_LIVE_MEETING_RESP_CMD                8098
#define    SERVER_NOTIFY_LIVE_MEETING_CMD                    8099

#define    SERVER_NOTIFY_ADMIN_MEETING_START_LIVE_CMD        9041
#define    SERVER_NOTIFY_ADMIN_MEETING_START_LIVE_RESP_CMD   9042


//新增四方会议
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

//获取sesssion 参会者
#define    SET_MEETING_INFORMATION_CMD						 8200

enum  CameraStatus
{
	CAMERA_INVALID = 0,
	CAMERA_OPEN,
	CAMERA_CLOSE,
	CAMERA_UNDEFINED,
};

//新增命令  会议控制路由与host客户端之间的命令(hostList)	
#define  GET_HOST_SERVER_REQ_NEW_CMD        6611
#define  GET_HOST_SERVER_RESP_NEW_CMD       6612

//新增命令 远程控制命令 信令层当黑盒转发
#define  BLACK_BOX_CMD                      100


/*******************4G模块命令列表*****************************************************/

//参会请求
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
//订阅版新增命令
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

////新增指定申请发言
//#define    ASSIGN_APPLY_SPEAK_REQ_CMD                        8057
//#define    ASSIGN_APPLY_SPEAK_RESP_CMD                       8058
//
////新增指定申请取消发言
//#define    ASSIGN_CANCEL_APPLY_SPEAK_REQ_CMD                 8059
//#define    ASSIGN_CANCEL_APPLY_SPEAK_RESP_CMD                8060
//
////插话相关命令
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


/***************云端录制 STP和终端交互命令 begin******************/
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

/***************云端录制 STP和录制服务器交互命令 begin************/
#define    STP_NOTIFY_MEETING_START_RECORD_REQ_CMD           8253
#define    STP_NOTIFY_MEETING_START_RECORD_RESP_CMD          8254
#define    STP_NOTIFY_MEETING_STOP_RECORD_REQ_CMD            8255
#define    STP_NOTIFY_MEETING_STOP_RECORD_RESP_CMD           8256
/***************************end*********************************/
   