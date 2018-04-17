#pragma once

#define  __HOST_LOCAL_GETSHARE  11

struct __host_local_getshare_req
{
	char userid[100];
	char ip[16];
	int  port;
};