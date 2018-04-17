#ifndef GMSOCKETBASE_H_2006_CHANNELSOFT
#define GMSOCKETBASE_H_2006_CHANNELSOFT
#include <iostream>
#include "BaseDataType.h"
#ifdef WIN32

#include <Ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")
typedef int socklen_t;

static int GMWSAGetLastError()
{
	return WSAGetLastError();
}

#else // Linux

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

typedef int SOCKET;
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define	closesocket close
#define SokErrorNo  errno

//用于ShutDown
#define SD_BOTH    SHUT_RDWR
#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR

static int GMWSAGetLastError()
{
	return SokErrorNo;
}

#endif

//预处理：指定与静态库一起连接

typedef unsigned short T_PORT;
typedef char  T_IP[16];

#define ISIPVALID(x)  (strlen((x)) > 0 && strlen((x)) < 16) 
#define IP_COPY(to, from) memcpy((to), (from),16)
#define IPCOPY(to,from) IP_COPY(to, from)
#define IPZERO(x) memset(x,0,16)
#define IP_ZERO(x) IPZERO(x)
#define ISIPEQUAL(ipa, ipb) (strcmp((ipa),(ipb)) == 0 )
#define ISPORTVALID(x) (x > 0 && x < 65535)
//为什么不取俩端的值？为什么是16位？


class GMSocketBase
{

protected:
	GMSocketBase(bool ipv4 = true);
	~GMSocketBase();
	/*
	 *	建立SOCKET
	 */
	bool createSocket(int nSockType, bool ipv4 = true);

public:

	enum
	{
		MAXPACKETSIZE = 4 * 1024  //发送的最大数据PACKET
	};
	
	/*
	 *	绑定IP和port , 错误抛出 CSocketException
	 */
	bool bind(U16 port = 0, char* ip = "");

	/*
	 *	关闭SOCKET
	 */
	bool close();

	/*
	 *	设置CLOSE方式
	 */
	void setCloseType(bool bWait = false);

	/*
	 *	设置I/O模式 （阻塞、非阻塞）
	 */
	bool setBlocking( bool bIoIsBlock );

	bool isSocketValid();

	/*
	 *	设置发送超时
	 */
	bool setSendTimeout( unsigned int second  = 10);

	/*
	*	设置发送超时
	*/
	bool setSendTimeoutMillisecond( unsigned int millisecond  = 100);

	/*
	 *	设置接收超时
	 */
	bool setRecvTimeout( unsigned int second = 10 );
    
	/*
	 *	设置接收超时 单位 millisecond
	 */
	bool setRecvTimeoutMillisecond(unsigned int millisecond  = 100 );
	
	/*
	 *	设置接受、发送缓冲区大小
	 */
	bool setSockBuffSize( int nSndBuffSize = 64 * 1024, int nRcvBuffSize = 64 * 1024 );

	/*
	 *	if use ipv6 the length of localIP must be 32 byte or more than 32 byte
	 */
	bool getLocalAddr(char* localIP, U16& localPort);

	/*
	 *	SOCKET套接字
	 */
	SOCKET m_nSock;

protected:
	/*
	 *	初始化SOCKET环境
	 */
	bool init();
	/*
	 * 还原环境设置
	 */
	void destroy();
	
private:
	/*
	 * WSAStartup 是否成功
	 */
	static int m_bCallCount;
#ifdef WIN32
	static bool m_bIsSuccessInit;
#endif

protected:
	bool m_ipv4;
};

#endif
