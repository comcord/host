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
	 *	����STREAM SOCKET
	 */
	bool createSocket(bool ipv4 = true)
	{
		return GMSocketBase::createSocket(SOCK_DGRAM, ipv4);
	}

	/*
	 *	����һ��UDP���� �����׳� GMSocketException \ GMTimeOutException
	 */
	int sendDgramTo( unsigned char * szData, int len, char* szip, U16 port);

	/*
	 *	���ͻ��������ݵ�
	 */
	//int sendBuffTo( unsigned char * szData, int len, char* szip, U32 port);

	/*
	 *	����һ��UDP�� �����׳� GMSocketException \ GMTimeOutException
	 */
	int recvDgramFrom( unsigned char * szData, int len, char* szip, U16& port);

	/*
	 *	����ָ��ip��port��UDP�����������������
	 */
	//int recvBuffFrom( unsigned char * szData, int len, T_IP szip, T_PORT port);


	/*
	 *	����UDP����SIZE
	 */
	void setDgramSize( int nDgramSize)
	{
		m_nDgramSize = nDgramSize;
	}
	/*
	 *	���UDP����SIZE
	 */
	int getDgramSize()
	{
		return m_nDgramSize;
	}

private:
	int m_nDgramSize;//UDP���Ĵ�С	
};
#endif
