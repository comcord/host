#ifndef GMTCPSOCKET_H_2006_CHANNELSOFT
#define  GMTCPSOCKET_H_2006_CHANNELSOFT

#include "GMSocketBase.h"
#include "SysDiffDefine.h"

#pragma warning( disable : 4275 )

/*
 *	TCP服务端，守侯TCP连接
 */
class GMTcpServer : public GMSocketBase
{
public:
	GMTcpServer():m_listenCount(10){}

	bool createSocket(bool ipv4 = true)
	{
		return GMSocketBase::createSocket(SOCK_STREAM, ipv4);
	}

	bool startListen();

	/*
	 *	监听连接，返回连接的SOCKET if use ipv6 the length of szClientIP must be 32 bytes or more than 32 bytes
	 */
	SOCKET acceptConn(char *szClientIP, U16 &clientPort);

	/*
	 *	设置监听数量
	 */
	void setListenCount( int nListenCount = 10)
	{
		m_listenCount = nListenCount;
	}

private:
	int m_listenCount;
};

/*
 *	TCP的客户端，用于传输数据
 */
class GMTcpClient : public GMSocketBase
{
public:
	GMTcpClient(SOCKET nSock = INVALID_SOCKET, bool ipv4 = true) : GMSocketBase(ipv4)
	{
		m_nCnnTimeout= 0;
		if(nSock != INVALID_SOCKET)
			m_nSock = nSock;
	}

	/*
	 *	创建STREAM SOCKET
	 */
	bool createSocket(bool ipv4 = true)
	{
		return GMSocketBase::createSocket(SOCK_STREAM, ipv4);
	}

	/*
	 *	连接到远程的TcpServer
	 */
	bool connect(char* szIP, U16 nPort);

	/*
	 *	发送数据， 错误抛出 GMSocketException \ GMTimeOutException
	 */
	int send( unsigned char *szData, int len) ;

	/*
	 *	接收数据 错误抛出 GMSocketException \ GMTimeOutException
	 */
	int recv( unsigned char *, int len);

	/*
	 *	关闭NAGLE算法
	 */
	bool disableNagleAlog();

	void setCnnTimeout( int sec);

private:
	int m_nCnnTimeout ;
};
#endif
