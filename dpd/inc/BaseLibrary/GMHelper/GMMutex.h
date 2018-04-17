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
	0 �����ɹ�
	1 ���Ѿ����ڵ�
	< 0 ʧ��
	��һ�δ�����ӵ�л�����
	*/
#ifdef WIN32
	int create(LPCTSTR name);
#else
	//int create(const char* pathname,unsigned char chNameNum, bool bInitialOwner);
	int create(key_t key);
#endif

	/*
	0 �ɹ�
	-1 ��ʱ
	-2 ��������
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
	int m_semid;	// �ź������ϵı�ʶ��

	//��¼�̵߳���lock,unlock�Ĵ���������ʵ���߳�����
	pthread_t m_owner_thread;
	int m_owner_lock_counter;
	
#else
	HANDLE m_mutex_handle;
#endif

};
