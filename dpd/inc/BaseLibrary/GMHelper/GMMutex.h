#pragma once

#ifndef WIN32
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
#endif
#include "BaseDataType.h"
#include "GMLock.h"
#include "SysDiffDefine.h"
class GMMutex
{
public:
	GMMutex(void);

	~GMMutex(void);

	/*
	0 创建成功
	1 打开已经存在的
	< 0 失败
	第一次创建补拥有互斥量
	*/
#ifdef WIN32
	int create(LPCTSTR name);
#else
	//int create(const char* pathname,unsigned char chNameNum, bool bInitialOwner);
	int create(key_t key);
#endif

	/*
	0 成功
	-1 超时
	-2 其他错误
	*/
	int lock(U32 timeout = INFINITE);

	void unlock();

	bool is_ready();

private:

#ifndef WIN32
	bool init_sem();

	bool is_sem_init();
#endif

private:
	bool m_is_ready;    //
	GMLock m_lock; 

#ifndef WIN32
	int m_semid;	// 信号量集合的标识符

	//记录线程调用lock,unlock的次数，用来实现线程重入
	pthread_t m_owner_thread;
	int m_owner_lock_counter;
	
#else
	HANDLE m_mutex_handle;
#endif

};
