#pragma once

#define    MEETING_ID_BREAK_CHAR    "*"
#define    ENTERPRISE_ID_BREAK_CHAR   "%"
#define    MEETING_SPEAKER_BREAK_CHAR    "@"
#define    MEETING_ADMIN_STANDARD_BREAK_CHAR    "_"


//test chengyf
//心跳周期，单位ms
#define   HEART_BEAT_TIME_SPAN    (1*1000)

//udp包上限
#define    UDP_MAX_PACKET_LEN   (50*1024)

//服务器端发给客户端的显示msgId，是用于session投递的，业务ID在命令里面，需要解析
#define    SERVER_RESP_CMD             10000

//心跳
#define    HEART_BEAT_REQ_CMD         6000
#define    HEART_BEAT_RESP_CMD        6001

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


//新增命令
//与终端(媒体)之间的命令
#define    TERMINATE_GET_UPRELAY_CMD                                 5001
#define    TERMINATE_GET_UPRELAY_RESP_CMD                            5002
#define    TERMINATE_GET_DOWNRELAY_CMD                               5003
#define    TERMINATE_GET_DOWNRELAY_RESP_CMD                          5004
//与终端(控制)之间的命令
#define    TERMINATE_CHANGE_CTRL_SERVER_CMD                          5005

//与媒体服务器之间的命令
#define    MS_NOTIFY_UPRELAY_CHANGE_CMD                              5101

//与媒体路由之间的命令
#define    MSRC_NEW_TOPO_REQ_CMD_A                                    5201
#define    MSRC_NEW_TOPO_REQ_CMD_B                                    5202
#define    MSRC_NEW_TOPO_RESP_CMD                                     5203

//DHT相关命令
#define    DHT_MIC_TOPO_CMD                                            5301
#define    DHT_MEETING_INFO_CMD                                        5302
#define    DHT_NUM_OF_SQUARES_CMD                                      5303
#define    DHT_SPEAKER_INFO_CMD                                        5304
#define    DHT_ADMIN_STANDARD_CMD                                      5305

//与会议控制服务器路由之间的命令
#define    STPRC_HEART_BEAT_CMD                                           5401

//会议控制路由与终端的命令
#define    STPRC_TERMINATE_QUERY_REQ_CMD                                   5402
#define    STPRC_TERMINATE_QUERY_RESP_CMD                                  5403

//新增心跳矫正命令，保证每一个用户都能感知到发言者的变更
#define    CURR_SPEAKER_HEART_BEAT_CORRECT                                 5501

//会议Agent与媒体Agent之间命令
#define    NOTIFY_SPEAKER_CHANGE_CMD                                       5601
#define    NOTIFY_SELF_TO_SPEAKE_CMD                                       5602
#define    NOTIFY_MEETING_SERVER_CHANGE_CMD                                5603
#define    MEDIA_AGENT_REG_REQ_CMD                                         5604
#define    MEDIA_AGENT_REG_RESP_CMD                                        5605

//会议控制路由与终端的命令(获取会议控制服务器)
#define  QUERY_RC_REQ_CMD       6101
#define  QUERY_RC_RESP_CMD      6102

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










   