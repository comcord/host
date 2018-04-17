// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#ifdef WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
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
// TODO: 在此处引用程序需要的其他头文件
