#pragma once

#include "GMSocketBase.h"
#include "NetTypes.h"
#include <string>

class  GMNetworkUtility
{
public:
	enum NET_UNIT
	{
		U_B,
		U_KB,
		U_MB,
		U_GB
	};

public:
	//mac == NULL ��ʾ����ȡ mac
	//ip == NULL ��ʾ����ȡ ip ip������
	static bool getGatewayInfo(GMMAC* mac,GMIP* pIP);

	//���������ֻ���ص�һ��
	static bool getLocalMac(GMMAC& mac);

	static void formatMac(char mac_str[13],const GMMAC& mac);

	static bool fixRecvfromRtn_WSAECONNRESET(SOCKET sd);


	/*
	brief:
	��������ΪGMIP
	params:
		domainname
			[in] ����������
	return:0 ����ʧ�� ����Ϊ��������GMIP
	*/
	static GMIP parseDomainName(const char* domainname);


	/*
	brief:
		ping Ŀ������
	params:
		replyStatus
			[out] icmp��Ӧ״̬
		rtt
			[out] ������ʱ
		echoIP
			[out] ��Ӧ��ַ
		destIP
			[in] Ŀ���ַ
		timeout
			[in] ��ʱʱ��
		ttl
			[in] ttl
	return:
		0 �ɹ�
		-1 ��ȡ������ַʧ��
		-2 ���ú���ʧ��
	note:
	icmp��Ӧ״̬
	#define IP_SUCCESS                  0
	#define IP_BUF_TOO_SMALL            (IP_STATUS_BASE + 1)
	#define IP_DEST_NET_UNREACHABLE     (IP_STATUS_BASE + 2)
	#define IP_DEST_HOST_UNREACHABLE    (IP_STATUS_BASE + 3)
	#define IP_DEST_PROT_UNREACHABLE    (IP_STATUS_BASE + 4)
	#define IP_DEST_PORT_UNREACHABLE    (IP_STATUS_BASE + 5)
	#define IP_NO_RESOURCES             (IP_STATUS_BASE + 6)
	#define IP_BAD_OPTION               (IP_STATUS_BASE + 7)
	#define IP_HW_ERROR                 (IP_STATUS_BASE + 8)
	#define IP_PACKET_TOO_BIG           (IP_STATUS_BASE + 9)
	#define IP_REQ_TIMED_OUT            (IP_STATUS_BASE + 10)
	#define IP_BAD_REQ                  (IP_STATUS_BASE + 11)
	#define IP_BAD_ROUTE                (IP_STATUS_BASE + 12)
	#define IP_TTL_EXPIRED_TRANSIT      (IP_STATUS_BASE + 13)
	#define IP_TTL_EXPIRED_REASSEM      (IP_STATUS_BASE + 14)
	#define IP_PARAM_PROBLEM            (IP_STATUS_BASE + 15)
	#define IP_SOURCE_QUENCH            (IP_STATUS_BASE + 16)
	#define IP_OPTION_TOO_BIG           (IP_STATUS_BASE + 17)
	#define IP_BAD_DESTINATION          (IP_STATUS_BASE + 18)
	*/
#ifdef WIN32
static int ping(unsigned int& replyStatus, unsigned int& rtt,GMIP& echoIP,GMIP destIP,unsigned int timeout,unsigned char ttl = 100);
#endif

	/*
	brief:
		ping Ŀ������
	params:
		dest
			[in] Ŀ���ַ,������ping ���ܵĵ�ַ��ʽ
		timeout
			[in] ��ʱʱ��
	return:
		�ɹ� ʧ��
	note:
	*/
#ifdef WIN32
	static bool shell_ping(char* dest,unsigned int times,unsigned int timeout,unsigned int& lostNum,unsigned int& recvNum,unsigned int& average);
#endif

	static bool getLocalActiveIp(GMIP& localIP);

	static double transformUnit(unsigned int bytes,NET_UNIT& unit);

	static std::string getFlowText(unsigned int bytes);


	static std::string getSpeedText(unsigned int Bps);
};
