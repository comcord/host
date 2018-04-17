//
//  MeetingHostIos.m
//  testhost
//
//  Created by redcdn on 18/2/11.
//  Copyright © 2018年 redcdn. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <pthread.h>
#include <map>

#include "MeetingHostAgent.h"
#include "P2PConnApi.h"
#include <arpa/inet.h>
#include <netdb.h>

extern "C"
{
#include "MeetingHostIos.hpp"
}

#include "json.h"


class P2PConnCallback : public IP2PConnCallBack
{
public:
    virtual int OnRecvData(const char* accountId,const char* buf,const unsigned short len)
    {
        NSLog(@"recv data from %s, len=%d, data=%s\n",accountId,len,buf);
        return 0;
    }
    
    virtual int OnSendDataResult(const char* accountId,int rc,const unsigned long long userData)
    {
        NSLog(@"OnSendDataResult account=%s,rc=%d,userdata=%lld\n",accountId,rc,userData);
        return 0;
    }
};

int client_sock = -1;
const char* userId = "62000001";
unsigned short uiport = 10001;
unsigned short hostagentport = 10002;
const char* rcAddr = "10.160.71.12:20004|10.160.71.12:20004";
IP2PConn* pConn = NULL;
IP2PConnCallBack *pCb = NULL;
unsigned long long uid = 0;
const char* peerUserId = "69600006";

extern "C"
{

void* ui_Recv(void* param)
{
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == client_sock)
    {
        NSLog(@"create recv socket fail");
        return NULL;
    }
    
    struct sockaddr_in local_addr;
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(uiport);
    local_addr.sin_family = AF_INET;
    
    int bind_result = bind(client_sock,(struct sockaddr*)&local_addr,sizeof(local_addr));
    if (-1 == bind_result)
    {
        NSLog(@"recv socket bind fail");
        return NULL;
    }
    
    struct sockaddr_in peer_addr;
    socklen_t perr_addr_len = sizeof(peer_addr);
    
    char buf[2048] = { 0 };
    
    while (true)
    {
        int recv_len = recvfrom(client_sock, buf, sizeof(buf), 0,(struct sockaddr*)&peer_addr,(socklen_t*)&perr_addr_len);
        if (recv_len <= 0)
        {
            continue;
        }
        Json::Value root;
        Json::Reader reader;
        bool bsucc = reader.parse(buf, root, false);
        if (bsucc)
        {
            int cmdid = root["cmdid"].asInt();
            switch (cmdid)
            {
                case 3001:
                {
                    NSLog(@"recv register response");
                    break;
                }
                default:
                {
                    NSLog(@"recv cmd %d", cmdid);
                    break;
                }
            }
        }
    }
    
    return NULL;
}

void SendShare()
{
    Json::Value root;
    root["cmdid"] = 3000;
    root["accountid"] = userId;
    root["name"] = "iosuser";
    root["ssip"] = "10.160.71.120";
    root["ssport"] = 7001;
    
    std::string str;
    Json::FastWriter writer;
    str = writer.write(root);
    struct sockaddr_in peer_addr;
    peer_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    peer_addr.sin_port = htons(hostagentport);
    peer_addr.sin_family = AF_INET;
    int send_result = sendto(client_sock,str.c_str(),str.length() + 1,0,(struct sockaddr*)&peer_addr,sizeof(peer_addr));
    
}

int StartHost()
{
    int ret = MHClient_Start(userId, "127.0.0.1", 10000, "127.0.0.1", hostagentport, "127.0.0.1", uiport, rcAddr,
                             "", "");
    if (ret <= 0)
    {
        NSLog(@"MHClient_Start fail ,return = %d",ret);
        return -1;
    }
    
    hostagentport = ret;
    
    pthread_t thread;
    pthread_create(&thread, NULL, &ui_Recv, NULL);
    
    pCb = new P2PConnCallback();
    pConn = CreateP2PConn(userId);
    if (NULL == pConn)
    {
        NSLog(@"CreateP2PConn return NULL");
        return -2;
    }
    
    pConn->Start(pCb);
    
    return 0;
}

int SendData()
{
    if (pConn)
    {
        pConn->SendData(peerUserId, "1234567890", 11, uid++);
    }
    return 0;
}
    
}
