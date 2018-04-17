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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��UI��������,��������ʧ��:Ŀ��IP:%s,Ŀ��Port:%d",m_targetIP,m_targetPort);
			return false;
		}
	}
	catch (GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��UI��������,socket�쳣��Ϣ:%s,Ŀ��IP:%s,Ŀ��Port:%d",e.what(),m_targetIP,m_targetPort);
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]��UI��������,����ͳɹ�,Ŀ��IP:%s,Ŀ��Port:%d,buff:%s",m_targetIP,m_targetPort,buffer);

	return true;
}

bool RecvUdpCmdInterfaceCtrl::reCreateRecvCmdSocket()
{
	if(0 != m_creatUiSocketTime)
	{
		unsigned int timediff = GMGetTickCount() - m_creatUiSocketTime;
		if (timediff < 1000)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[Host agent]�ؽ���UI����SOCKET,���ϴ��ؽ�ʱ�����ڲ���1s,�������ؽ�");
			return false;
		}
	}
	m_creatUiSocketTime = GMGetTickCount();
	
	try
	{
		m_udpSocket.close();
		if(!m_udpSocket.createSocket())
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ؽ���UI����SOCKET,udpsocket����ʧ��[IP:%s Port:%d]",m_localIP1,m_localPort1);
			return false;
		}

		m_udpSocket.setDgramSize(UDP_MAX_PACKET_LEN);

		if(!m_udpSocket.bind(m_localPort1,m_localIP1))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ؽ���UI����SOCKET,udpsocket��IP��Portʧ��[IP:%s Port:%d]",m_localIP1,m_localPort1);
			return false;
		}
	}
	catch(GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ؽ���UI����SOCKET,udpsocket�쳣�׳�,[IP:%s Port:%d],������Ϣ:%s",m_localIP1,m_localPort1,e.what());
		return false;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ؽ���UI����SOCKET,�ؽ��ɹ�");

	return true;
}

bool RecvUdpCmdInterfaceCtrl::Start(char localIP[16],unsigned short localPort,char targetIP[16],unsigned short targetPort)
{	
	if(localIP == NULL || targetIP ==NULL || localPort == 0 || targetPort == 0)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,0,"[host agent]�������潻���߳�,localIP:%s,localPort:%u targetIP:%s,targetPort:%u������Ч���޷�����",
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
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,socket����ʧ��,[IP:%s Port:%d]",localIP,localPort);
			return false;
		}

		m_udpSocket.setDgramSize(UDP_MAX_PACKET_LEN);

		if(!m_udpSocket.bind(m_localPort,m_localIP))
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,socket��ʧ��,[IP:%s Port:%d]",localIP,localPort);
			return false;
		}
	}
	catch(GMGeneralException & e)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,socket�׳��쳣,[IP:%s Port:%d],������Ϣ:%s",localIP,localPort,e.what());
		return false;
	}

	/***********************************************************************************/
	/*  ����һ��socket��ģʽ�����������³����£��������⣺���ø�socketȥ���ͣ�         */  
	/*  ������IP��port��Ч���Է��������ˣ�ʱ���ڸ�socket����recv�Ķ�������Ȼ��         */
	/*  �쳣����������10054���󣬶���������������ļ�������,������δ��������������   */
	/************************************************************************************/

	if(!GMFixRecvfromRtn_WSAECONNRESET(m_udpSocket.m_nSock))
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,udpsocket��������ģʽʧ��,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);
		return false;
	}
	/***************************************************************/

	m_recvActive = true;
	//m_run        = 1;
	if(!m_recvCmdThread.run())
	{
		//m_run = 0;
		m_recvActive = false;
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,ʧ��,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);
		return false;
	}

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�������潻���߳�,�ɹ�,[LocalIP:%s,LocalPort:%d,TargetIP:%s,TargetPort:%d]",m_localIP,m_localPort,m_targetIP,m_targetPort);

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
	//		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ȴ�����UI�߳��˳�");
	//		Sleep(100);
	//	}
	//}
	//Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ȴ�����UI�߳��˳�����");
	//return true;

	m_recvActive = false;
	m_udpSocket.close();

	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ȴ�����潻���߳��˳�,��ʼ");
	if(m_recvCmdThread.waitThreadExit(INFINITE))
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����潻���߳�,�˳��ɹ�,IP:%s,Port:%d",m_localIP,m_localPort);
		return true;
	}
	else
	{
		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����潻���߳�,�˳�ʧ��,IP:%s,Port:%d",m_localIP,m_localPort);
		return false;
	}
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]�ȴ�����潻���߳��˳�,����");

}

void RecvUdpCmdInterfaceCtrl::recvCmdProc(void *Param)
{
	int recvLen = 0;
	char* pbuf  = new char[2048];
	if(NULL == pbuf)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI����,udp���ջ���������ʧ��,�޷�����UI����");
		return ;
	}
	
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI�����߳�����");

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

		//Ԥ���ĻỰID���Լ�ͷ�ϼ��ĸ��ֽڵ�packetID
		memset(pbuf,0,2048);
		recvLen = 0;

		try
		{
			//�����ж�socket�Ƿ���Ч,ʧЧ���ؽ�
			if(!m_udpSocket.isSocketValid())
			{
				reCreateRecvCmdSocket();
				Sleep(100);
				continue;
			}

			recvLen = recvfrom(m_udpSocket.m_nSock,pbuf,2047,0,(sockaddr*)&sin,&addrLen);
			if(recvLen <= 0)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI����,socket�յ�����С��0");
				reCreateRecvCmdSocket();
				Sleep(100);
				continue;
			}
			if(recvLen > 1024*10)
			{
				Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI����,���ȴ���10*1024,����:%d",pbuf);
				Sleep(100);
				continue;
			}
		}
		catch (GMGeneralException & e)
		{
			Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI����,socket�쳣,������ϢΪ:%s",e.what());		
		}

		Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI����,�ɹ�,[����IP:%s,����Port:%d],����:%d,����:%s",m_localIP,m_localPort,recvLen,pbuf);

		AsynModel::AsynResultRet ret = PostMsg(m_workSessionId,MEETINGHOSTAGNET_UI_CMD,(char*)pbuf,recvLen,false);
	}

	//m_run = 2;
	Log::writeWarning(MEETING_HOST_AGENT_LOG_SUBTYPE,3000,"[host agent]����UI�����߳��˳�");
}

