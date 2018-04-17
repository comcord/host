/****************************************************************************                                                                          
* ISender.h                       
* @version 2.2
* @date    2008.9.20
* @author  �첽ģ��С��
* @fuction ����Sender����̳еĽӿ�
* Copyright (c) GOUMANG Corporation. All rights reserved.
****************************************************************************/
#ifndef ASYN_ISENDER_H_GOUMANG_2008
#define ASYN_ISENDER_H_GOUMANG_2008

#include "GMEmbedSmartPtr.h"
namespace AsynModel
{
	/**
	*��νSender��һ��ͨ�������ķ�װ��ϵͳĬ���ṩ3��Sender��TCPSender��UDPSender��LocalSender��
	*�ֱ��װTCP���ͣ�UDP�����뱾����Ϣ������Ϊ���û��ɶ����Լ���Sender���̳л�����ISender�ӿ�
	*����װ�Լ���ͨ����Ϊ���ʼ������ߴ���ȣ���ͨ��Session��PostMsg�������Զ���Sender���첽ģ��
	*ͨ��ISender�ӿ��������û���ͨ����Ϊ��Ŀ�ĵ�ʵ��ͨ��
	*ע���Զ���Sender�����첽ģ�Ͱ汾��û��֧��
	*/
	class ISender
	{
	public:
		/**
		*����֧�ֵ�Sender������
		*/
		enum SenderType
		{
			ASYNLOCALSENDER = 0,///<����Sender
			ASYNUDPSENDER,///<UDPSender
			ASYNTCPSENDER///<TCPSender
		};
		/**
		* ���� �첽���ͽӿڣ��û�����Ҫ���͵������볤�ȣ��û��Զ���Sender������ʵ�ָýӿ�
		* @param buff char����ָ�� �û������׵�ַ
		* @param len  unsigned int���� �û����ݳ���
		* @return int���� 0���ɹ�������ʧ��
		*/
		virtual int AsynSend(char* buff,unsigned int len) = 0;
		/**
		* ���� ��ȡ���ص�IPPort�������� ��ҪUDP��TCP��������ʵ��
		* @param localIP unsigned long���ʹ������� 
		* @param localPort unsigned short���ʹ������� 
		* @return ��
		*/
		virtual void GetLocalIPPort(unsigned long &localIP,unsigned short &localPort)const = 0;
		/**
		* ���� ��ȡԶ�̵�IPPort�������� ��ҪUDP��TCP��������ʵ��
		* @param remoteIP unsigned long���ʹ������� 
		* @param remotePort unsigned short���ʹ������� 
		* @return ��
		*/
		virtual void GetRemoteIPPort(unsigned long &remoteIP,unsigned short &remotePort)const = 0;
		/**
		* ���� ���ü�������һ
		* @param ��
		* @return ��
		*/
		virtual void AddRef()  = 0;
		/**
		* ���� ���ü�������һ
		* @param ��
		* @return ��
		*/
		virtual void Release() = 0;
		/**
		* ���� ��ȡSender������
		* @see SenderType
		* @param ��
		* @return int���� ֵΪ���ʹ���
		*/
		virtual int  GetType()const  = 0;
		/**
		* ���� �ж�Sender�Ƿ���Ч
		* @param ��
		* @return bool���� true���ɹ���false��ʧ��
		*/
		virtual bool IsValid()const = 0;
	};

typedef GMEmbedSmartPtr<ISender> SptrISender;
#define NULLSptrISender SptrISender(NULL) 
}
#endif //ASYN_ISENDER_H_GOUMANG_2008
