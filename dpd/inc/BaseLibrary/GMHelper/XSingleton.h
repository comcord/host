#ifndef XSINGLETON_H
#define XSINGLETON_H

/*解决main函数之前访问到单键类的场景*/
template <class T>
class XSingleton
{
public:
	static T * inst()
	{
		if(!get_inst())
		{
			GMAutoLock<GMLock> al(get_lock());
			if(!get_inst())
				get_inst() = new(std::nothrow) T; 
		}
		return get_inst();
	}

	static void destroy()
	{
		GMAutoLock<GMLock> al(get_lock() );
		if(get_inst())
		{
			delete get_inst(); 
			get_inst() = NULL;
		}	
	}

private:
	/*
	不能定义为静态成员变量，因为其他全局变量初始化时访问时，可能还未初始化
	*/
	static GMLock* get_lock(){
		static GMLock _lock;
		return &_lock;
	}

	static T*& get_inst(){
		static T* _inst = NULL;
		return _inst;
	}

	class InitLocalStaticVarible
	{
		InitLocalStaticVarible()
		{
			get_lock();
			get_inst();
		}
	};

	static InitLocalStaticVarible m_init_local_static_varible;
};

template <class T>
typename XSingleton<T>::InitLocalStaticVarible XSingleton<T>::m_init_local_static_varible;

#endif
