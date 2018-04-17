#ifndef __P2PCONNAPI_H__
#define __P2PCONNAPI_H__

class IP2PConnCallBack
{
public:
	IP2PConnCallBack() {}
	virtual ~IP2PConnCallBack() {}

public:

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
	*       rc            ： 发送结果  0：成功  -1：发送失败   -2 ： 发送超时
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
	*   初始化P2PConn
	*/
	virtual int Start(IP2PConnCallBack* cb) = 0;

	/*
	*   发送数据
	*   参数：
	*       peerAccountId ： 对端视讯号
	*       buf    ： 发送数据
	*       len    ： 发送数据长度
	*       userData ： 用户数据
	*/
	virtual int SendData(const char* peerAccountId,const char* buf,const unsigned short len,const unsigned long long userData) = 0;

	/*
	*   停止P2PConn
	*/
	virtual int Stop() = 0;
};

extern "C"
{
	/*
	*   创建P2PConn对象
	*   参数：
	*       accountId  ： 本地视讯号
	*/
	IP2PConn* CreateP2PConn(const char* accountId);

	/*
	*   删除P2PConn对象
	*/
	void DeleteP2PConn(IP2PConn* pConn);

}

#endif
