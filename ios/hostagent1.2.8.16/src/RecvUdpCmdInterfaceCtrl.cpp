#include "stdafx.h"
#include "RecvUdpCmdInterfaceCtrl.h"
#include "innercommand.h"

#define PACKETID_MAX       2147483640
#define UDP_MAX_PACKET_LEN 64*1024
using namespace AsynModel;

extern bool JsonParser(const char * buff, int len, int & cmdId);

bool RecvUdpCmdInterfaceCtrl::UdpSend(char* buffer,int len)
{
	len = min(len,UDP_MAX_PACKET_LEN);
	int sendLen = 0;

	try
	{
		sendLen = m_udpSocket.sendDgramTo((unsigned char*)buffer,len,m_targetIP,m_targetPort);
		if(sendLen <= 0)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向UI发送命令,发送命令失败:目标IP:%s,目标Port:%d",m_targetIP,m_targetPort);
			return false;
		}
	}
	catch (GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向UI发送命令,socket异常信息:%s,目标IP:%s,目标Port:%d",e.what(),m_targetIP,m_targetPort);
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]向UI发送命令,命令发送成功,目标IP:%s,目标Port:%d,buff:%s",m_targetIP,m_targetPort,buffer);

	return true;
}

bool RecvUdpCmdInterfaceCtrl::reCreateRecvCmdSocket()
{
	if(0 != m_creatUiSocketTime)
	{
		unsigned int timediff = GMGetTickCount() - m_creatUiSocketTime;
		if (timediff < 1000)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[Host agent]重建与UI交互SOCKET,离上次重建时间现在不到1s,不进行重建");
			return false;
		}
	}
	m_creatUiSocketTime = GMGetTickCount();
	
	try
	{
		m_udpSocket.close();
		if(!m_udpSocket.createSocket())
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]重建与UI交互SOCKET,udpsocket创建失败[IP:%s Port:%d]",m_localIP1,m_localPort1);
			return false;
		}

		m_udpSocket.setDgramSize(UDP_MAX_PACKET_LEN);

		if(!m_udpSocket.bind(m_localPort1,m_localIP1))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]重建与UI交互SOCKET,udpsocket绑定IP和Port失败[IP:%s Port:%d]",m_localIP1,m_localPort1);
			return false;
		}
	}
	catch(GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]重建与UI交互SOCKET,udpsocket异常抛出,[IP:%s Port:%d],错误信息:%s",m_localIP1,m_localPort1,e.what());
		return false;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]重建与UI交互SOCKET,重建成功");

	return true;
}

bool RecvUdpCmdInterfaceCtrl::Start(char localIP[16],unsigned short localPort,char targetIP[16],unsigned short targetPort)
{	
	if(localIP == NULL || targetIP ==NULL || localPort == 0 || targetPort == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]启动界面交互线程,localIP:%s,localPort:%u targetIP:%s,targetPort:%u参数无效，无法继续",
			localIP,localPort,targetIP,targetPort);
		return false;
	}

	memcpy(m_localIP,localIP,16);
	m_localPort = localPort;

	memcpy(m_targetIP,targetIP,16);
	m_targetPort = targetPort;
	
	try
	{
		if(!m_udpSocket.createSocket())
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,socket创建失败,[IP:%s Port:%d]",localIP,localPort);
			return false;
		}

		m_udpSocket.setDgramSize(UDP_MAX_PACKET_LEN);

		if(!m_udpSocket.bind(m_localPort,m_localIP))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,socket绑定失败,[IP:%s Port:%d]",localIP,localPort);
			return false;
		}
	}
	catch(GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,socket抛出异常,[IP:%s Port:%d],错误信息:%s",localIP,localPort,e.what());
		return false;
	}

	/***********************************************************************************/
	/*  设置一下socket的模式，否则在如下场景下，会有问题：当用该socket去发送，         */  
	/*  而发送IP和port无效（对方进程已退）时，在该socket身上recv的动作，居然会         */
	/*  异常出来，返回10054错误，而不是像我们想象的继续阻塞,下面这段代码修正这个问题   */
	/************************************************************************************/

	if(!GMFixRecvfromRtn_WSAECONNRESET(m_udpSocket.m_nSock))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,udpsocket设置重连模式失败,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);
		return false;
	}
	/***************************************************************/

	m_recvActive = true;
	//m_run        = 1;
	if(!m_recvCmdThread.run())
	{
		//m_run = 0;
		m_recvActive = false;
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,失败,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);
		return false;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]启动界面交互线程,成功,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);

	return true;
}

bool RecvUdpCmdInterfaceCtrl::Stop()
{
	//if (1 == m_run)
	//{
		//m_run        = 0;
	//	m_recvActive = false;
	//	m_udpSocket.close();
	//	while(2 != m_run)
	//	{
	//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]等待接收UI线程退出");
	//		Sleep(100);
	//	}
	//}
	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]等待接收UI线程退出结束");
	//return true;

	m_recvActive = false;
	m_udpSocket.close();

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]等待与界面交互线程退出,开始");
	if(m_recvCmdThread.waitThreadExit(INFINITE))
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]与界面交互线程,退出成功,IP:%s,Port:%d",m_localIP,m_localPort);
		return true;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]与界面交互线程,退出失败,IP:%s,Port:%d",m_localIP,m_localPort);
		return false;
	}
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]等待与界面交互线程退出,结束");

}

void RecvUdpCmdInterfaceCtrl::recvCmdProc(void *Param)
{
	int recvLen = 0;
	char* pbuf  = new char[2048];
	if(NULL == pbuf)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令,udp接收缓冲区分配失败,无法接收UI命令");
		return ;
	}
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令线程启动");

	sockaddr_in sin;
	socklen_t addrLen = sizeof(sin);
	memset( &sin, 0, addrLen);

	while(m_recvActive)
	//while(1 == m_run)
	{
		//if (1 != m_run)
		//{
		//	break;
		//}

		if (false == m_recvActive)
		{
			break;
		}

		//预留的会话ID，以及头上加四个字节的packetID
		memset(pbuf,0,2048);
		recvLen = 0;

		try
		{
			//新增判断socket是否有效,失效即重建
			if(!m_udpSocket.isSocketValid())
			{
				reCreateRecvCmdSocket();
				Sleep(100);
				continue;
			}

			recvLen = recvfrom(m_udpSocket.m_nSock,pbuf,2047,0,(sockaddr*)&sin,&addrLen);
			if(recvLen <= 0)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令,socket收到长度小于0");
				reCreateRecvCmdSocket();
				Sleep(100);
				continue;
			}
			if(recvLen > 1024*10)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令,长度大于10*1024,命令:%d",pbuf);
				Sleep(100);
				continue;
			}
		}
		catch (GMGeneralException & e)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令,socket异常,错误信息为:%s",e.what());		
		}

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令,成功,[本地IP:%s,本地Port:%d],长度:%d,内容:%s",m_localIP,m_localPort,recvLen,pbuf);

		AsynModel::AsynResultRet ret = PostMsg(m_workSessionId,MEETINGHOSTAGNET_UI_CMD,(char*)pbuf,recvLen,false);
	}

	//m_run = 2;
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]接收UI命令线程退出");
}

