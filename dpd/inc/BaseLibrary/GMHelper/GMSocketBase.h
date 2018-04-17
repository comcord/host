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

//����ShutDown
#define SD_BOTH    SHUT_RDWR
#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR

static int GMWSAGetLastError()
{
	return SokErrorNo;
}

#endif

//Ԥ����ָ���뾲̬��һ������

typedef unsigned short T_PORT;
typedef char  T_IP[16];

#define ISIPVALID(x)  (strlen((x)) > 0 && strlen((x)) < 16) 
#define IP_COPY(to, from) memcpy((to), (from),16)
#define IPCOPY(to,from) IP_COPY(to, from)
#define IPZERO(x) memset(x,0,16)
#define IP_ZERO(x) IPZERO(x)
#define ISIPEQUAL(ipa, ipb) (strcmp((ipa),(ipb)) == 0 )
#define ISPORTVALID(x) (x > 0 && x < 65535)
//Ϊʲô��ȡ���˵�ֵ��Ϊʲô��16λ��


class GMSocketBase
{

protected:
	GMSocketBase(bool ipv4 = true);
	~GMSocketBase();
	/*
	 *	����SOCKET
	 */
	bool createSocket(int nSockType, bool ipv4 = true);

public:

	enum
	{
		MAXPACKETSIZE = 4 * 1024  //���͵��������PACKET
	};
	
	/*
	 *	��IP��port , �����׳� CSocketException
	 */
	bool bind(U16 port = 0, char* ip = "");

	/*
	 *	�ر�SOCKET
	 */
	bool close();

	/*
	 *	����CLOSE��ʽ
	 */
	void setCloseType(bool bWait = false);

	/*
	 *	����I/Oģʽ ����������������
	 */
	bool setBlocking( bool bIoIsBlock );

	bool isSocketValid();

	/*
	 *	���÷��ͳ�ʱ
	 */
	bool setSendTimeout( unsigned int second  = 10);

	/*
	*	���÷��ͳ�ʱ
	*/
	bool setSendTimeoutMillisecond( unsigned int millisecond  = 100);

	/*
	 *	���ý��ճ�ʱ
	 */
	bool setRecvTimeout( unsigned int second = 10 );
    
	/*
	 *	���ý��ճ�ʱ ��λ millisecond
	 */
	bool setRecvTimeoutMillisecond(unsigned int millisecond  = 100 );
	
	/*
	 *	���ý��ܡ����ͻ�������С
	 */
	bool setSockBuffSize( int nSndBuffSize = 64 * 1024, int nRcvBuffSize = 64 * 1024 );

	/*
	 *	if use ipv6 the length of localIP must be 32 byte or more than 32 byte
	 */
	bool getLocalAddr(char* localIP, U16& localPort);

	/*
	 *	SOCKET�׽���
	 */
	SOCKET m_nSock;

protected:
	/*
	 *	��ʼ��SOCKET����
	 */
	bool init();
	/*
	 * ��ԭ��������
	 */
	void destroy();
	
private:
	/*
	 * WSAStartup �Ƿ�ɹ�
	 */
	static int m_bCallCount;
#ifdef WIN32
	static bool m_bIsSuccessInit;
#endif

protected:
	bool m_ipv4;
};

#endif
