#ifndef GMHELPERPUB_H_2006_CHANNELSOFT
#define GMHELPERPUB_H_2006_CHANNELSOFT


//WINSOCK2 必须出现在 Windows 前

/*Defining _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES to be 1 enables template 
*overloads of standard CRT functions that call the more secure variants automatically.
*/
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
/*
*_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES does not affect the functions that take a count,
*such as strncpy. To enable template overloads for the count functions, 
*define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT to be 1. 
*Before doing so, however, make sure that your code passes the count of characters, 
*not the size of the buffer (a common mistake). 
*/
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1

void set_crt_invalid_parameter_handler();

#ifdef _DEBUG

#define PRIVATE public     
#define PROTECTED public

#ifdef UNIT_TEST
#define TEST_API __declspec(dllimport)
#else
#define TEST_API __declspec(dllexport)
#endif

#else

#define PRIVATE private     
#define PROTECTED protected

#define TEST_API 

#endif

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 

//#define  _WIN32_WINNT 0x0501
//#define  _WIN32_WINNT 0x0502
#ifndef _LIB
#pragma comment( lib, "gmhelperLib" )
#endif

#include <winsock2.h>
#include <Windows.h>
#endif

/*MemoryAllocWatcher.h必须出现在第一项因为此文件定义了是否打开内存分配监视的开关
*构成同一个动态库的所有静态库的开关状态必须相同。
*如果想要内存监视在MemoryAllocWatcher.h中 #define MONITER_MEM_ALLOC
*/
#ifndef _ANDROID_NDK_
#include "MemoryAllocWatcher.h"
#endif //_ANDROID_NDK_

#include "NetTypes.h"
#include "GMUInt.h"

/*
 *  String
 */
#include "GMSZ.h"

/*
 *	APP TOOLS
 */
#include "GMPubFuns.h"
#include "GMNetworkutility.h"
#include "GMFilesystemutility.h"
#include "GMHttpClient.h"
#include "GMDllHelper.h"
/*
 *	SOCKET
 */
#include "GMGeneralException.h"
#include "GMSocketException.h"
#include "GMSocketBase.h"
#include "GMTcpSocket.h"
#include "GMUdpSocket.h"
#include "GMRawUdpSocket.h"
#include "GMBadUdpPackageException.h"
#include "GMHttpClient.h"
/*
 *	SYSTEM ABOUT
 */
#include "GMEvent.h"
#include "GMThread.h"
#include "GMThreadEx.h"
#include "GMThreadFromPool.h"
#include "GMRecyclableThreadPool.h"
#include "GMLock.h"
#include "GMToken.h"
#include "GMAutoToken.h"
//#include "GMLockFreeFIFOQueue.h"

#include "GMRWLock.h"

#include "GMLockPool.h"
#include "GMLockPoolAgent.h"
#include "GMRWLockPoolAgent.h"
#include "GMSleepWithWakeup.h"
#include "GMTimerAssistant.h"

#include "GMCpuUsage.h"
#include "GMAnyAccessSecurityAttributes.h"
#ifndef _ANDROID_NDK_
#include "GMFileMapping.h"
#endif //_ANDROID_NDK_
/*
 *	XML PARSER
 */
#include "GMMarkupSTL.h"


/*
 *	platform
 */
#include "GMPlatformPack.h"


/*
 *	MemAlloc
 */
#include "GMMemAlloc.h"
#include "GMSmallObj.h"
#include "GMWidgetAlloctor.h"
/*
 *	CommonTemplate
 */
#include "GMautolock.h"
#include "GMAutoPtr.h"
#include "GMBlist.h"
#include "GMEmbedSmartPtr.h"
#include "GMSmartPtr.h"
#include "GMCustomTimer.h"
#include "InterlockedRWLock.h"
#include "GMSynQueue.h"
#include "GMindexlist.h"
#include "GMSingleTon.h"
#include "GMThread.h"
#include "GMCircleQueue.h"
#include "FQueue.h"
#include "BalanceQueue.h"
#include "XSingleton.h"
#ifndef WIN32
#include "SysDiffDefine.h"
#endif
#include "AverageCounterByTime.h"

/*experiment works, the interface is volatile*/
#ifndef _ANDROID_NDK_
#include "GMMutex.h"
#endif //_ANDROID_NDK_

//LOG
#include "FileGroup.h"

#endif //GMHELPERPUB_H_2006_CHANNELSOFT
