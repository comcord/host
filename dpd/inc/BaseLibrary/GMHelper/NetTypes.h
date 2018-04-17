#pragma once

#include <string.h>

typedef unsigned int GMIP;
typedef unsigned short GMPORT;

struct V6IP{char ip[16];};

class GMMAC
{
public:
	GMMAC(void)
	{
		memset(m_val,0,6);
	}


	bool operator!=(const GMMAC& r)const
	{
		return !(*this == r);
	}

	bool operator==(const GMMAC& r)const
	{
		return 0 == memcmp(this->m_val,r.m_val,6);
	}

	unsigned char m_val[6];
};

class GMAddr
{
public: 
	GMAddr():m_ip(0),m_port(0){}

	bool operator<(const GMAddr& val)const
	{
		if( m_ip < val.m_ip)
			return true;

		else if( m_ip > val.m_ip)
			return false;

		return m_port < val.m_port;

	}

	bool operator > (const GMAddr& val) const
	{
		if( m_ip > val.m_ip)
			return true;

		else if( m_ip < val.m_ip)
			return false;

		return m_port > val.m_port;
	}

	bool operator==(const GMAddr& val)const{
		return m_port == val.m_port && m_ip == val.m_ip; 
	}

	bool operator!=(const GMAddr& val)const{
		return m_port != val.m_port || m_ip != val.m_ip; 
	}

	GMIP m_ip;//主机序
	GMPORT m_port;//主机序
};

class GMTcpAddr
{
public:
	bool operator<(const GMTcpAddr& val)const
	{
		if( m_localAddr < val.m_localAddr)
			return true;
		else if( m_localAddr > val.m_localAddr)
			return false;
		
		return m_remoteAddr < val.m_remoteAddr;
	}

	GMAddr m_localAddr;
	GMAddr m_remoteAddr;
};

class GMAddress
{
public:
	GMAddress(GMIP ip,GMPORT port = 0):m_ip(ip),m_port(port){}

	GMAddress(void):m_ip(0),m_port(0){}

	char* ip();

	char* toStr();									   

public:
	GMIP m_ip;//主机序
	GMPORT m_port;//主机序

private:
	char m_buf[32];
};

//add for ipv6
class GMAddrEx
{
public:
	
	GMAddrEx(bool v4 = true) : m_ip(0), m_port(0), m_v4(v4) 
	{
		if (!v4)
			memset(&m_v6ip, 0, 16);
	}

	bool operator<(const GMAddrEx &val) const
	{
		if (m_v4 != val.m_v4)
			return m_v4 < val.m_v4;

		if (m_port != val.m_port)
			return m_port < val.m_port;

		if (m_v4)
			return m_ip < val.m_ip;

		return memcmp(&m_v6ip, &val.m_v6ip, 16) < 0;
	}

	bool operator==(const GMAddrEx &val) const
	{
		if (m_v4)
			return m_v4 == val.m_v4 && m_port == val.m_port && m_ip == val.m_ip; 
		else
			return m_v4 == val.m_v4 && m_port == val.m_port && memcmp(&m_v6ip, &val.m_v6ip, 16) == 0;
	}

	bool operator!=(const GMAddrEx &val) const
	{
		if (m_v4)
			return m_v4 != val.m_v4 || m_port != val.m_port || m_ip != val.m_ip; 
		else
			return m_v4 != val.m_v4 || m_port != val.m_port || memcmp(&m_v6ip, &val.m_v6ip, 16) != 0;
	}

	GMIP m_ip;		//v4 ip  host sequence
	GMPORT m_port;	//port   host sequence
	bool m_v4;		//protocol type, true means v4, false means v6 
	V6IP m_v6ip;	//v6 ip
};

class GMTcpAddrEx
{
public:
	bool operator<(const GMTcpAddrEx& val) const
	{
		if( m_localAddr != val.m_localAddr)
			return m_localAddr < val.m_localAddr;

		return m_remoteAddr < val.m_remoteAddr;
	}

	GMAddrEx m_localAddr;
	GMAddrEx m_remoteAddr;
};

class GMAddressEx
{
public:
	GMAddressEx(V6IP ip, GMPORT port = 0):m_ip(ip),m_port(port){}

	GMAddressEx(void):m_port(0){memset(&m_ip, 0, sizeof(m_ip));}

	char* ip();

	char* toStr();									   

public:
	V6IP m_ip;//主机序
	GMPORT m_port;//主机序

private:
	char m_buf[48];
};
