// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#ifdef WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

#endif

#include <stdio.h>

#ifdef WIN32
#include <tchar.h>
#endif

#include "gmhelperLibPub.h"
#include "AsynModelPublish.h"
#include "LogPub.h"

#include "json.h"
#pragma comment(lib,"json.lib")

#ifndef MEETING_HOST_AGENT_LOG_SUBTYPE 
#define MEETING_HOST_AGENT_LOG_SUBTYPE  ((I64)1 << 2)

#define  CELL_PHONE_VERSION

#endif
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
