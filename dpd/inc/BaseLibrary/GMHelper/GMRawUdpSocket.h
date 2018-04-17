
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RAWUDPSOCKET_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RAWUDPSOCKET_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifndef GMRAWUDPSOCKET_H
#define GMRAWUDPSOCKET_H

#ifdef WIN32
#include <WINSOCK2.H>
#endif

#ifdef WIN32

#include "GMBadUdpPackageexception.h"

//Value not specified within winsock2
//Thanks on this one goes to Bjorn Stickler author of Natas
#ifndef SIO_RCVALL
#define SIO_RCVALL  0x98000001
#endif

/**
 *IP报头
 */
typedef struct GMIP_HDR
{
	unsigned char ip_verlen;
	unsigned char ip_tos;
	unsigned short ip_totallength;
	unsigned short ip_id;
	unsigned short ip_offset;
	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned int ip_srcaddr;
	unsigned int ip_destaddr;
}GMIP_HDR;

/**
*UDP报头
*/
typedef struct GMUDP_HDR
{
	unsigned short src_portno;
	unsigned short dst_portno;
	unsigned short udp_length;
	unsigned short udp_checksum;
}GMUDP_HDR;

typedef int (WSAAPI *Recvfromfun)( SOCKET s,char* buf,int len,int flags,struct sockaddr* from,int* fromlen);

class     GMRawUdpSocket
{
	static const short SEND_IP_PLUS_UDP_HEADER_SIZE = 20+8;                                                   //允许发送的udp+ip报头的大小,ip报头不包括可选项
	static const short MAX_SEND_UDP_DATA_SIZE = 2*1024;                                                       //允许发送的最大udp包数据大小,不包括报头大小
	static const short MAX_SEND_IP_PACKAGE_SIZE = SEND_IP_PLUS_UDP_HEADER_SIZE+MAX_SEND_UDP_DATA_SIZE;   //允许发送的最大udp包大小,报头加数据

	static const short MAX_RECV_UDP_DATA_SIZE = 4*1024;                                                       //最大接收数据的大小                    
	static const short IP_PACKAGE_RECV_BUF_SIZE = SEND_IP_PLUS_UDP_HEADER_SIZE+MAX_RECV_UDP_DATA_SIZE+4;      //接收ip包的缓冲大小,ip报头可能带有可选项（所以+4)

public:
	
	GMRawUdpSocket(Recvfromfun myRecvfromfun = NULL,unsigned short MAX_RECV_UDP_PACKAGE_CONTENT_LEN = 65535);
	
	bool create();
   
	bool bind(char* IP,unsigned short port);
    
	bool close();

	int sendTo(
		char* sourceIP,
		unsigned short sourcePort,
		char* destIP,
		unsigned short destPort,
		char* buf,
		int bufLen);


	/**
	 *@brief 
	 *
	 *@param [in]  sourceIP   源地址
	 *@param [in]  sourcePort 源端口
	 *@param [in]  destIP 目的地址
	 *@param [in]  destPort 目的端口
	 *@param [in]  buf 发送的缓存区
	 *@param [in]  bufLen 发送的数据大小
	 *@return 发送的数据包大小 不包括报头
	 *@exception std::out_of_range 发送的IP包数据超过MAX_IP_PACKAGE_SIZE 抛出
	 */
	int sendTo(
		unsigned int sourceIP,
		unsigned short sourcePort,
		unsigned int destIP,
		unsigned short destPort,
		char* buf,
		int bufLen);
    
	int recvFrom(
		char *databuf, 
		int dataBufLen,
		unsigned int& sourceIP,
		unsigned short& sourcePort,
		unsigned int& destIP,
		unsigned short& destPort
		);

	int recvFrom(
		char *databuf, 
		int dataBufLen,
		char sourceIP[16],
		unsigned short* sourcePort,
		char destIP[16],
		unsigned short* destPort
		);
	
	int recvAt(
		char *databuf, 
		int dataBufLen,
		char sourceIP[16],
		unsigned short* sourcePort,
		unsigned short* destPort,
		char sniffedIP[16],
		unsigned short sniffedPort=0
		);
	
	bool sniff(BOOL bSniff);
    bool setRecvTimeout(unsigned int millsecond = 1000);

private:
	//caller delete GMUDP_HDR
	GMIP_HDR* constructIPHeader(
		char* sourceIP,
		char* destIP,
		unsigned short  itotalsize,
		GMIP_HDR* pIphdr);


	//caller delete GMUDP_HDR
	GMUDP_HDR* constructUdpHeader(
		unsigned short sourcePort,
		unsigned short destPort,
		unsigned short  iudpsize,
		GMUDP_HDR* pUdphdr);

	bool checkSocketValid();

	char* longToString(int lAddr);
 
	/**
	*@brief 
	*
	*@param 
	*@return 
	*@exception GMBadUdpPackageexception 
	*/
	int analyzeUDP(
		char *cUDPBuffer,
		int cUDPBufferLen,
		unsigned short* sourcePort,
		unsigned short* destPort,
		char* databuf,
		int databufLen);
public:
	SOCKET m_sock;

private:
	Recvfromfun m_myRecvfromfun;
	const unsigned short m_MAX_RECV_UDP_PACKAGE_CONTENT_LEN   ;           //要接收的UDP包的最大数据长度，提高性能
	//	std::string m_sourceIP;
	
};

void gmShowError(char* msg);

#else // linux


#endif

#endif
