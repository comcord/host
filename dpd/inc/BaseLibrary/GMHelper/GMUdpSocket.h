#ifndef GMUDPSOCKET_H_2006_CHANNELSOFT
#define GMUDPSOCKET_H_2006_CHANNELSOFT

#include "GMSocketBase.h"

class  GMUdpSocket : public GMSocketBase	
{
public:

	GMUdpSocket(SOCKET nSock = INVALID_SOCKET, bool ipv4 = true)
		: GMSocketBase(ipv4)
		, m_nDgramSize(1024 * 4)
	{
		if(nSock != INVALID_SOCKET)
			m_nSock = nSock;
	}
	
	/*
	 *	创建STREAM SOCKET
	 */
	bool createSocket(bool ipv4 = true)
	{
		return GMSocketBase::createSocket(SOCK_DGRAM, ipv4);
	}

	/*
	 *	发送一个UDP包到 错误抛出 GMSocketException \ GMTimeOutException
	 */
	int sendDgramTo( unsigned char * szData, int len, char* szip, U16 port);

	/*
	 *	发送缓冲区数据到
	 */
	//int sendBuffTo( unsigned char * szData, int len, char* szip, U32 port);

	/*
	 *	接收一个UDP包 错误抛出 GMSocketException \ GMTimeOutException
	 */
	int recvDgramFrom( unsigned char * szData, int len, char* szip, U16& port);

	/*
	 *	接收指定ip和port的UDP包（多个）到缓冲区
	 */
	//int recvBuffFrom( unsigned char * szData, int len, T_IP szip, T_PORT port);


	/*
	 *	设置UDP包的SIZE
	 */
	void setDgramSize( int nDgramSize)
	{
		m_nDgramSize = nDgramSize;
	}
	/*
	 *	获得UDP包的SIZE
	 */
	int getDgramSize()
	{
		return m_nDgramSize;
	}

private:
	int m_nDgramSize;//UDP包的大小	
};
#endif
