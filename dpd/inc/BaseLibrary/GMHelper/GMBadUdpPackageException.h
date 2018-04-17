#pragma once
#include "GMGeneralException.h"

class GMBadUdpPackageexception :public GMGeneralException
{
public:
	GMBadUdpPackageexception(const char *pMsg = "GMBadUdpPackageexception",int error = 1)
		:GMGeneralException(pMsg,error)
	{
	}
};
