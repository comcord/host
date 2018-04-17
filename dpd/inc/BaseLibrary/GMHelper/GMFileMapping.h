#pragma once
#include "GMLock.h"
#include "GMautolock.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

class GMFileMapping
{
public:
	GMFileMapping(void);
	~GMFileMapping(void);

	/*
	0 �����ɹ�
	1 ���Ѿ����ڵĳ�
	< 0 ʧ��
	*/
#ifdef WIN32
	int create(LPCTSTR name,unsigned int size);
#else
	//int create(const char* pathname,unsigned char proj_id,unsigned int size);
	int create(key_t key,unsigned int size);
#endif
	bool map();
	bool unmap();
	void close();

	bool isReady();
public:
	void* m_baseAddr;

private:
#ifdef WIN32
	HANDLE m_shm;
#else
	int m_shm;
#endif
	unsigned int m_size;//
	bool m_bReady;
	GMLock m_lock;
};
