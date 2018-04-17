#pragma once

class  GMDllHelper
{
public:
	/*
	If the function succeeds, the return value is a handle to the module.
	If the function fails, the return value is NULL. 
	*/
	static void* loadLibrary(const char* pFileName);
	static bool freeLibrary(void* hModule);

	/*
	If the function succeeds, the return value is the address of the exported function or variable.
	If the function fails, the return value is NULL. 
	*/
	static void* getProcAddr(void* hModule,const char* pProcName);
};
