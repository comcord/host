#pragma once

class COMMON_DLL_EXPORT cz
{
public:
	explicit cz(const char * fmt, ...);
	~cz();
	operator const char * () const
	{
		return m_buf;
	}
private:
	char m_buf[1024];
};
