#pragma once

#include <cassert>

#pragma warning(disable:4355)

/*
�̶���С�Ļ��ζ��У���������ʱ������Push��ȥ�����ݽ�������
���̰߳�ȫ

������ʾ������Ϊ2�Ļ��ζ��е��ڲ��仯����
1.��ʼʱ��û��һ��Ԫ��

	 begin
	|NULL|NULL|NULL|
	   0    1    2  
     end
2.ִ��һ��push(1)
	 begin
	|1   |NULL|NULL|
	  0    1    2    
	      end
3.ִ�еڶ���push(2)
	begin
	|1   |2  |NULL|
	  0    1   2   
		       end
4.ִ�е�����push(3)����һ����������
	      begin
	|NULL|2   |3   |
	  0    1    2   
	 end
5.ִ�е��Ĵ�push(4)���ڶ�����������
	            begin
	|4   |NULL |3   |
	  0    1     2   
	      end
6.ִ�е����push(5),�ֻص���һ�θ�����״̬
		
	begin
	|4   |5   |NULL |
	 0    1     2   
		  	   end
*/

#ifndef UT_PRIVATE
#define UT_PRIVATE private
#endif

template<class ValueType>
class GMCircleQueue
{
	typedef ValueType& reference; 
	typedef ValueType* pointer; 
public:
	class iterator
	{
		friend class GMCircleQueue<ValueType>;

	public:
		iterator():m_pQ(NULL){}
		iterator(GMCircleQueue* pQ):m_pQ(pQ),m_index(0){}

		iterator& operator++()
		{// preincrement
			m_index = (++m_index)%(m_pQ->m_capacity+1);
			return *this;
		}

		iterator operator++(int)
		{// postincrement
			iterator temp = *this;
			++*this;
			return temp;
		}

		iterator& operator--()
		{// predecrement
			if (0 == m_index)
				m_index = m_pQ->m_capacity;
			else
				--m_index;

			return *this;
		}

		iterator operator--(int)
		{// postdecrement
			iterator temp = *this;
			--*this;

			return temp;
		}

		reference operator*()const
		{
			return m_pQ->m_records[m_index];
		}

		pointer operator->()const
		{	// return pointer to class object
			return (&**this);
		}

		bool operator==(const iterator& val)const
		{
			return m_index == val.m_index && m_pQ == val.m_pQ;
		}		
		
		bool operator!=(const iterator& val)const
		{
			return !(*this == val);
		}

	UT_PRIVATE:
		unsigned int m_index;
		GMCircleQueue* m_pQ;
	};

	class reverse_iterator
	{
	public:
		reverse_iterator(){}
		reverse_iterator(const iterator& it):m_iterator(it){}

		reference operator*() const
		{	// return designated value
			iterator _Tmp = m_iterator;
			return (*--_Tmp);
		}

		pointer operator->()const
		{	// return pointer to class object
			return (&**this);
		}

		reverse_iterator& operator++()
		{	// preincrement
			--m_iterator;
			return (*this);
		}

		reverse_iterator operator++(int)
		{	// postincrement
			reverse_iterator _Tmp = *this;
			--m_iterator;
			return (_Tmp);
		}

		//reverse_iterator& operator--()
		//{	// predecrement
		//	++m_iterator;
		//	return (*this);
		//}

		//reverse_iterator operator--(int)
		//{	// postdecrement
		//	reverse_iterator _Tmp = *this;
		//	++m_iterator;
		//	return (_Tmp);
		//}

		bool operator==(const reverse_iterator& val)const
		{
			return m_iterator == val.m_iterator;
		}

		bool operator!=(const reverse_iterator& val)const
		{
			return m_iterator != val.m_iterator;
		}

UT_PRIVATE:
		iterator m_iterator;
	};

public:
	GMCircleQueue(unsigned int capacity)
		:m_beginIterator(this),m_endInterator(this),m_capacity(capacity),m_pushTimes(0)
	{
		m_records = new ValueType[m_capacity+1];//1��λ���������end
	}

	~GMCircleQueue(void)
	{
		delete[] m_records;
	}

	unsigned int capacity()
	{
		return m_capacity;
	}
	
	void clear()
	{
		m_pushTimes = 0;
		m_beginIterator.m_index = 0;
		m_endInterator.m_index = 0;
	}
    
	//����ʵ�ʴ�С�����������������ʵ�ʴ�С���ǵ��ڶ�������
    unsigned int getRealSize()
	{
		if (m_pushTimes>=m_capacity)
			return m_capacity;
		else
			return m_endInterator.m_index-m_beginIterator.m_index;
	}

    //����push��
	iterator begin()
	{
		return m_beginIterator;
	}

	iterator end()
	{
		return m_endInterator;
	}

	//���push��
	reverse_iterator rbegin()
	{
		return reverse_iterator(m_endInterator);
	}

	reverse_iterator rend()
	{
		return reverse_iterator(m_beginIterator);
	}

	void push(const ValueType& val)
	{
		if (m_pushTimes<m_capacity)
			++m_pushTimes;

		m_records[m_endInterator.m_index] = val;

		if ((m_endInterator.m_index + 1)%(m_capacity+1) == m_beginIterator.m_index)
		{
			++m_endInterator;
			++m_beginIterator;
		}
		else 
		{
			++m_endInterator;
		}
	}

UT_PRIVATE:
	unsigned int m_pushTimes;
	unsigned int m_capacity;
	iterator m_beginIterator;
	iterator m_endInterator;
	ValueType* m_records;
};

#pragma warning(default:4355)

