#include "StdAfx.h"
#include "P2PConn.h"
#include "MeetingHostAgent.h"


CP2PConn* CP2PConn::m_pInstance = NULL;
std::string	CP2PConn::m_userId;

IP2PConn* CreateP2PConn(const char* accountId)
{
	return CP2PConn::Create(accountId);
}

void DeleteP2PConn(IP2PConn* pConn)
{
	CP2PConn::Destroy(pConn);
}

IP2PConn* CP2PConn::Create(const char* userId)
{
	if (NULL == userId || strlen(userId) == 0)
	{
		return NULL;
	}

	if (m_pInstance == NULL)
	{
		m_pInstance = new CP2PConn();
		m_userId = userId;
	}
	else if (m_userId.compare(userId) != 0)
	{
		m_pInstance->Stop();
		m_userId = userId;
	}

	return m_pInstance;
}

void CP2PConn::Destroy(IP2PConn* pConn)
{
	if (pConn != m_pInstance)
	{
		return;
	}
	if (m_pInstance != NULL)
	{
		m_pInstance->Stop();
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CP2PConn::OnRecvTransDataCB(const char* fromUserId,const char* buf, const unsigned short len)
{
	if (m_pInstance)
	{
		m_pInstance->OnRecvTransData(fromUserId,buf,len);
	}
}

void CP2PConn::OnTransDataResultCB(const unsigned long long userData,int result)
{
	if (m_pInstance)
	{
		m_pInstance->OnRecvTransResult(userData,result);
	}
}

CP2PConn::CP2PConn(void)
{
	m_pCallBack = NULL;
	m_bStopped = false;
}


CP2PConn::~CP2PConn(void)
{
}

int CP2PConn::Start(IP2PConnCallBack* cb)
{
	m_pCallBack = cb;
	MHClient_SetTransDataCB(&CP2PConn::OnRecvTransDataCB);
	MHClient_SetTransDataResultCB(&CP2PConn::OnTransDataResultCB);
	m_memcache.Init();
	m_bStopped = false;
	m_stopEvent.reset();
	m_sendDataProc.init(this,&CP2PConn::SendDataProc,NULL);
	m_sendDataProc.run();
	return 0;
}

int CP2PConn::SendData(const char* peerAccountId,const char* buf,const unsigned short len,const unsigned long long userData)
{
	if (NULL == peerAccountId || NULL == buf || len == 0)
	{
		return -1;
	}

	p2p_conn_head head;
	int pack_payload = SEND_PACKET_SIZE - head.GetRealSize();
	int pack_count = len / pack_payload;
	int packetId = 0;
	send_data_info datainfo;
	datainfo.m_dataLen = len;
	datainfo.m_userData = userData;
	datainfo.m_sendTime = GMGetTickCount();
	strncpy(datainfo.m_peerUserId,peerAccountId,sizeof(datainfo.m_peerUserId) - 1);

	head.m_msgId = CSendDataGUID::GetSendMsgUID();
	head.m_msgLen = len;
	head.m_packNum = (len + pack_payload - 1) / pack_payload;
	datainfo.m_msgId = head.m_msgId;
	for (; packetId < pack_count; packetId++)
	{
		head.m_packId = packetId;
		send_packet_info packet;
		packet.m_packetId = packetId;
		packet.m_sendStatus = SEND_STATUS_WAIT_RESULT;
		packet.m_sendTimes = 1;
		packet.m_dataPtr = m_memcache.GetPacket();
		int headLen = head.Serilize(packet.m_dataPtr,SEND_PACKET_SIZE);
		memcpy(packet.m_dataPtr + headLen,buf + packetId * pack_payload, pack_payload);
		packet.m_dataLen = SEND_PACKET_SIZE;
		packet.m_lastSendTime = GMGetTickCount();
		U64 packUid = CSendDataGUID::GetSendDataUID();
		MHClient_SendTo(peerAccountId,packet.m_dataPtr,packet.m_dataLen,packUid);
		datainfo.m_sendPacketsUids[packetId] = packUid;
		m_sendPacketsLock.lock();
		m_sendPackets[packUid] = packet;
		m_sendPacketsLock.unlock();
	}
	if (len % pack_payload != 0)
	{
		head.m_packId = packetId;
		send_packet_info packet;
		packet.m_packetId = packetId;
		packet.m_sendStatus = SEND_STATUS_WAIT_RESULT;
		packet.m_sendTimes = 1;
		packet.m_dataPtr = m_memcache.GetPacket();
		packet.m_dataLen = len % pack_payload;
		int headLen = head.Serilize(packet.m_dataPtr,SEND_PACKET_SIZE);
		memcpy(packet.m_dataPtr + headLen,buf + packetId * pack_payload, packet.m_dataLen);
		packet.m_dataLen += headLen;
		packet.m_lastSendTime = GMGetTickCount();
		U64 packUid = CSendDataGUID::GetSendDataUID();
		MHClient_SendTo(peerAccountId,packet.m_dataPtr,packet.m_dataLen,packUid);
		datainfo.m_sendPacketsUids[packetId] = packUid;
		m_sendPacketsLock.lock();
		m_sendPackets[packUid] = packet;
		m_sendPacketsLock.unlock();
	}
	m_sendDataListLock.lock();
	m_sendDataList.push_back(datainfo);
	m_sendDataListLock.unlock();

	return 0;
}

int CP2PConn::Stop()
{
	m_bStopped = true;
	m_stopEvent.signal();
	m_sendDataProc.waitThreadExit(1000);
	return 0;
}

void CP2PConn::SendDataProc(void* param)
{
	while (!m_bStopped)
	{
		CheckSendData();

		CheckRecvData();

		m_stopEvent.wait(200);
	}
}

void CP2PConn::FreeSendData(int msgId)
{
	std::map<unsigned short,U64> packets;
	m_sendDataListLock.lock();
	std::list<send_data_info>::iterator iter = m_sendDataList.begin();
	for (; iter != m_sendDataList.end(); iter++)
	{
		if (iter->m_msgId == msgId)
		{
			packets = iter->m_sendPacketsUids;
			m_sendDataList.erase(iter);
			break;
		}
	}
	m_sendDataListLock.unlock();

	std::map<unsigned short,U64>::iterator itPacket = packets.begin();
	for (; itPacket != packets.end(); itPacket++)
	{
		m_sendPacketsLock.lock();
		if (m_sendPackets.find(itPacket->second) != m_sendPackets.end())
		{
			m_memcache.FreePacket(m_sendPackets[itPacket->second].m_dataPtr);
			m_sendPackets.erase(itPacket->second);
		}
		m_sendPacketsLock.unlock();
	}
}

void CP2PConn::CheckSendData()
{
	m_sendDataListLock.lock();
	std::list<send_data_info> sendlist = m_sendDataList;
	m_sendDataListLock.unlock();

	std::list<send_data_info>::iterator iter = sendlist.begin();
	for (; iter != sendlist.end(); iter++)
	{
		bool bSendFail = false;
		bool bRecvAllAck = true;
		std::map<unsigned short,U64>::iterator itPacket = iter->m_sendPacketsUids.begin();
		for (; itPacket != iter->m_sendPacketsUids.end(); itPacket++)
		{
			bool bNeedResendPacket = false;
			U64 packUid = itPacket->second;
			m_sendPacketsLock.lock();
			if (m_sendPackets.find(packUid) != m_sendPackets.end())
			{
				switch (m_sendPackets[packUid].m_sendStatus)
				{
				case SEND_STATUS_RECV_ACK:
					{
						//收到确认消息，说明对方已经收到，不用处理
					}
					break;
				case SEND_STATUS_NOT_SEND:
					{
						//不可能走到这里，插入队列的时候是SEND_STATUS_WAIT_RESULT
						bRecvAllAck = false;
					}
					break;
				case SEND_STATUS_WAIT_RESULT:
					{
						//还没收到应答，没超时，也没发送失败
						bRecvAllAck = false;
					}
					break;
				case SEND_STATUS_SEND_FAIL:
					{
						//发送失败，需要重发
						bRecvAllAck = false;
						bSendFail = true;
						bNeedResendPacket = true;
					}
					break;
				case SEND_STATUS_SEND_TIMEOUT:
					{
						//发送超时，需要重发
						bRecvAllAck = false;
						bNeedResendPacket = true;
					}
					break;
				default:
					{
						//不可能走到这里，fetal error
						bRecvAllAck = false;
					}
					break;
				}
			}
			if (bNeedResendPacket)
			{
				MHClient_SendTo(iter->m_peerUserId,m_sendPackets[packUid].m_dataPtr,m_sendPackets[packUid].m_dataLen,packUid);
				m_sendPackets[packUid].m_sendTimes++;
				m_sendPackets[packUid].m_lastSendTime = GMGetTickCount();
			}
			m_sendPacketsLock.unlock();
		}
		if (bRecvAllAck)
		{
			//回调发送成功
			if (m_pCallBack)
			{
				m_pCallBack->OnSendDataResult(iter->m_peerUserId,0,iter->m_userData);
			}
			//移除发送对象
			FreeSendData(iter->m_msgId);
		}
		else
		{
			//数据包是否发送超时，超时回调发送失败 并 移除发送对象
			U64 tnow = GMGetTickCount();
			if (tnow - iter->m_sendTime > DATA_SEND_MAX_WAIT_TIME)
			{
				//回调发送失败
				if (m_pCallBack)
				{
					m_pCallBack->OnSendDataResult(iter->m_peerUserId,(bSendFail ? -1 : -2),iter->m_userData);
				}
				//移除发送对象
				FreeSendData(iter->m_msgId);
			}
		}
	}
}

void CP2PConn::CheckRecvData()
{
	m_recvMsgLock.lock();
	std::map<std::string,recv_data_info> recvMsgs = m_recvMsgs;
	m_recvMsgLock.unlock();

	U64 tnow = GMGetTickCount();
	std::map<std::string,recv_data_info>::iterator iter = recvMsgs.begin();
	for (; iter != recvMsgs.end(); iter++)
	{
		if (tnow - iter->second.m_recvTime > 60000)
		{
			m_recvMsgLock.lock();
			if (m_recvMsgs.find(iter->first) != m_recvMsgs.end())
			{
				m_recvMsgs.erase(iter->first);
			}
			m_recvMsgLock.unlock();
		}
	}
}

void CP2PConn::OnRecvTransData(const char* fromUserId,const char* buf, const unsigned short len)
{
	p2p_conn_head head;
	if (len < head.GetRealSize())
	{
		return;
	}
	int unSerLen = head.UnSerilize((char*)buf,len);
	if (unSerLen <= 0)
	{
		return;
	}
	if (head.m_packNum == 1)
	{
		//没有分包直接回调
		if (m_pCallBack)
		{
			m_pCallBack->OnRecvData(fromUserId,buf + unSerLen,len - unSerLen);
		}
		return;
	}
	if (head.m_packNum > 1)
	{
		int payloadLen = SEND_PACKET_SIZE - head.GetRealSize();
		char lszKey[128] = { 0 };
		sprintf(lszKey,"%s_%u",fromUserId,head.m_msgId);
		if (m_recvMsgs.find(lszKey) == m_recvMsgs.end())
		{
			recv_data_info msginfo;
			strncpy(msginfo.m_peerUserId,fromUserId,sizeof(msginfo.m_peerUserId) - 1);
			msginfo.m_dataLen = head.m_msgLen;
			msginfo.m_msgId = head.m_msgId;
			msginfo.m_packNum = head.m_packNum;
			msginfo.m_recvPackList.insert(head.m_packId);
			msginfo.m_recvTime = GMGetTickCount();
			msginfo.m_msgPtr = new char[msginfo.m_dataLen];
			memcpy(msginfo.m_msgPtr + head.m_packId * payloadLen,buf + unSerLen,len - unSerLen);
			m_recvMsgs[lszKey] = msginfo;
		}
		else
		{
			m_recvMsgs[lszKey].m_recvPackList.insert(head.m_packId);
			memcpy(m_recvMsgs[lszKey].m_msgPtr + head.m_packId * payloadLen,buf + unSerLen,len - unSerLen);
			if (m_recvMsgs[lszKey].m_recvPackList.size() == m_recvMsgs[lszKey].m_packNum)
			{
				if (m_pCallBack)
				{
					m_pCallBack->OnRecvData(fromUserId,m_recvMsgs[lszKey].m_msgPtr,m_recvMsgs[lszKey].m_dataLen);
					delete []m_recvMsgs[lszKey].m_msgPtr;
					m_recvMsgs.erase(lszKey);
				}
			}
		}
	}
}

void CP2PConn::OnRecvTransResult(const unsigned long long userData,int result)
{
	m_sendPacketsLock.lock();
	if (m_sendPackets.find(userData) != m_sendPackets.end())
	{
		if (0 == result)
		{
			m_sendPackets[userData].m_sendStatus = SEND_STATUS_RECV_ACK;
		}
		else if (1 == result)
		{
			m_sendPackets[userData].m_sendStatus = SEND_STATUS_SEND_TIMEOUT;
		}
		else if (-1 == result)
		{
			m_sendPackets[userData].m_sendStatus = SEND_STATUS_SEND_FAIL;
		}
	}
	m_sendPacketsLock.unlock();
}

/************************CMemCacheMgr**********************/
CMemCacheMgr::CMemCacheMgr()
{
	m_packetSize = 0;
}

CMemCacheMgr::~CMemCacheMgr()
{
}

bool CMemCacheMgr::Init(unsigned short packetSize,unsigned int count)
{
	m_packetSize = packetSize;
	if (count > 0)
	{
		unsigned int bufsize = count * m_packetSize;
		char *pbuf = new(nothrow) char[bufsize];
		if (pbuf)
		{
			m_freePacketsLock.lock();
			for (int pos = 0; pos < count; pos++)
			{
				m_freePackets.insert(pbuf + pos * m_packetSize);
			}
			m_freePacketsLock.unlock();

			m_memBlocksLock.lock();
			m_memBlocks.insert(pbuf);
			m_memBlocksLock.unlock();
		}
	}
	return true;
}

char* CMemCacheMgr::GetPacket()
{
	m_freePacketsLock.lock();
	if (m_freePackets.empty())
	{
		unsigned int count = 16;
		unsigned int bufsize = count * m_packetSize;
		char *pbuf = new(nothrow) char[bufsize];
		if (pbuf)
		{
			for (int pos = 0; pos < count; pos++)
			{
				m_freePackets.insert(pbuf + pos * m_packetSize);
			}
			m_memBlocksLock.lock();
			m_memBlocks.insert(pbuf);
			m_memBlocksLock.unlock();
		}
	}
	m_freePacketsLock.unlock();

	char *packet = NULL;
	m_freePacketsLock.lock();
	if (!m_freePackets.empty())
	{
		packet = *m_freePackets.begin();
		m_freePackets.erase(packet);
	}
	m_freePacketsLock.unlock();
	if (packet)
	{
		m_usePacketsLock.lock();
		m_usePackets.insert(packet);
		m_usePacketsLock.unlock();
	}
	return packet;
}

void CMemCacheMgr::FreePacket(char* ptr)
{
	if (!ptr)
	{
		return ;
	}

	m_freePacketsLock.lock();
	m_freePackets.insert(ptr);
	m_freePacketsLock.unlock();

	m_usePacketsLock.lock();
	m_usePackets.erase(ptr);
	m_usePacketsLock.unlock();
}

void CMemCacheMgr::Destroy()
{
	m_freePacketsLock.lock();
	m_freePackets.clear();
	m_freePacketsLock.unlock();

	m_usePacketsLock.lock();
	m_usePackets.clear();
	m_usePacketsLock.unlock();

	m_memBlocksLock.lock();
	std::set<char*>::iterator iter = m_memBlocks.begin();
	for (; iter != m_memBlocks.end(); iter++)
	{
		char *ptr = *iter;
		if (ptr)
		{
			delete []ptr;
		}
	}
	m_memBlocks.clear();
	m_memBlocksLock.unlock();
}

/************************CSendDataGUID************************/
U64 CSendDataGUID::m_dataUID = 0;
GMLock CSendDataGUID::m_dataUIDLock;
unsigned int CSendDataGUID::m_msgId = 0;

U64 CSendDataGUID::GetSendDataUID()
{
	m_dataUIDLock.lock();
	U64 dataUid = m_dataUID;
	m_dataUID++;
	m_dataUIDLock.unlock();

	return dataUid;
}

unsigned int CSendDataGUID::GetSendMsgUID()
{
	m_dataUIDLock.lock();
	unsigned int msgUid = m_msgId;
	m_msgId++;
	m_dataUIDLock.unlock();

	return msgUid;
}
