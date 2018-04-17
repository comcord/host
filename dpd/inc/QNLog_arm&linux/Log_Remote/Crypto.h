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
	*	����ֵ:
			0: ���ܳɹ�
			-1:����ʧ��
			-2:����Ĳ�������
			-3:���ܵ��ļ�������
	*/
	static int encryptoConfig( const char *pFilePath,const char *pPassWord );

	/*
	*	����ֵ:
			0: ���ܳɹ�
			-1:����ʧ��
			-2:����Ĳ�������
			-3:���ܵ��ļ�������
	*/
	static int decryptoConfig( const char *pFilePath,const char *pPassWord );
	
	/*
	*	����ֵ:
			>0: ���ܳɹ�
			<0:����ʧ��
	*/
	static int encryptoMemBlock( unsigned char *pMemoryIn,int memSize, char *pPassword );
	
	/*
	*	����ֵ:
			>0: ���ܳɹ�
			<0:����ʧ��
	*/
	static int decryptoMemBlock( unsigned char *pMemoryIn,int memSize, char *pPassword );
private:
	static int CAPIEncryptFile(PCHAR szSource, PCHAR szDestination, PCHAR szPassword);
	static int CAPIDecryptFile(PCHAR szSource, PCHAR szDestination, PCHAR szPassword);

};
