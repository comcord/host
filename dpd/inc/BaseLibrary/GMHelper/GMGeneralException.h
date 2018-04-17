#ifndef GMGENERALexception_H_2006_CHANNELSOFT
#define GMGENERALexception_H_2006_CHANNELSOFT

#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#pragma warning( disable : 4275 ) 
#endif

using namespace std;

/*
 *	通用异常类
 */
class GMGeneralException : public exception
{
public:
	
	GMGeneralException(const char *pMsg = "",int error = 0)
	{
		if(NULL != pMsg && pMsg[0] != '\0')
		{
			strcpy(m_pexceptionMsg,pMsg);
		}
		m_iError = error;
	}
	virtual ~GMGeneralException() throw()
	{
	}
	
	const char *getExceptionMsg() const
	{
		return m_pexceptionMsg;
	}
	
	int getExcetptionError() const
	{
		return m_iError;
	}
	
	virtual void action()
	{
		;
	}


	virtual const char *what() const throw()
	{
		return m_pexceptionMsg;	
	}
	
protected:
	char m_pexceptionMsg[1024];
	int  m_iError;
	
};


/*
 *	超时异常类
 */
class    GMTimeOutException : public GMGeneralException
{
public:
	GMTimeOutException(const char *pMsg = "Timeout exception",int error = 1)
		:GMGeneralException(pMsg,error)
	{
		;
	}
	virtual ~GMTimeOutException() throw()
	{
		;
	}
	virtual void action()
	{
		;
	}
};

#endif

