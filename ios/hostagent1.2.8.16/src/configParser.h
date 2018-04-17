#pragma once
#include <list>
#include <string>

struct serverInfo
{
	serverInfo():port(0)
	{}

	std::string ip;
	unsigned short port;

	bool operator<(const serverInfo &info) const 
	{
		if( ip < info.ip)
		{
			return true;
		}
		else if(ip == info.ip)
		{
			return port < info.port;
		}
		else
		{
			return false;
		}
	}

	serverInfo & operator=(const serverInfo &src)
	{
		if(this != &src)
		{
			ip   = src.ip;
			port = src.port;
		}

		return *this;
	}

};



class  CConfigParser_STPIPMAP
{
public:
	CConfigParser_STPIPMAP(){}
	~CConfigParser_STPIPMAP(){}

	std::map<serverInfo,serverInfo>   stpIPMap;
	GMRWLock                          stpIPMapRWLock;                           

	bool           GetMappedStpIp(serverInfo & info,serverInfo & outInfo);

	bool           SetMappedStpIp(serverInfo & info,serverInfo & outInfo);
};
