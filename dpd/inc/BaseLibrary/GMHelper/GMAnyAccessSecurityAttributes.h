#pragma once

#ifdef WIN32
//��������Ȩ�޵İ�ȫ��������(Ҳ�������ܱ���)
class GMAnyAccessSecurityAttributes 
{
public:
	GMAnyAccessSecurityAttributes();
	~GMAnyAccessSecurityAttributes();


	SECURITY_ATTRIBUTES* getSA();

protected:
	PACL   m_pDACL;

	SECURITY_ATTRIBUTES m_sa;
	SECURITY_DESCRIPTOR m_sd;
};
#endif
