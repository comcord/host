#ifndef GMPUBLICFUNS1_2005_KYG_H
#define GMPUBLICFUNS1_2005_KYG_H

#include "GMSocketBase.h"
#include <list>
#include "BaseDataType.h"
#include "NetTypes.h"

//参数方向
#define In
#define Out
#define InOut

extern    unsigned int GMGetSysTimePointer();

void    GMMyTRACE(const char * f,...);

/*
*	指定的端口是否空闲
*/
bool    GMIsTcpPortFree(T_PORT port);
bool    GMIsUdpPortFree(T_PORT port);

/*
*	获得可以连接到SERVERIP的IP
*/
bool  GMGetLocalHostActiveIp( char* activeIp , char* serverIp = "1.1.1.1");
//bool  GMGetLocalHostActiveIp6( char* activeIp , char* serverIp );

/*
 *转换IP格式
 */
   char* GMConvertIP_n2a(unsigned int srcIP,T_IP destIP);

   unsigned int  GMConvertIP_a2n(T_IP srcIP);

   char* GMConvertIPv6_n2a(const V6IP &srcIP, char *destIP);

   V6IP GMConvertIPv6_a2n(char *srcIP);

/*
 * 随机数
 */
   int GMRandNum();

//返回闭区间[low,high]之间的一个数。注意有可能包括边界。
   unsigned int GMRandNumBetween(unsigned int low,unsigned int high);


   char* formatLastError(unsigned int errcode, char* msgBuf, size_t bufSize);

   //设置控制台窗口大小
   bool set_console_window_size(U32 width,U32 height);

   //绑定当前进程到cpu节点(从0开始)
   bool bind_current_process_on_numa_node(U32 node_num);

   /* return: -1 ,失败 ，1：ipv4 ，2：ipv6 
   */
   int  GMCheckIpFamilyType(const char* readable_address);
   bool GMIPv4AddrToIPv6( const char * rmtIPV4 ,char * outIpv6Addr ,int * outAddrLen );


#endif
