#pragma once
#include "AsynResultDefine.h"

namespace AsynModel
{
	class SubScribeEventCallBack
	{
	public:
		virtual void TCPDefaultListenPortFailureCallBack( char errorIP[16],unsigned short errorPort,AsynResultRet errorType) = 0;
		virtual void UDPDefaultPortFailureCallBack(char errorIP[16],unsigned short errorPort,AsynResultRet errorType) = 0;
	};
}
