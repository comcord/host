#pragma once

#ifdef WIN32
//生成所有权限的安全访问属性(也即对象不受保护)
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
