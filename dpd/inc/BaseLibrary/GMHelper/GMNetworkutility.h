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
	//mac == NULL 表示不获取 mac
	//ip == NULL 表示不获取 ip ip主机序
	static bool getGatewayInfo(GMMAC* mac,GMIP* pIP);

	//多个网卡的只返回第一个
	static bool getLocalMac(GMMAC& mac);

	static void formatMac(char mac_str[13],const GMMAC& mac);

	static bool fixRecvfromRtn_WSAECONNRESET(SOCKET sd);


	/*
	brief:
	解析域名为GMIP
	params:
		domainname
			[in] 待解析域名
	return:0 解析失败 否则为解析出的GMIP
	*/
	static GMIP parseDomainName(const char* domainname);


	/*
	brief:
		ping 目标主机
	params:
		replyStatus
			[out] icmp回应状态
		rtt
			[out] 往返延时
		echoIP
			[out] 回应地址
		destIP
			[in] 目标地址
		timeout
			[in] 超时时间
		ttl
			[in] ttl
	return:
		0 成功
		-1 获取函数地址失败
		-2 调用函数失败
	note:
	icmp回应状态
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
		ping 目标主机
	params:
		dest
			[in] 目标地址,命令行ping 接受的地址形式
		timeout
			[in] 超时时间
	return:
		成功 失败
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
