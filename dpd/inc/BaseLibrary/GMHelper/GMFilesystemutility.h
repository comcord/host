#pragma once
#include "GMUInt.h"
#include <string>
#include "SysDiffDefine.h"

class GMFileSystemUtility
{
public:
	static bool bFileExist(const char* file);

	static bool createDirectories(const char* dir);

	static void makeSureEndWithSlash(std::string& dir);

	//Ω·Œ≤√ª”– /
	static bool getModulePath(std::string& dir);

	static bool resizeFile(const std::string& filePathName, U32 newSize);
};
