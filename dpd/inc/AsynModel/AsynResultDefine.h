/****************************************************************************                                                                          
* AsynResultDefine.h                       
* @version 2.2
* @date    2008.9.20
* @author  异步模型小组
* @fuction 系统中API返回值定义
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009
#define ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009

namespace AsynModel
{
	/**
	*系统中方法的返回值定义，可结合日志找寻问题，需要重新整理
	*/
	enum AsynResultRet
	{
		SUCCESS = 0,///<成功
		
		ASYNMODEL_ALREADY_INIT = 1,///<异步模型已经被初始化
		ASYNMODEL_COMPORTCONTROLER_INIT_ERROR = 2,///<完成端口初始化错误
		ASYNMODEL_CREATE_COMPUTEWORKERS_ERROR = 3,///<创建ComputeWorker时出错
		ASYNMODEL_TCPIOPROCESSOR_INIT_ERROR = 4,///<TCPIOProcessor初始化失败
		ASYNMODEL_UDPIOPROCESSOR_INIT_ERROR = 5,///<UDPIOProcessor初始化失败
		ASYNMODEL_UNINIT = 6,///<异步模型没有初始化
		
		IP_LENGTH_INVALID = 7,///输入的IP长度非法
		IPPORT_INVALID = 8,///<输入的IPPORT非法
		MANAGER_INVALID = 9,///<AsynManager无效
		SENDER_INVALID = 10,///<Sender无效
		UNKNOWN_SENDER_TYPE = 11,///<未知的Sender类型
		MANAGER_NOT_RUNNING = 12,///<AsynManager没有运行
		TCPPROCESSOR_NOT_ENABLE = 13,///<TCPProcessor没有启用
		MSG_DATAPACKET_TOO_BIG = 14,///<用户数据过大
		UNKNOWN_PROTOCOL = 15,///<未知的协议类型
		//UNKNOWN_PERIO_TYPE,///<未知的PerIO类型
		MSG_SERALIZE_ERROR = 16,///<Msg 序列化出错

		CONNECT_NOT_ESTABLISHED = 17,///<tcp 连接没有建立
		POST_USERDATA_ERROR = 18,///<用户投递TCP发送数据出错

		TCPSOCKETITEM_GET_FALSE = 19,///<TCPSocketItem获取失败
		TCPSOCKETITEM_INVALID = 20,///<TCPSocketItem无效
		ASYNCONNECT_POST_FALSE = 21,///<投递异步连接失败
		TCPSOCKET_CANNOT_RECONNECT = 22,///<TCP Socket不能重连

		CONNECT_ALSO_ESTABLISHED = 23,///<TCP连接已经被建立

		DESTSESSION_TERMINATED = 24,///<要发送的目标Session已经被销毁

		MEMALLOC_ERROR = 25,///<MemStream分配失败
		MSG_UNSERIALIZE_ERROR = 26,///<消息反序列化失败
		//COMPLETATION_ERROR,///<投递完成端口出错
		NONSUPPORT_CONNID_TYPE = 27,///<不支持的 ConnID类型

		UDP_SENDER_HAS_NOT_INIT = 28,///<UDP Sender没有初始化
		UDP_SENDER_CONSTRUCT_SOCKET_ERROR = 29,///<UDP Sender构建Socket失败
		UDP_SENDER_HAS_INITED = 30,///<UDP Sender已经被初始化
		UDP_SOCKET_ITEM_INVALID = 31,///<UDPSocketItem无效
		UDP_SOCKET_ERROR = 32,///<UDP Socket 错误
		UDP_ADD_SOCKET_ERROR = 33,///< UDP添加Socket失败
		UDP_REMOVE_DEFAULTPORT_SUCCESS = 34,///<UDP移出默认端口成功？why 不返回SUCCESS

		POSTTO_COMPLETATIONPORT_ERROR = 35,///<投递完成端口失败
		COMPLETATIONPORT_UNKNOW_PERIO_TYPE = 36,///未知的PerIO类型
		COMPLETATIONPORT_DESTROY = 37,///<完成端口已被销毁
		ASYNMODEL_SOCKET_ERROR = 38,///<Socket错误
		RECV_PARAM_INVALID = 39,///<接收参数无效
		SEND_PARAM_INVALID = 40,///<发送参数无效
		PARAM_INVALID = 41,///参数无效，这么多的参数无效定义是否需要重新审视一下

		TCP_RECV_FULL_PACKET = 42,///<TCP接收到完整的包
		TCP_RECV_PART_PACKET = 43,///<TCP接收到半包 没有使用与TCP_RECV_PART_HEAD_SUCCESS和TCP_RECV_PART_BODY_SUCCESS定义重复
		TCP_SEND_FULL_PACKET = 44,///<TCP发送了完整的包
		TCP_SEND_PART_PACKET = 45,///<TCP包中的一部分被发送 epoll使用
		TCP_RECV_ERROR = 46,///<TCP接收数据出错
		//TCP_SEND_ERROR = 47,///<TCP发送数据出错
		TCP_ERROR_PACKET = 48,///<TCP错包
		TCP_SOCKET_ERROR=49,///<TCP Socket错误
		PERIO_TYPE_ERROR=50,///<在TCP中接收到非TCP的PerIO处理请求
		TCP_RECV_PART_HEAD_SUCCESS = 51,///<成功接收了一部分的TCP头
		TCP_RECV_PART_BODY_SUCCESS = 52,///<成功接收了一部分的TCP体
		TCP_ADDDEFAULTLISTEN_FAILURE = 53,///<添加TCP默认监听失败
		TCP_ADD_LISTEN_IP_PORT_ALREADYEXIST = 54,///<TCP添加监听失败，传入的IP与Port已经被监听
		TCP_CREATE_SOCKET_ERROR = 55,///<创建TCPSocket出错
		TCP_CREATE_TCPSOCKETITEM_FAILURE = 56,///<创建TCPSocketItem失败
		TCP_BIND_LISTEN_FAILURE = 57,///<绑定Listen失败
		TCP_BIND_COMPLETEPORT_FAILURE = 58,///<TCP Socket与完成端口绑定失败
		TCP_POST_ACCEPT_FAILURE = 59,///<TCP 投递accept失败
		TCP_REMOVE_LISTEN_IP_PORT_FAILURE = 60,///<移出默认的TCP监听失败
		TCP_USER_CANCEL_DEFAULT_LISTEN =61,///<用户取消默认监听 没有使用
		TCP_REMOVE_DEFAULT_LISTEN_SUCCESS = 62,///<移出默认的TCP监听成功
		TCP_ERROR_PACKET_SIZE = 63,///<错误的TCP包大小

		NEW_OBJECT_ERROR = 64,///<new 实例失败
		UNKNOWN_ERROR = 65,///<位置错误
		TCP_SEND_PACKET_TIMEOUT = 66, ///<大数据发送超时错误

		INSTANT_COMPLET = 67, //立即将整个perio处理完，（有可能是接着上一次继续处理）
		POST_MSG_FAILED = 68,
		FATAL_ERROR = 69,
		SYSTEM_BUSY = 70, //系统繁忙
		WSA_STARTUP_ERROR = 71, //WsaStartup错误
		UDP_ADD_PORT_ALREADYEXIST = 72,///< UDP添加端口失败，传入的IP与Port已经被绑定
	};
}
#endif //ASYNMODEL_ASYNRESULTDEFINE_H_GOUMANG_2009
