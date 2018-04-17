#ifndef __P2PCONNAPI_H__
#define __P2PCONNAPI_H__

class IP2PConnCallBack
{
public:
	IP2PConnCallBack() {}
	virtual ~IP2PConnCallBack() {}

public:
	/*
	*   ���ӽ��
	*   ������
	*       accountId �� �Զ���Ѷ��
	*       rc  �� �����롣 0�� ���ӳɹ�  ��0�� ����ʧ��
	*       rd  �� ��������
	*/
	virtual int OnConnectResult(const char* accountId,int rc,const char* rd) = 0;

	/*
	*   ���ӶϿ�֪ͨ
	*   ������
	*       accountId �� �Զ���Ѷ��
	*       reason  �� ���ӶϿ�ԭ��
	*                  1���Զ������Ͽ�����
	*                  2��������hostserver���ӶϿ�
	*                  3����Զ�������ʱ
	*/
	virtual int OnDisConnect(const char* accountId,int reason) = 0;

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
	*       rc            �� ���ͽ��  0���ɹ�  -1������ʧ��
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
	*   ���ûص�����
	*/
	virtual int SetP2PConnCB(IP2PConnCallBack* cb) = 0;

	/*
	*   ��ʼ����
	*/
	virtual int Connect() = 0;

	/*
	*   ��������
	*   ������
	*       buf    �� ��������
	*       len    �� �������ݳ���
	*       userData �� �û�����
	*/
	virtual int SendData(const char* buf,const unsigned short len,const unsigned long long userData) = 0;

	/*
	*   �Ͽ�����
	*/
	virtual int DisConnect() = 0;
};

/*
*   ���յ���������ص�����
*/
typedef void (*P2PConnectCB)(const char* peerAccountId,IP2PConn* conn);

extern "C"
{
	/*
	*   ����P2PConn����
	*   ������
	*       accountId  �� ������Ѷ��
	*       peerAccountId  �� �Զ���Ѷ��
	*       cb  �� �ص�����
	*/
	IP2PConn* CreateP2PConn(const char* accountId,const char* peerAccountId,IP2PConnCallBack* cb);

	/*
	*   ɾ��P2PConn����
	*/
	void DeleteP2PConn(IP2PConn* pConn);

	/*
	*   ����P2P���ӻص�����
	*/
	void SetP2PConnectCallback(P2PConnectCB* cb);
}

#endif
