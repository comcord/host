#ifndef __P2PCONNAPI_H__
#define __P2PCONNAPI_H__

class IP2PConnCallBack
{
public:
	IP2PConnCallBack() {}
	virtual ~IP2PConnCallBack() {}

public:

	/*
	*   ���յ��Զ�����
	*   ������
	*       accountId     �� ���ݷ��ͷ���Ѷ��
	*       buf           �� ���յ�������buf
	*       len           �� ���յ����ݳ���
	*/
	virtual int OnRecvData(const char* accountId,const char* buf,const unsigned short len) = 0;

	/*
	*   ���ݷ��ͽ��
	*   ������
	*       accountId     �� Ŀ����Ѷ��
	*       rc            �� ���ͽ��  0���ɹ�  -1������ʧ��   -2 �� ���ͳ�ʱ
	*       userData      �� �û�����
	*/
	virtual int OnSendDataResult(const char* accountId,int rc,const unsigned long long userData) = 0;
};

class IP2PConn
{
public:
	IP2PConn() {}
	virtual ~IP2PConn() {}

public:

	/*
	*   ��ʼ��P2PConn
	*/
	virtual int Start(IP2PConnCallBack* cb) = 0;

	/*
	*   ��������
	*   ������
	*       peerAccountId �� �Զ���Ѷ��
	*       buf    �� ��������
	*       len    �� �������ݳ���
	*       userData �� �û�����
	*/
	virtual int SendData(const char* peerAccountId,const char* buf,const unsigned short len,const unsigned long long userData) = 0;

	/*
	*   ֹͣP2PConn
	*/
	virtual int Stop() = 0;
};

extern "C"
{
	/*
	*   ����P2PConn����
	*   ������
	*       accountId  �� ������Ѷ��
	*/
	IP2PConn* CreateP2PConn(const char* accountId);

	/*
	*   ɾ��P2PConn����
	*/
	void DeleteP2PConn(IP2PConn* pConn);

}

#endif
