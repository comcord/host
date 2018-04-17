#ifndef COMMON_PUB_H_2012
#define COMMON_PUB_H_2012

#include "gmhelperLibPub.h"

#define Address GMAddress
#define CRWLock GMRWLock
#define CLock GMLock //同一线程不能lock 2次
#define CAutoLock GMAutoLock<GMLock>
#define GeneralException GMGeneralException
#define CTimeOutException GMTimeOutException
#define CSleepWithWakeup GMSleepWithWakeup 
#define CSocketException GMSocketException 
#define CSysEvent GMSysEvent
#define CTcpClient GMTcpClient
#define CTcpServer GMTcpServer
#define CUdpSocket GMUdpSocket
#define DllHelper GMDllHelper
#define FixedAllocator GMFixedAllocator 
#define SmallObjAllocator GMSmallObjAllocator
#define HttpClient GMHttpClient
#include "classes_for_common.h"
#endif
