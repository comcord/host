#pragma once

#include <list>
#include <string>
#include "GMTcpSocket.h"
#include "GMSocketException.h"
#include "GMSZ.h"
#include "SysDiffDefine.h"

/**
*	web���صĿͻ�����
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
	*	�������������url,���server��Host��get�����ݣ�server ip��server�˿�
	*/
	static bool parseURL( const char * pUrl, char * pHost, char * pGet,
		char * pIp, unsigned short * pPort, char * pRefer = static_cast<char*>(NULL) );

	/**
	*	������purl���ӵ�socket
	*/
	bool connect( const char * pUrl );

	/**
	*	���ý��ճ�ʱ
	*/
	bool setRecvTimeout( unsigned int second = 10 );

	void setConnectTimeOut( unsigned int second = 10 );

	/**
	*	��һ�������ص�url

	-1 ����URLʧ��
	-2 û�н�������
	-3 ��url������ͨ���쳣
	-4 http��Ӧͷ���������մ�С
	-5 ��Ӧͷ���� ������
	*/
	int openURL( const char * pUrl,
		bool isKeepLive = false,
		unsigned int offset = 0,
		unsigned int len = 0,
		std::string* httpRespHead = static_cast<std::string*>(NULL) );

	/**
	*	��ô����صĳ��ȣ��ڵ���openURL֮���ٵ����������
	*/
	int getContentSize()
	{
		return m_nContentSize;
	}

	/**
	*	������������
	*/
	int readData( char * pBuff, int size );

	/**
	*	�ر���web server������
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

	GMTcpClient m_client; ///<��web server���ӵ�tcpsock��
	int m_nContentSize; ///<���ص����ݳ���
	bool m_bConned; ///<�Ƿ��Ѿ���������
};
