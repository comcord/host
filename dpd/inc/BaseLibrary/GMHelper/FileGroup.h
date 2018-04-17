#ifndef FILE_GROUP
#define FILE_GROUP
#include <string>
#include <list>
#include "BaseDataType.h"
#include "GMLock.h"
#include "GMautolock.h"

/*
�ļ�Ⱥ��
���ܣ�ÿ�ν��û�����׷�ӵ������ļ����û����ز��ĵ���д���ĸ��ļ����ļ�д������ô�죬�������ļ��������������

�������ļ�Ⱥ������ļ�������ÿ���ļ������ߴ�
���ļ�������������ʱ���Զ�ɾ�����ϵ��ļ�
�����µ��ļ��ĳߴ糬������ʱ���Զ��������ļ�

����˵��:
�ļ��������ļ�����Ϊ file_prefix_*.file_postfix,�ļ������dirָ����Ŀ¼�£�*Ϊ�Զ����ɵ��ļ���

Author:yecw
Date:2009-6-11
*/

class FileGroup
{
public:
	FileGroup();

	//NULL == dir �ڵ�ǰĿ¼�������ļ�
	void open(U32 max_file_num, U32 max_file_size/*Byte*/, const char* file_prefix,const char* file_postfix,const char* dir);

	void close();

	//д������
	bool write(void* buf, U32 bufSize);

	//ǿ�н��ļ�����д�����
	void flush();

private:
	//����һ�����ļ���������ӳ��
	bool createfile();

	//�����µĴ��ڵ��ļ���Ϊ�󱸿ռ�
	bool get_free_space_from_existing_file(U32 size);

	//���ؿռ��ַ
	bool prepare_free_space(U32 size);

	//��ȡ�����ļ��б�
	void get_file_list();

	//���ļ��б�����,��ǰ��������µ�,ע�⴦���������
	void sort_filelist();

	//��ȡ��ǰ�����ļ����
	U32 get_latest_filenum();

	//�����ǰ�ļ���Ŀ�������ޣ������ɾ��
	void del_unwanted_files();

	U32 parse_filenum(const std::string& filename);

	std::string compse_filename(U32 filenum);

private:
	GMLock m_lockall;

	bool m_is_open;                 
	U32 m_max_file_num;           //����ļ�����
	U32 m_max_file_size;          //ÿ���ļ�����С
	std::string m_file_prefix;				//�ļ���ͳһǰ׺
	std::string m_file_postfix;
#ifdef WIN32
	HANDLE m_file_handle;
#else 
	int	m_file_handle;
#endif
	U32 m_free_space_size;        //��ǰ���������С
	std::list<std::string> m_filename_list;//�ļ����б� �����µ��ļ����������
	std::string m_dir;            //�ļ�Ŀ¼ ��������"\"
};

#endif 
