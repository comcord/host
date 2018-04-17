/*******************************************************************
* Copyright (C), 2008-2011,GOUMANG Corporation. All rights reserved.
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 异步模型控制接口方法
* @email   liuju@goumang.com
* @warning 非线程安全
* 异步模型小组  2009/2/13     2.2     定版 
*******************************************************************/
#ifndef ASYNMODEL_ASYNMODELPUB_H_GOUMANG_2009
#define ASYNMODEL_ASYNMODELPUB_H_GOUMANG_2009

#include "IOMsgPreProcessor.h"
#include "AsynResultDefine.h"
#include "AsynModelDataDefine.h"
#include "SubscribeEventInterface.h"


enum AsynModelSystemStatus
{
	AsynModel_System_Distroy = 0,	/**< enum value 异步模型销毁. */ 
	AsynModel_System_Inited,		///<异步模型初始化
	AsynModel_System_Running,		///<异步模型运行
	AsynModel_System_Stoping,		///<异步模型正在停止
	asynModel_System_Starting		///<异步模型正在启动
};
/**
/**
* SetLocalIPV6Addr() 设置ipv6地址
* @param sIP ipv6地址
* @return 无
*/
void SetLocalIPV6Addr(const char* sIP);
/**
/**
* clearLocalIPV6Addr() 清空异步模型地址
* @param 无
* @return 无
*/
void clearLocalIPV6Addr();


/**
* InitAsynModel() 异步模型初始化
* @param threadCount 工作线程数，如果为0使用默认的线程数。如果设置的值过大则采用系统允许的最大线程数，当前使用的最大线程数为50.
* @return AsynResultRet 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet InitAsynModel(unsigned int threadCount = 0);

/** 
* StartAsynModel() InitAsynModel()后调用此方法启动异步框架
* @param 无
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet StartAsynModel();

/** 
* StopAsynModel() 停止异步框架，取消所有的监听Tcp端口，Udp和Tcp Socket
* @param 无
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet StopAsynModel();

/** 
* ReleaseAsynModel() 同步销毁异步框架，可与InitAsynModel()方法配合反复调用
* @param 无
* @return 无
*/
void ReleaseAsynModel();

/** 
* GetConnID() 获取ConnID。
* @param 无
* @return 返回ConnID类型实例
*/
AsynModel::ConnID GetConnID();

/** 
* GetAsynModelStatus() 获取异步模型当前状态。
* @param 无
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModelSystemStatus GetAsynModelStatus();

/** 
* AddListenPort() 添加TCP监听
* @param ip char[16]
* @param port U16
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet AddListenPort(char ip[16],U16 port);

/** 
* RemoveListenPort() 删除TCP监听
* @param ip char[16] 
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet RemoveListenPort(char ip[16],U16 port);

void RemoveAllConnectSocket();

/** 
* AddListenPort() 添加TCP监听
* @param ip unsigned long  本机序
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet AddListenPort(unsigned long ip,U16 port);

/** 
* RemoveListenPort() 删除TCP监听
* @param ip unsigned long  本机序
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet RemoveListenPort(unsigned long ip,U16 port);

/** 
* AddUDPPort() 添加本地UDP监控端口
* @param ip ip[16]  
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型 ，
			SUCCESS，
			UDP_ADD_PORT_ALREADYEXIST，
			UDP_ADD_SOCKET_ERROR
*/
AsynModel::AsynResultRet AddUDPPort(char ip[16],U16 port);

/** 
* RemoveUDPPort() 删除本地UDP监控端口
* @param ip ip[16]  
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet RemoveUDPPort(char ip[16],U16 port);

/** 
* AddUDPPort() 添加本地UDP监控端口
* @param ip unsigned long  本机序
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet AddUDPPort(unsigned long ip ,U16 port);

/** 
* RemoveUDPPort() 删除本地UDP监控端口
* @param ip unsigned long  本机序
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet RemoveUDPPort(unsigned long ip, U16 port);

#ifdef __IOS__
/**
 * SetDefaultLocalUdpAddr() 设置默认的TCP本地监听
 * @param ip char[16]
 * @param port U16 本机序
 * @return 返回AsynResultRet类型告知是否成功以及错误类型
 */

AsynModel::AsynResultRet SetDefaultLocalUdpAddr(char ip[16], U16 port,bool IsAdd = true);

/**
 * SetDefaultLocalTcpAddr() 设置默认的UDP本地监控端口
 * @param ip char[16]
 * @param port U16 本机序
 * @return 返回AsynResultRet类型告知是否成功以及错误类型
 */
AsynModel::AsynResultRet SetDefaultLocalTcpAddr(char ip[16], U16 port,bool IsAdd = true);
#else
/** 
* SetDefaultLocalUdpAddr() 设置默认的TCP本地监听
* @param ip char[16] 
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet SetDefaultLocalUdpAddr(char ip[16], U16 port);

/** 
* SetDefaultLocalTcpAddr() 设置默认的UDP本地监控端口
* @param ip char[16] 
* @param port U16 本机序
* @return 返回AsynResultRet类型告知是否成功以及错误类型
*/
AsynModel::AsynResultRet SetDefaultLocalTcpAddr(char ip[16], U16 port);
#endif

/** 
* RegistIOPreProcessor() 注册预处理器,process 外部维护
* @param processor synModel::IOMsgPreProcessor类型指针 
* @return bool类型告知是否成功
*/
bool RegistIOPreProcessor(AsynModel::IOMsgPreProcessor *processor);

/** 
* UnregistIOPreProcessor() 注销预处理器,process 外部维护
* @param processor synModel::IOMsgPreProcessor类型指针 
* @return bool类型告知是否成功
*/
bool UnregistIOPreProcessor(AsynModel::IOMsgPreProcessor *processor);

#ifdef __IOS__

/**
 * RegistEventCallBackInterface() 注册异步框架事件观察者，只能有一个
 * @param SubScribeEventCallBack类型指针
 * @return 无
 */
void RegistEventCallBackInterface(char*localIp,unsigned short port,AsynModel::SocketType type,AsynModel::SubScribeEventCallBack * pCallback);

void RegistEventCallBackInterface_New(AsynModel::SubScribeEventCallBack * pCallback);

void UnregistEventCallBackInterface_New(AsynModel::SubScribeEventCallBack * pCallback);

#endif
/**
 * RegistEventCallBackInterface() 注册异步框架事件观察者，只能有一个
 * @param SubScribeEventCallBack类型指针
 * @return 无
 */
void RegistEventCallBackInterface(AsynModel::SubScribeEventCallBack * pCallback);


/**
* UnregistEventCallBackInterface() 注销异步框架事件观察者
* @param 无
* @return 无
*/
void UnregistEventCallBackInterface();

/** 
* PostMsg() 在Session外部投递消息到Session
* @param destSid U32类型 目标SessionID
* @param msgid U32类型 发送的消息ID
* @param msgData char类型指针 发送的消息内容
* @param dataLen U32类型  发送的消息长度
* @param isShareMsgData bool类型 是否需要共享内存传递，默认为否
* @return bool类型告知是否成功
*/
AsynModel::AsynResultRet PostMsg(U32 destSid,U32 msgid,char* msgData,U32 dataLen,
								 bool isShareMsgData = false);

/** 
* PostMsg() 在Session外部投递销毁消息到Session
* @param sid const U32类型 目标SessionID
* @return bool类型告知是否成功
*/
AsynModel::AsynResultRet PostCancelSessionEvent(const U32 sid);


/** 
* GetFluxStatInfo() 获取流量统计信息,每获取一次内部记数器将清空一次
* @param sendUdpPacketNum  U32类型 发送的UDP包个数
* @param sendUdpPacketSize U32类型 发送的UDP包大小
* @param recvUdpPacketNum  U32类型 接收的UDP包个数
* @param recvUdpPacketSize U32类型 接收的UDP包大小
* @param sendTcpPacketNum  U32类型 发送的Tcp包个数
* @param sendTcpPacketSize U32类型 发送的Tcp包大小
* @param recvTcpPacketNum  U32类型 接收的Tcp包个数
* @param recvTcpPacketSize U32类型 接收的Tcp包大小
* @return 无
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
