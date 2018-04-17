#ifndef GMTCPSOCKET_H_2006_CHANNELSOFT
#define  GMTCPSOCKET_H_2006_CHANNELSOFT

#include "GMSocketBase.h"
#include "SysDiffDefine.h"

#pragma warning( disable : 4275 )

/*
 *	TCP����ˣ��غ�TCP����
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
	 *	�������ӣ��������ӵ�SOCKET if use ipv6 the length of szClientIP must be 32 bytes or more than 32 bytes
	 */
	SOCKET acceptConn(char *szClientIP, U16 &clientPort);

	/*
	 *	���ü�������
	 */
	void setListenCount( int nListenCount = 10)
	{
		m_listenCount = nListenCount;
	}

private:
	int m_listenCount;
};

/*
 *	TCP�Ŀͻ��ˣ����ڴ�������
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
	 *	����STREAM SOCKET
	 */
	bool createSocket(bool ipv4 = true)
	{
		return GMSocketBase::createSocket(SOCK_STREAM, ipv4);
	}

	/*
	 *	���ӵ�Զ�̵�TcpServer
	 */
	bool connect(char* szIP, U16 nPort);

	/*
	 *	�������ݣ� �����׳� GMSocketException \ GMTimeOutException
	 */
	int send( unsigned char *szData, int len) ;

	/*
	 *	�������� �����׳� GMSocketException \ GMTimeOutException
	 */
	int recv( unsigned char *, int len);

	/*
	 *	�ر�NAGLE�㷨
	 */
	bool disableNagleAlog();

	void setCnnTimeout( int sec);

private:
	int m_nCnnTimeout ;
};
#endif
