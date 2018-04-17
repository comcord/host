#ifndef XSINGLETON_H
#define XSINGLETON_H

/*���main����֮ǰ���ʵ�������ĳ���*/
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
	���ܶ���Ϊ��̬��Ա��������Ϊ����ȫ�ֱ�����ʼ��ʱ����ʱ�����ܻ�δ��ʼ��
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
