#ifndef __P2PCONN_H__
#define __P2PCONN_H__

#include "P2PConnApi.h"
#include "gmhelperLibPub.h"
#include "GMEvent.h"
#include <map>
#include <string>
#include <list>
#include <set>

#ifndef U64
#define U64 unsigned long long
#endif

#define SEND_PACKET_SIZE		1024
#define MEM_CACHE_INIT_COUNT	64
#define DATA_SEND_MAX_WAIT_TIME 1000

enum PACKET_SEND_STATUS
{
	SEND_STATUS_RECV_ACK = 0,
	SEND_STATUS_NOT_SEND = 1,
	SEND_STATUS_WAIT_RESULT = 2,
	SEND_STATUS_SEND_FAIL = 3,
	SEND_STATUS_SEND_TIMEOUT = 4
};

struct p2p_conn_head
{
	unsigned int		m_msgId;
	unsigned char		m_packNum;
	unsigned char		m_packId;
	unsigned short		m_msgLen;

	p2p_conn_head()
	{
		m_msgId = 0;
		m_packNum = 0;
		m_packId = 0;
		m_msgLen = 0;
	}

	int Serilize(char* buf,int len)
	{
		if (GetRealSize() > len)
		{
			return 0;
		}

		int serLen = 0;
		memcpy(buf + serLen,&m_msgId,sizeof(m_msgId));
		serLen += sizeof(m_msgId);

		memcpy(buf + serLen,&m_packNum,sizeof(m_packNum));
		serLen += sizeof(m_packNum);

		memcpy(buf + serLen,&m_packId,sizeof(m_packId));
		serLen += sizeof(m_packId);

		memcpy(buf + serLen,&m_msgLen,sizeof(m_msgLen));
		serLen += sizeof(m_msgLen);

		return serLen;
	}

	int UnSerilize(char* buf,int len)
	{
		if (GetRealSize() > len)
		{
			return 0;
		}

		int unSerLen = 0;
		memcpy(&m_msgId,buf + unSerLen,sizeof(m_msgId));
		unSerLen += sizeof(m_msgId);

		memcpy(&m_packNum,buf + unSerLen,sizeof(m_packNum));
		unSerLen += sizeof(m_packNum);

		memcpy(&m_packId,buf + unSerLen,sizeof(m_packId));
		unSerLen += sizeof(m_packId);

		memcpy(&m_msgLen,buf + unSerLen,sizeof(m_msgLen));
		unSerLen += sizeof(m_msgLen);

		return unSerLen;
	}

	int GetRealSize()
	{
		return sizeof(m_msgId) + sizeof(m_packNum) + sizeof(m_packId) + sizeof(m_msgLen);
	}
};

struct send_packet_info
{
	unsigned short	m_packetId;
	unsigned short	m_dataLen;
	unsigned short	m_sendTimes;
	unsigned short	m_sendStatus;
	char*			m_dataPtr;
	U64				m_lastSendTime;

	send_packet_info()
	{
		m_packetId = 0;
		m_dataLen = 0;
		m_sendTimes = 0;
		m_sendStatus = SEND_STATUS_NOT_SEND;
		m_dataPtr = NULL;
		m_lastSendTime = 0;
	}
};

struct send_data_info
{
//	const char*		m_dataPtr;
	char			m_peerUserId[16];
	unsigned short	m_dataLen;
	unsigned int	m_msgId;
	U64				m_sendTime;
	U64				m_userData;
	std::map<unsigned short,U64> m_sendPacketsUids;   //packetId -> dataUid

	send_data_info()
	{
		memset(m_peerUserId,0,sizeof(m_peerUserId));
		m_dataLen = 0;
		m_sendTime = 0;
		m_userData = 0;
		m_msgId = 0;
	}
};

struct recv_data_info
{
	char			m_peerUserId[16];
	unsigned char	m_packNum;
	unsigned short	m_dataLen;
	unsigned int	m_msgId;
	char*			m_msgPtr;
	U64				m_recvTime;
	std::set<unsigned char>		m_recvPackList;

	recv_data_info()
	{
		memset(m_peerUserId,0,sizeof(m_peerUserId));
		m_packNum = 0;
		m_dataLen = 0;
		m_msgId = 0;
		m_msgPtr = NULL;
		m_recvTime = 0;
	}
};

class CMemCacheMgr
{
public:
	CMemCacheMgr();
	~CMemCacheMgr();

public:
	bool Init(unsigned short packetSize = SEND_PACKET_SIZE,unsigned int count = MEM_CACHE_INIT_COUNT);
	char* GetPacket();
	void FreePacket(char*);
	void Destroy();

private:
	std::set<char*> m_freePackets;
	GMLock			m_freePacketsLock;
	std::set<char*> m_usePackets;
	GMLock			m_usePacketsLock;
	unsigned short  m_packetSize;
	std::set<char*> m_memBlocks;
	GMLock			m_memBlocksLock;
};

class CSendDataGUID
{
public:
	CSendDataGUID() {}
	~CSendDataGUID() {}

	static U64 GetSendDataUID();
	static unsigned int GetSendMsgUID();

private:
	static U64 m_dataUID;
	static GMLock m_dataUIDLock;
	static unsigned int m_msgId;
};

class CP2PConn : public IP2PConn
{
public:
	static IP2PConn* Create(const char* userId);
	static void Destroy(IP2PConn* pConn);

public:
	virtual int Start(IP2PConnCallBack* cb);
	virtual int SendData(const char* peerAccountId,const char* buf,const unsigned short len,const unsigned long long userData);
	virtual int Stop();

	void OnRecvTransData(const char* fromUserId,const char* buf, const unsigned short len);
	void OnRecvTransResult(const unsigned long long userData,int result);

protected:
	void SendDataProc(void* param);
	void FreeSendData(int msgId);
	void CheckSendData();
	void CheckRecvData();

	CP2PConn(void);
	virtual ~CP2PConn(void);

	static void OnRecvTransDataCB(const char* fromUserId,const char* buf, const unsigned short len);
	static void OnTransDataResultCB(const unsigned long long userData,int result);

private:
	IP2PConnCallBack*  m_pCallBack;
	std::list<send_data_info> m_sendDataList;
	GMLock                    m_sendDataListLock;
	std::map<U64,send_packet_info>  m_sendPackets;  //dataUid -> send_packet_info
	GMLock                    m_sendPacketsLock;
	GMThread<CP2PConn>        m_sendDataProc;
	CMemCacheMgr			  m_memcache;
	bool					  m_bStopped;

	std::map<std::string,recv_data_info> m_recvMsgs;
	GMLock					  m_recvMsgLock;

	GMSysEvent				  m_stopEvent;

	static CP2PConn*		  m_pInstance;
	static std::string		  m_userId;
};

#endif
