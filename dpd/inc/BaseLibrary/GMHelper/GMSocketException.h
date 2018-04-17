#ifndef GMSOCKETexception_H_2006_CHANNELSOFT
#define GMSOCKETexception_H_2006_CHANNELSOFT

#include "GMGeneralException.h"

/*
 *	SOCKET“Ï≥£¿‡
 */
class GMSocketException : public GMGeneralException
{
public:
	GMSocketException(const char *pMsg = "Socket exception", int error = 1):GMGeneralException(pMsg, error)
	{
		
	}
	virtual ~GMSocketException() throw()
	{
		
	}
	virtual void action()
	{
		
	}
};

#endif

