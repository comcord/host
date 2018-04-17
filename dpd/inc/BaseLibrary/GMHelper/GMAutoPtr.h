#ifndef GMAutoPtr_H_2006_CHANNELSOFT
#define GMAutoPtr_H_2006_CHANNELSOFT

/*
*	采用代理方式实现智能指针类，
*	
*/

template<class T_PTR>
class GMAutoPtr 
{
	public:
		/*
		 *	构造函数
		 */
		GMAutoPtr( T_PTR ptr = NULL)
			:m_ptr(NULL)
		{
			m_ptr = ptr;
		}
		/*
		 *	DESTRUCTORS
		 */
		~GMAutoPtr()
		{
			if(m_ptr)
			{
				delete m_ptr;
				m_ptr = NULL;
			}
		}

		/*
		 *	*操作
		 */
		T_PTR operator*()
		{
			return *m_ptr;
		}
		/*
		 *	 ->操作
		 */
		T_PTR operator->()
		{
			return m_ptr;
		}

		/*
		 *	返回m_ptr
		 */
		T_PTR & getPtr()
		{
			return m_ptr;
		}
		
		/*
		 *	设置托管的指针
		 */
		void setPtr(T_PTR ptr)
		{
			m_ptr = ptr;
		}
		
	private:

		/*
		 *	禁止拷贝，预防生成多个对象，释放同一个内存，出错
		 */
		GMAutoPtr( GMAutoPtr & srcPtr)
		{
			;
		}
		/*
		 *	禁止 = ，，预防生成多个对象，释放同一个内存，出错
		 */
		GMAutoPtr & operator=( GMAutoPtr rightPtr)
		{
			;
		}


		T_PTR m_ptr;	//代理的指针
		
};

#endif
