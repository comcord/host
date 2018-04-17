#pragma once

template<typename T>
class FQueue
{
public:
	FQueue(void):m_front(0),m_back(0),m_maxsize(0),m_data(NULL){};

	bool init(unsigned int maxsize)
	{
		m_front = 0;
		m_back = 0;
		m_maxsize = maxsize;
		m_data = new T[m_maxsize+1];
		return NULL != m_data;
	}

	void uninit()
	{
		if(m_data)
		{
			delete[] m_data;
			m_data = NULL;
		}
	}

	unsigned int size(){return (m_front-m_back+m_maxsize+1)%(m_maxsize+1);}

	unsigned int capacity(){return m_maxsize;}

	bool push_back(const T& val)
	{
		if (!m_data)return false;
		
		int newBack = (m_back-1+m_maxsize+1)%(m_maxsize+1);
		if (newBack == m_front)return false;//the queue is full
		
		m_data[m_back] = val;
		m_back = newBack;
		return true;
	}

	bool push_front(const T& val)
	{
		if (!m_data)return false;

		int newFront =(m_front+1)%(m_maxsize+1);
		if (newFront == m_back)return false;//the queue is full
		
		m_data[newFront] = val;
		m_front = newFront;
		return true;
	}

	bool pop_back(T& val)
	{
		if (!m_data)return false;

		if (m_back == m_front)return false;//the queue is empty
		
		int new_back = (m_back+1)%(m_maxsize+1);
		val = m_data[new_back];
		m_back = new_back;
		return true;
	}

	bool pop_front(T& val)
	{
		if (!m_data)return false;

		if (m_back == m_front)return false;//the queue is empty

		val = m_data[m_front];
		m_front = (m_front-1+m_maxsize+1)%(m_maxsize+1);
		return true;
	}

	void clear()
	{
		m_back = 0;
		m_front = 0;
	}
private:
	T* m_data;//一个位置标记空块
	int m_front;
	int m_back;//back指向位置为空
	unsigned int m_maxsize;
};

