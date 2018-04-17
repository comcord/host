#pragma once

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;

typedef  char I8;
typedef  short I16;
typedef  int I32;
typedef  long long I64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef  char int8;
typedef  short int16;
typedef  int int32;
typedef  long long int64;

#ifdef X64
typedef U64 GMPTR;
#else
typedef U32 GMPTR;
#endif

class GMBUF
{
public:
	GMBUF(char* buf_, U32 len_):buf(buf_),len(len_){}
	GMBUF():buf(NULL),len(0){}
public:
	U32 len;
	char* buf;
};
