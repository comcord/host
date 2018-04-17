/****************************************************************************
*                                                                           *
* GMRWLockPoolAgent.h --								                        *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/

#ifndef GF_RWGMLockPoolAgent_H_GOUMANG_2008
#define GF_RWGMLockPoolAgent_H_GOUMANG_2008
#include "GMLockPool.h"

/*
*	����Լ���� ���ȵ���lock��Ȼ�����unlock, LOCK��UNLock�Ķ�����Ҫ�ɶ�
*/
class GMRWLockPoolAgent
{
public:
	GMRWLockPoolAgent(GMRWLockPool_SmartPtr);
	~GMRWLockPoolAgent();

	 void readLock();  
	 void writeLock();
	 void unReadLock();
	 void unWriteLock();

private:
	void get_lock_poc();

	GMRWLockPool_SmartPtr m_pLockPool;
	GMRWLock * m_pLock;
	unsigned int m_lockCallNum;
};

#endif
