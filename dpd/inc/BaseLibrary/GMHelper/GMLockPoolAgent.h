/****************************************************************************
*                                                                           *
* GMRWLockPoolAgent.h --								                        *
*                                                                           *
* Copyright (c) GOUMANG Corporation. All rights reserved.                   *
*                                                                           *
****************************************************************************/

#ifndef GF_GMLockPoolAgent_H_GOUMANG_2008__
#define GF_GMLockPoolAgent_H_GOUMANG_2008__

#include "GMLockPool.h"


/*
 *	调用约定： 首先调用lock，然后调用unlock, LOCK与UNLock的动作需要成队
 */
class GMLockPoolAgent
{
public:
	GMLockPoolAgent(GMLockPool_SmartPtr);
	~GMLockPoolAgent();

	 void lock();
	 void unlock();

private:
	GMLockPool_SmartPtr m_pLockPool;
	GMLock * m_pLock;
	volatile int m_lockCallNum;
};

#endif
