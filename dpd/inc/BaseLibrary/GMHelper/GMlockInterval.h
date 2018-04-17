#pragma once
#ifndef WIN32
#include <sys/time.h>
class GMlockInterval
{
public:
	GMlockInterval();
	unsigned int GetTickInterval();
private:
	timeval m_startimeval;
};
#endif
