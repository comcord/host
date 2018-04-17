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
*	调用约定： 首先调用lock，然后调用unlock, LOCK与UNLock的动作需要成队
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
