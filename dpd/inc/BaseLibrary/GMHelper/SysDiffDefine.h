#ifndef SysDiffDefine_h__
#define SysDiffDefine_h__

#ifdef WIN32

#define PATHSPLIT_CHAR '\\'
#define PATHSPLIT_STRING "\\"
#define GMGetTickCount()  GetTickCount()

#else // linux

#include <stddef.h>
#include <errno.h>

#ifdef __IOS__
#include <libkern/OSAtomic.h>
#endif

#ifndef Sleep
	#define Sleep(x) usleep((x)*1000) //sleep
#endif
#ifndef _snprintf
	#define _snprintf snprintf
#endif
#ifndef PATHSPLIT_CHAR
	#define PATHSPLIT_CHAR '/'
#endif //PATHSPLIT_CHAR
#ifndef PATHSPLIT_STRING
	#define PATHSPLIT_STRING "/"
#endif //PATHSPLIT_STRING
#ifndef _MAX_PATH
	#define _MAX_PATH 512
#endif //_MAX_PATH
#ifndef MAX_PATH
	#define MAX_PATH 512
#endif //MAX_PATH
#ifndef INFINITE
	#define INFINITE 0xFFFFFFFF
#endif //INFINITE
#ifndef _ANDROID_NDK_
#ifndef UCHAR_MAX
	#define UCHAR_MAX     0xff
#endif //UCHAR_MAX
#endif //_ANDROID_NDK_
#ifndef _memicmp
	#define _memicmp strncasecmp
#endif
#ifndef _stricmp
#define _stricmp stricmp
#endif //_memicmp
int GetLastError();
long GetTickCount64();

#include "GMTimerAssistant.h"
extern GMlockInterval g_clockInterval;
#define GMGetTickCount()  g_clockInterval.GetTickInterval()
#define GetCurrentProcessId() getpid();

#ifdef _ANDROID_NDK_
#include <sys/atomics.h>
__ATOMIC_INLINE__  int InterlockedCompareExchange(volatile int *dest, int exch, int comp)
{
	//return __atomic_cmpxchg(comp,_new ,ptr);
	return __sync_val_compare_and_swap(dest, comp, exch) != comp;
}
__ATOMIC_INLINE__ int InterlockedIncrement(volatile int *val)
{
  return __sync_fetch_and_add (val, 1);
}
__ATOMIC_INLINE__ int InterlockedDecrement(volatile int *val)
{
	 return __sync_fetch_and_sub (val, 1);	
}
__ATOMIC_INLINE__ int InterlockedExchange(volatile int *Target, int Value)
{
    int prev;
    do {
        prev = *Target;
    } while (__sync_val_compare_and_swap(Target, prev, Value ) != prev);
    return prev;
}
__ATOMIC_INLINE__ int InterlockedExchangeAdd(volatile int *Addend, int Increment)
{
  return __sync_fetch_and_add (Addend, Increment);	
}

#else
static __inline__ U32 InterlockedIncrement(U32 volatile *val)
{
	U32 tmp;
#ifdef __IOS__
	return OSAtomicIncrement32((int32_t*)val);

#else
	__asm__ __volatile__ ("lock; xaddl %0, %1"
		: "=r" (tmp), "=m" (*val)
		: "0" (1), "m" (*val));

	return(tmp+1);
#endif
}

static __inline__ U32 InterlockedDecrement(U32 volatile *val)
{
	U32 tmp;
#ifdef __IOS__
	return OSAtomicDecrement32((int32_t*)val);
#else

	__asm__ __volatile__ ("lock; xaddl %0, %1"
		: "=r" (tmp), "=m" (*val)
		: "0" ((U32)-1), "m" (*val));

	return(tmp-1);
#endif
}

static __inline__ U32 InterlockedExchangeAdd(U32 volatile *Addend, U32 Increment)
{
	U32 ret;
#ifdef __IOS__
  /*return value after add*/
  ret = OSAtomicAdd32((int32_t)Increment,(int32_t*)Addend);
	return (ret - Increment);
#else

	__asm__ __volatile__(
		/* lock for SMP systems */
		"lock\n\t"
		"xaddl %0,(%1)"
		:"=r" (ret)
		:"r" (Addend), "0" (Increment)
		:"memory");
	return ret;
#endif
}

#ifdef __IOS__
static __inline__ bool InterlockedCompareExchange(U32 volatile *dest, U32 exch, U32 comp)
{
	return OSAtomicCompareAndSwap32(comp,exch,(int32_t*)dest);
}
#else
static __inline__ U32 InterlockedCompareExchange(U32 volatile *dest, U32 exch, U32 comp)
{
	U32 old;
	__asm__ __volatile__ ("lock; cmpxchgl %2, %0"
                        : "=m" (*dest), "=a" (old)
                        : "r" (exch), "m" (*dest), "a" (comp));
	return old;
}
#endif

static __inline__ U32 InterlockedExchange(U32 volatile *Target, U32 Value)
{
#ifdef __IOS__
  return 0;
	int prev;
    do {
        prev = *Target;
    } while (true != OSAtomicCompareAndSwap32(prev, Value, (int32_t*)Target ));
    return prev;

#else
	U32 ReturnValue;

	__asm__ __volatile(
		"lock xchgl %2,(%1)"
		: "=r" (ReturnValue)
		: "r" (Target), "0" (Value)
		: "memory");

	return ReturnValue;
#endif
}
#endif //_ANDROID_NDK_
static __inline__ long long InterlockedIncrement64(long long volatile *val)
{
	
#ifdef __IOS__
	return OSAtomicIncrement64((int64_t*)val);
#else
  long long tmp;
	__asm__ __volatile__ ("lock; xadd %0, %1"
		: "=r" (tmp), "=m" (*val)
		: "0" (1), "m" (*val));

	return(tmp+1);
#endif
}

static __inline__ long long InterlockedDecrement64(long long volatile *val)
{
	
#ifdef __IOS__
	return OSAtomicDecrement64((int64_t*)val);

#else
 long long tmp;
	__asm__ __volatile__ ("lock; xadd %0, %1"
		: "=r" (tmp), "=m" (*val)
		: "0" ((long long)-1), "m" (*val));

	return(tmp-1);
#endif
}

static __inline__ long long InterlockedExchangeAdd64(long long volatile *Addend, long long Increment)
{
	
#ifdef __IOS__
  int64_t ios_ret;
  ios_ret = OSAtomicAdd64(Increment,(int64_t*)Addend);
	return (ios_ret - Increment);
#else
  long long ret;
	__asm__ __volatile__(
		/* lock for SMP systems */
		"lock\n\t"
		"xadd %0,(%1)"
		:"=r" (ret)
		:"r" (Addend), "0" (Increment)
		:"memory");
	return ret;
#endif
}

#ifdef __IOS__
static __inline__ bool InterlockedCompareExchange64(long long volatile *dest, long long exch, long long comp)
{
    return OSAtomicCompareAndSwap64(comp,exch,(int64_t*)dest);
}
  
#else
static __inline__ long long InterlockedCompareExchange64(long long volatile *dest, long long exch, long long comp)
{
	long long old;
	__asm__ __volatile__ ("lock; cmpxchg %2, %0"
		: "=m" (*dest), "=a" (old)
		: "r" (exch), "m" (*dest), "a" (comp));	
	return(old);
}
#endif

#endif //against

#define gmmin(a,b) (((a) < (b)) ? (a) : (b))
#define gmmax(a,b) (((a) > (b)) ? (a) : (b))

#endif // SysDiffDefine_h__

