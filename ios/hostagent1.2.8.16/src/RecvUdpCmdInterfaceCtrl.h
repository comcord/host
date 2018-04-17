#pragma once
#include "GMThread.h"
#include "GMUdpSocket.h"

class RecvUdpCmdInterfaceCtrl
{
public:
	//线程相关
	GMThread<RecvUdpCmdInterfaceCtrl>   m_recvCmdThread;
	bool                    m_recvActive;
	GMUdpSocket             m_udpSocket;
	int                     m_packetId;

	//重建SOCKET使用
	char                    m_localIP1[16];
	unsigned short          m_localPort1;

	//本地
	char                    m_localIP[16];
	unsigned short          m_localPort;
	//目标
	char                    m_targetIP[16];
	unsigned short          m_targetPort;
	int                     m_workSessionId;

	int                    m_run;
	unsigned int           m_creatUiSocketTime;

	void  recvCmdProc(void *Param);
	bool  reCreateRecvCmdSocket();

public:
	RecvUdpCmdInterfaceCtrl():m_recvActive(false),m_packetId(1),m_localPort(0),m_targetPort(0),
		m_recvCmdThread(&RecvUdpCmdInterfaceCtrl::recvCmdProc,this,NULL)
	{
		m_workSessionId     = 0;
		m_run               = 0;
		m_creatUiSocketTime = 0;
	}

	~RecvUdpCmdInterfaceCtrl(){}

	bool UdpSend(char* buffer,int len);
	bool Start(char localIP[16],unsigned short localPort,char targetIP[16],unsigned short targetPort);
	bool Stop();
};