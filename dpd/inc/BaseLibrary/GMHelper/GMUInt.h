/****************************************************************************
*                                                                           *
* GMUint128.h --	constant and macros definitions	                    *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/
#ifndef _GF_GMUint128_DEFINE_20080422
#define _GF_GMUint128_DEFINE_20080422

#include <string>
#include <cmath>
#include "BaseDataType.h"


class GMUint128
{
public:
	GMUint128(void);
	~GMUint128(void);

	GMUint128(const U32 uValue);
	//explicit GMUint128(const uint8 *pbyValueBE);

	GMUint128& SetValue(const GMUint128 &uValue);
	GMUint128& SetValue(unsigned int uValue);
	GMUint128& SetValueBE(const U8 *pbyValueBE);

	const U8* GetData() const;

	int CompareTo(const GMUint128 &uOther) const;
	int CompareTo(unsigned int uValue) const;

	GMUint128& Xor(const GMUint128 &uValue);
	//GMUint128& XorBE(const uint8 *pbyValueBE);

	GMUint128& GMUint128Pow2(U32 num);

	GMUint128& Add(const GMUint128 &uValue);
	GMUint128& Subtract(const GMUint128 &uValue);
	GMUint128& operator+  (const GMUint128 &uValue);
	GMUint128& operator-  (const GMUint128 &uValue);
	U32   GMUint128Log();
	void operator=  (const GMUint128 &uValue);

	bool operator<  (const GMUint128 &uValue) const;

	bool operator>  (const GMUint128 &uValue) const;

	bool operator<= (const GMUint128 &uValue) const;
	bool operator>= (const GMUint128 &uValue) const;
	bool operator== (const GMUint128 &uValue) const;
	bool operator!= (const GMUint128 &uValue) const;

	bool operator== (unsigned int uValue) const;
	
	std::string ToString() const;
private:
	unsigned int		m_uData[4];
};
#endif
