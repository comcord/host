#ifndef GMPUBLICFUNS1_2005_KYG_H
#define GMPUBLICFUNS1_2005_KYG_H

#include "GMSocketBase.h"
#include <list>
#include "BaseDataType.h"
#include "NetTypes.h"

//��������
#define In
#define Out
#define InOut

extern    unsigned int GMGetSysTimePointer();

void    GMMyTRACE(const char * f,...);

/*
*	ָ���Ķ˿��Ƿ����
*/
bool    GMIsTcpPortFree(T_PORT port);
bool    GMIsUdpPortFree(T_PORT port);

/*
*	��ÿ������ӵ�SERVERIP��IP
*/
bool  GMGetLocalHostActiveIp( char* activeIp , char* serverIp = "1.1.1.1");
//bool  GMGetLocalHostActiveIp6( char* activeIp , char* serverIp );

/*
 *ת��IP��ʽ
 */
   char* GMConvertIP_n2a(unsigned int srcIP,T_IP destIP);

   unsigned int  GMConvertIP_a2n(T_IP srcIP);

   char* GMConvertIPv6_n2a(const V6IP &srcIP, char *destIP);

   V6IP GMConvertIPv6_a2n(char *srcIP);

/*
 * �����
 */
   int GMRandNum();

//���ر�����[low,high]֮���һ������ע���п��ܰ����߽硣
   unsigned int GMRandNumBetween(unsigned int low,unsigned int high);


   char* formatLastError(unsigned int errcode, char* msgBuf, size_t bufSize);

   //���ÿ���̨���ڴ�С
   bool set_console_window_size(U32 width,U32 height);

   //�󶨵�ǰ���̵�cpu�ڵ�(��0��ʼ)
   bool bind_current_process_on_numa_node(U32 node_num);

   /* return: -1 ,ʧ�� ��1��ipv4 ��2��ipv6 
   */
   int  GMCheckIpFamilyType(const char* readable_address);
   bool GMIPv4AddrToIPv6( const char * rmtIPV4 ,char * outIpv6Addr ,int * outAddrLen );


#endif
