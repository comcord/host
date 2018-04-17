#pragma once

#include <list>
#include <string>
#include "GMTcpSocket.h"
#include "GMSocketException.h"
#include "GMSZ.h"
#include "SysDiffDefine.h"

/**
*	web下载的客户端类
*/
class GMHttpClient
{
public:
	struct Chunkeds
	{
		int  iChunkedLen;
		char *pChunkedBuff;
	};

	GMHttpClient(void);
	~GMHttpClient(void);

	/**
	*	解析传入的下载url,获得server的Host，get的内容，server ip，server端口
	*/
	static bool parseURL( const char * pUrl, char * pHost, char * pGet,
		char * pIp, unsigned short * pPort, char * pRefer = static_cast<char*>(NULL) );

	/**
	*	建立与purl连接的socket
	*/
	bool connect( const char * pUrl );

	/**
	*	设置接收超时
	*/
	bool setRecvTimeout( unsigned int second = 10 );

	void setConnectTimeOut( unsigned int second = 10 );

	/**
	*	打开一个待下载的url

	-1 解析URL失败
	-2 没有建立连接
	-3 与url服务器通信异常
	-4 http响应头超过最大接收大小
	-5 响应头返回 错误码
	*/
	int openURL( const char * pUrl,
		bool isKeepLive = false,
		unsigned int offset = 0,
		unsigned int len = 0,
		std::string* httpRespHead = static_cast<std::string*>(NULL) );

	/**
	*	获得待下载的长度，在调用openURL之后，再调用这个方法
	*/
	int getContentSize()
	{
		return m_nContentSize;
	}

	/**
	*	接收下载内容
	*/
	int readData( char * pBuff, int size );

	/**
	*	关闭与web server的连接
	*/
	void close();

private:
	void Writefile(const std::string& str);

	int recvChunkedBuffAndReturnBuffLen();
	int readChunkedLen();
	int hex2Decimal( char *b );
	int readChunkedBuff( char *pChunkedBuff,int chunkedLen );

private:
	std::list<Chunkeds>  m_sumOfChunked;
	bool m_bIsChunkedEncode;

	GMTcpClient m_client; ///<与web server连接的tcpsock类
	int m_nContentSize; ///<下载的内容长度
	bool m_bConned; ///<是否已经建立连接
};
