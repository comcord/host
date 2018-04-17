#include "stdafx.h"
//#include "basicDefine.h"
#include "configParser.h"
#include "GMFilesystemutility.h"
#include <string>
#include "GMMarkupSTL.h"


bool CConfigParser_STPIPMAP::GetMappedStpIp(serverInfo & info,serverInfo & outInfo)
{
	GMAutoLock<GMRWLock> al(&stpIPMapRWLock,GMReadOperator);
	std::map<serverInfo,serverInfo>::iterator iter = stpIPMap.find(info);
	if(iter == stpIPMap.end())
	{
		return false;
	}

	outInfo = iter->second;

	return true;
}


bool CConfigParser_STPIPMAP::SetMappedStpIp(serverInfo & info,serverInfo & outInfo)
{
	GMAutoLock<GMRWLock> al(&stpIPMapRWLock,GMWriteOperator);

	stpIPMap[info] = outInfo;

	return true;
}