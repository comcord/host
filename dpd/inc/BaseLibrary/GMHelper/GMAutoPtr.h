#ifndef GMAutoPtr_H_2006_CHANNELSOFT
#define GMAutoPtr_H_2006_CHANNELSOFT

/*
*	���ô���ʽʵ������ָ���࣬
*	
*/

template<class T_PTR>
class GMAutoPtr 
{
	public:
		/*
		 *	���캯��
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
		 *	*����
		 */
		T_PTR operator*()
		{
			return *m_ptr;
		}
		/*
		 *	 ->����
		 */
		T_PTR operator->()
		{
			return m_ptr;
		}

		/*
		 *	����m_ptr
		 */
		T_PTR & getPtr()
		{
			return m_ptr;
		}
		
		/*
		 *	�����йܵ�ָ��
		 */
		void setPtr(T_PTR ptr)
		{
			m_ptr = ptr;
		}
		
	private:

		/*
		 *	��ֹ������Ԥ�����ɶ�������ͷ�ͬһ���ڴ棬����
		 */
		GMAutoPtr( GMAutoPtr & srcPtr)
		{
			;
		}
		/*
		 *	��ֹ = ����Ԥ�����ɶ�������ͷ�ͬһ���ڴ棬����
		 */
		GMAutoPtr & operator=( GMAutoPtr rightPtr)
		{
			;
		}


		T_PTR m_ptr;	//�����ָ��
		
};

#endif
