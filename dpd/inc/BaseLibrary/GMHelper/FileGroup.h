#ifndef FILE_GROUP
#define FILE_GROUP
#include <string>
#include <list>
#include "BaseDataType.h"
#include "GMLock.h"
#include "GMautolock.h"

/*
文件群类
功能：每次将用户内容追加到最新文件，用户不必操心到底写到哪个文件，文件写满了怎么办，创建的文件个数过多等问题

可设置文件群的最大文件个数和每个文件的最大尺寸
当文件个数超过上限时，自动删除最老的文件
当最新的文件的尺寸超过上限时，自动创建新文件

其它说明:
文件创建的文件命名为 file_prefix_*.file_postfix,文件存放在dir指定的目录下，*为自动生成的文件号

Author:yecw
Date:2009-6-11
*/

class FileGroup
{
public:
	FileGroup();

	//NULL == dir 在当前目录下生成文件
	void open(U32 max_file_num, U32 max_file_size/*Byte*/, const char* file_prefix,const char* file_postfix,const char* dir);

	void close();

	//写入内容
	bool write(void* buf, U32 bufSize);

	//强行将文件内容写入磁盘
	void flush();

private:
	//创建一个新文件，并对其映射
	bool createfile();

	//以最新的存在的文件作为后备空间
	bool get_free_space_from_existing_file(U32 size);

	//返回空间地址
	bool prepare_free_space(U32 size);

	//获取磁盘文件列表
	void get_file_list();

	//对文件列表排序,最前面的是最新的,注意处理回绕问题
	void sort_filelist();

	//获取当前最新文件编号
	U32 get_latest_filenum();

	//如果当前文件数目超过上限，则进行删除
	void del_unwanted_files();

	U32 parse_filenum(const std::string& filename);

	std::string compse_filename(U32 filenum);

private:
	GMLock m_lockall;

	bool m_is_open;                 
	U32 m_max_file_num;           //最大文件个数
	U32 m_max_file_size;          //每个文件最大大小
	std::string m_file_prefix;				//文件的统一前缀
	std::string m_file_postfix;
#ifdef WIN32
	HANDLE m_file_handle;
#else 
	int	m_file_handle;
#endif
	U32 m_free_space_size;        //当前空闲区间大小
	std::list<std::string> m_filename_list;//文件名列表 ，最新的文件放在最后面
	std::string m_dir;            //文件目录 包含最后的"\"
};

#endif 
