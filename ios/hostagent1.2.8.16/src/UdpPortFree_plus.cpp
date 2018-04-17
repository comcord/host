#include "stdafx.h"

static HANDLE g_ListenPortMutex = NULL;

#pragma data_seg("LiveStreamReceiverPort")				//全局UDP端口分配，从1万到2万之间

volatile static USHORT g_ListenPortMin = 10000;
volatile static USHORT g_ListenPortMax = 60000;
volatile static USHORT g_TryListenPort = 10000;

volatile static USHORT g_TCPListenPortMin = 10000;
volatile static USHORT g_TCPListenPortMax = 60000;
volatile static USHORT g_TCPTryListenPort = 10000;

//extern string sim_ip;


#pragma data_seg()
#pragma   comment(linker, "/section:LiveStreamReceiverPort,RWS ") 

void InitListenPortCriticalSection( void )				//端口分配进程锁
{
	g_ListenPortMutex = CreateMutex(NULL, FALSE, "LiveStreamReceiverPort");
	assert(g_ListenPortMutex != NULL );
}

void UninitListenPortCriticalSection( void )
{
	CloseHandle(g_ListenPortMutex);
}

bool GMIsUdpPortFreeForTest(T_IP ip,T_PORT port)
{
	if(!ISPORTVALID(port))
		return false;

	bool bRtn = true;
	GMUdpSocket udp;
	try
	{
		////T_IP localIP = {0};
		//if (!GMGetLocalHostActiveIp(ip))
		//	IP_COPY(ip,"127.0.0.1");

		udp.createSocket();
		udp.bind(port,ip);
	}
	catch (GMSocketException &e)
	{
		GMMyTRACE("端口Udp:%d被占用! %s",port,e.getExceptionMsg());
		bRtn = false;
	}

	udp.close();
	return bRtn;
}

bool GMIsTcpPortFreeForTest(T_IP ip,T_PORT port)
{
	if(!ISPORTVALID(port))
		return false;

	bool bRtn = true;
	GMTcpServer tcp;
	try
	{
		////T_IP localIP = {0};
		//if (!GMGetLocalHostActiveIp(ip))
		//	IP_COPY(ip,"127.0.0.1");

		tcp.createSocket();
		tcp.bind(port,ip);
	}
	catch (GMSocketException &e)
	{
		GMMyTRACE("端口tcp:%d被占用! %s",port,e.getExceptionMsg());
		bRtn = false;
	}

	tcp.close();
	return bRtn;
}

unsigned short GetFreeUDPPort( /*void*/string sim_ip )
{
	int count = 0;

	do{

		//锁定端口分配进程锁
		DWORD lockWait = WaitForSingleObject(g_ListenPortMutex, INFINITE);

		//assert( lockWait == WAIT_OBJECT_0  );

		USHORT usePort = g_TryListenPort;

		g_TryListenPort++;
		if( g_TryListenPort >= g_ListenPortMax )
		{
			g_TryListenPort = g_ListenPortMin;				//超过最大后折返
		}

		ReleaseMutex(g_ListenPortMutex);					//释放锁

		if ( GMIsUdpPortFreeForTest((char *)sim_ip.c_str(),usePort) )						//判断是否空闲
		{
			return usePort;
		}
		++count;
	}while( count< 5000 );									//重试5000次
	return -1;

}

unsigned short GetFreeTCPPort(/*void*/string sim_ip)
{
	int count = 0;

	do{

		//锁定端口分配进程锁
		DWORD lockWait = WaitForSingleObject(g_ListenPortMutex, INFINITE);

		//assert( lockWait == WAIT_OBJECT_0  );

		USHORT usePort = g_TCPTryListenPort;

		g_TCPTryListenPort++;
		if( g_TCPTryListenPort >= g_TCPListenPortMax )
		{
			g_TCPTryListenPort = g_TCPListenPortMin;				//超过最大后折返
		}

		ReleaseMutex(g_ListenPortMutex);					//释放锁

		if ( GMIsTcpPortFree(usePort) )						//判断是否空闲
		{
			return usePort;
		}
		++count;
	}while( count< 5000 );									//重试5000次
	return -1;
}




