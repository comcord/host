#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef WIN32
#define _vsnprintf vsnprintf
#endif

class GMSZ
{
public:
	explicit GMSZ(const char * fmt, ...){
		va_list args;
		va_start(args, fmt);
		_vsnprintf(m_buf, sizeof(m_buf) - 1, fmt, args);
		m_buf[sizeof(m_buf) - 1] = '\0';
		va_end(args);
	}

	operator const char * () const{
		return m_buf;
	}
private:
	char m_buf[1024];
};


typedef GMSZ cz; 

