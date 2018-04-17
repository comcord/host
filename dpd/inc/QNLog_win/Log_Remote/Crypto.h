#pragma once

#ifndef CRYPTOCONFIG_EXPORTS
#define CRYPTO_EXPORT __declspec(dllimport)
#pragma comment(lib,"Crypto.lib")
#else
#define CRYPTO_EXPORT __declspec(dllexport)
#endif

extern bool g_bIsCrypt;

class CRYPTO_EXPORT CCrypto
{
private:
	CCrypto();
	~CCrypto();
public:
	/*
	*	返回值:
			0: 加密成功
			-1:加密失败
			-2:传入的参数不对
			-3:加密的文件不存在
	*/
	static int encryptoConfig( const char *pFilePath,const char *pPassWord );

	/*
	*	返回值:
			0: 解密成功
			-1:解密失败
			-2:传入的参数不对
			-3:解密的文件不存在
	*/
	static int decryptoConfig( const char *pFilePath,const char *pPassWord );
	
	/*
	*	返回值:
			>0: 加密成功
			<0:加密失败
	*/
	static int encryptoMemBlock( unsigned char *pMemoryIn,int memSize, char *pPassword );
	
	/*
	*	返回值:
			>0: 解密成功
			<0:解密失败
	*/
	static int decryptoMemBlock( unsigned char *pMemoryIn,int memSize, char *pPassword );
private:
	static int CAPIEncryptFile(PCHAR szSource, PCHAR szDestination, PCHAR szPassword);
	static int CAPIDecryptFile(PCHAR szSource, PCHAR szDestination, PCHAR szPassword);

};
