/*******************************************************************
* Copyright (C), 2008-2011,GOUMANG Corporation. All rights reserved.
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 异步模型发布的头文件声明
* @email   liuju@goumang.com
* @warning 非线程安全
* 异步模型小组  2009/2/13     2.2     定版 
*******************************************************************/
#ifndef ASYN_MODEL_PUBLISH_H_2008_GOUMANG
#define ASYN_MODEL_PUBLISH_H_2008_GOUMANG
#ifdef WIN32
#ifndef _LIB
#pragma comment( lib, "AsynModel" )
#pragma comment(lib,"ws2_32")
#pragma comment(lib,"Mswsock")
#pragma comment(lib, "gmhelperLib")
#endif 
#endif

#ifndef LOG_AM 
#define LOG_AM  ((I64)1 << 47)
#endif

#define ASYNMODELPUB_LOGID				100
#define ASYNMANAGER_LOGID					101				

#define SESSIONMAP_LOGID					103
#define SESSION_LOGID						104
#define PBS_LOGID			105

#define CTRLOR_LOGID	106
#define CW_LOGID			    107
#define LOCALSENDER_LOGID					108

#define TCPIOPROCESSOR_LOGID				110
#define ASYNMODEL_TCPIOPROCESSHELPER_LOGID			111
#define TCPSENDER_LOGID					112

#define UDPIOPROCESS_LOGID				120
#define UDPSENDER_LOGID					121

#define ALIABLEUDPACKTABLE_LOGID			122
#define RECVDATATABLE_LOGID				123


#include "gmhelperLibPub.h"
#include "AsynModelPub.h"
#include "Session.h"
#include "ISender.h"

#endif //ASYN_MODEL_PUBLISH_H_2008_GOUMANG
