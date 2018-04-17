#pragma once

//��������  �������·����host�ͻ���֮�������
#define  GET_HOST_SERVER_REQ_CMD        6601
#define  GET_HOST_SERVER_RESP_CMD       6602

//�������ֻ��ID���ɣ���������������£�

struct GetHostServerRespCmd
{
	char              userIp[16];
	unsigned short    userPort;
	int               result; //0��ʾ�п��ýڵ㣻-1��ʾû�п��ýڵ�

	int Seralize(char* buff, size_t len)
	{
		if(buff == NULL || len < GetRealSize())
			return -1;

		int serLen = 0;
		memcpy(buff+serLen, &userIp, sizeof(userIp));
		serLen += sizeof(userIp);

		memcpy(buff+serLen, &userPort, sizeof(userPort));
		serLen += sizeof(userPort);

		memcpy(buff+serLen,&result,sizeof(result));
		serLen += sizeof(result);

		return serLen;
	}

	int UnSeralize(const char * buff,size_t len)
	{
		if(buff == NULL || len <  sizeof(userIp) + sizeof(userPort)+sizeof(result))
			return -1;

		int unSerLen = 0;
		memcpy(&userIp, buff+unSerLen, sizeof(userIp));
		unSerLen += sizeof(userIp);

		memcpy(&userPort,buff+unSerLen,sizeof(userPort));
		unSerLen += sizeof(userPort);

		memcpy(&result,buff+unSerLen,sizeof(result));
		unSerLen += sizeof(result);

		return unSerLen;
	}

	unsigned int GetRealSize()
	{
		int realSize = sizeof(userIp) + sizeof(userPort) +sizeof(result);

		return realSize;
	}
};