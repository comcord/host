#ifndef MEMORY_ALLOC_WATCHER_H
#define MEMORY_ALLOC_WATCHER_H

//#define MONITER_MEM_ALLOC

#include <map>
#include <string>
#include <time.h>
#include <typeinfo>
#include "FileGroup.h"
#include "GMThread.h"
#include "GMFilesystemutility.h"
#include "XSingleton.h"
#include "GMMutex.h"

class MemAllocWatcher:public XSingleton<MemAllocWatcher>
{
	friend class XSingleton<MemAllocWatcher>;

	class Counter{
	public:
		Counter():m_new_counter(0),m_del_counter(0),m_old_new_counter(0),m_old_del_counter(0),m_size(0){}

		int m_size;
		int m_new_counter;
		int m_del_counter;
		int m_old_new_counter;
		int m_old_del_counter;
	};

public:
#pragma warning(disable:4355)

	~MemAllocWatcher();

	void recored_new(const char* name,int mem_size);

	void recored_del(const char* name);

	void show_proc(void*);

private:
	MemAllocWatcher();

private:
	//FileLoger m_file_loger;
	std::string m_moudle_info;
	FileGroup m_fg;
	GMMutex m_mutex;        //保护多进程写文件组
	GMLock m_lock;
	std::map<std::string,Counter> m_ndmap;
	GMDaemonThread<MemAllocWatcher> m_show_thread;
};
#endif
