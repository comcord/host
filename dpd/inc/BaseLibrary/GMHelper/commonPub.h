#ifndef COMMON_PUB_H_2012
#define COMMON_PUB_H_2012

#include "gmhelperLibPub.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
	if (x != NULL)       \
   {                    \
   x->Release();     \
   x = NULL;         \
   }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
	if (x != NULL)      \
   {                   \
   delete x;        \
   x = NULL;        \
   }
#endif

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(x) \
	if (x != NULL)            \
   {                         \
   delete[] x;            \
   x = NULL;              \
   }
#endif

typedef unsigned char uint8x;
typedef unsigned short uint16x;
typedef unsigned int uint32x;
typedef unsigned long long uint64x;

typedef  char int8x;
typedef  short int16x;
typedef  int int32x;
typedef  long long int64x;

#define Address GMAddress
//#define CRWLock GMRWLock
#define CRWLock GMJRRWLock  //这个性能不好 建议使用GMRWLock 但是接口不一致
#define CLock GMLock //同一线程不能lock 2次
#define CAutoLock GMAutoLock<GMLock>
#define CGeneralException GMGeneralException
#define CTimeOutException GMTimeOutException
#define CSleepWithWakeup GMSleepWithWakeup 
#define CSocketException GMSocketException 
#define CSysEvent GMSysEvent
#define CTcpServer GMTcpServer
#define CUdpSocket GMUdpSocket
#define DllHelper GMDllHelper
#define FixedAllocator GMFixedAllocator 
#define SmallObjAllocator GMSmallObjAllocator
#define HttpClient GMHttpClient
#define CMarkupSTL GMMarkupSTL 
#include "classes_for_common.h"
#define getSysTimePointer GMGetTickCount
#define isTcpPortFree GMIsTcpPortFree
#define isUdpPortFree GMIsUdpPortFree
#define getFreeTcpPort GMIsTcpPortFree
#define getFreeUdpPort GMIsUdpPortFree
#define getLocalHostActiveIp GMGetLocalHostActiveIp
#define getLocalHostIP(ip) getLocalHostActiveIp( ip ,"1.1.1.1" )
#define convertIP_n2a GMConvertIP_n2a
#define convertIP_a2n GMConvertIP_a2n
#define randNum GMRandNum
#define randNumBetween GMRandNumBetween

#endif
