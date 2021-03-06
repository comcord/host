#ifndef __P2PCONNAPI_H__
#define __P2PCONNAPI_H__

class IP2PConnCallBack
{
public:
	IP2PConnCallBack() {}
	virtual ~IP2PConnCallBack() {}

public:
	/*
	*   连接结果
	*   参数：
	*       accountId ： 对端视讯号
	*       rc  ： 错误码。 0： 连接成功  非0： 连接失败
	*       rd  ： 错误描述
	*/
	virtual int OnConnectResult(const char* accountId,int rc,const char* rd) = 0;

	/*
	*   连接断开通知
	*   参数：
	*       accountId ： 对端视讯号
	*       reason  ： 连接断开原因
	*                  1：对端主动断开连接
	*                  2：本端与hostserver连接断开
	*                  3：与对端心跳超时
	*/
	virtual int OnDisConnect(const char* accountId,int reason) = 0;

	/*
	*   接收到对端数据
	*   参数：
	*       accountId     ： 数据发送方视讯号
	*       buf           ： 接收到的数据buf
	*       len           ： 接收到数据长度
	*/
	virtual int OnRecvData(const char* accountId,const char* buf,const unsigned short len) = 0;

	/*
	*   数据发送结果
	*   参数：
	*       accountId     ： 目标视讯号
	*       rc            ： 发送结果  0：成功  -1：发送失败
	*       userData      ： 用户数据
	*/
	virtual int OnSendDataResult(const char* accountId,int rc,const unsigned long long userData) = 0;
};

class IP2PConn
{
public:
	IP2PConn() {}
	virtual ~IP2PConn() {}

public:
	/*
	*   设置回调对象
	*/
	virtual int SetP2PConnCB(IP2PConnCallBack* cb) = 0;

	/*
	*   开始连接
	*/
	virtual int Connect() = 0;

	/*
	*   发送数据
	*   参数：
	*       buf    ： 发送数据
	*       len    ： 发送数据长度
	*       userData ： 用户数据
	*/
	virtual int SendData(const char* buf,const unsigned short len,const unsigned long long userData) = 0;

	/*
	*   断开连接
	*/
	virtual int DisConnect() = 0;
};

/*
*   接收到连接请求回调函数
*/
typedef void (*P2PConnectCB)(const char* peerAccountId,IP2PConn* conn);

extern "C"
{
	/*
	*   创建P2PConn对象
	*   参数：
	*       accountId  ： 本地视讯号
	*       peerAccountId  ： 对端视讯号
	*       cb  ： 回调对象
	*/
	IP2PConn* CreateP2PConn(const char* accountId,const char* peerAccountId,IP2PConnCallBack* cb);

	/*
	*   删除P2PConn对象
	*/
	void DeleteP2PConn(IP2PConn* pConn);

	/*
	*   设置P2P连接回调函数
	*/
	void SetP2PConnectCallback(P2PConnectCB* cb);
}

#endif
