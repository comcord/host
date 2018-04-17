#pragma once
#include "GMToken.h"

class  GMAutoToken
{
public:
	GMAutoToken(GMToken * const pToken)
		:m_pToken(pToken)
	{
		if(m_pToken != NULL)
			m_pToken->Borrow();
	}
	~GMAutoToken(void)
	{
		if(m_pToken != NULL)
			m_pToken->Repay();
	}

private:
	void* operator new(size_t t) throw()
	{
		return NULL;
	}
private:
	GMToken * m_pToken;
};
