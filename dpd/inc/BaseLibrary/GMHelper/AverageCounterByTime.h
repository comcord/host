#ifndef AverageCounterByTime_h__
#define AverageCounterByTime_h__

#include <string>
class AverageCounterByTime
{
	class ArrayValue
	{
	public:
		U32 times;
		U32 values;
	};
public:

	AverageCounterByTime(std::string name="") 
	{
		new (this)AverageCounterByTime(1000,5000,name);
	}

	AverageCounterByTime(U32 interval,U32 time,std::string name="")
	{
		m_name = name;
		m_rate = 0;
		m_times = 0;
		m_value = 0;
		m_interval = interval;
		m_totaltime = time;
		m_i = 0;

		m_arrryNum = m_totaltime/m_interval ;//数组可用范围
		m_ArrayValue = new ArrayValue[m_arrryNum];
		memset(m_ArrayValue,0,sizeof(ArrayValue)*m_arrryNum);
	}

	~AverageCounterByTime()
	{
		delete m_ArrayValue;
	}

	void start()
	{
		m_calthread.init(&AverageCounterByTime::calValue,this,NULL);
		m_calthread.start();
	}

	void stop()
	{
		m_calthread.stop();
		m_calthread.waitThreadExit();
	}

	void add(U32 vl)
	{
#ifdef WIN32
		if (vl)
		{
			InterlockedExchangeAdd(&m_value,static_cast<long>(vl));
		}
		InterlockedIncrement (&m_times);

#else
#ifdef _ANDROID_NDK_
		if (vl)
		{
			InterlockedExchangeAdd(&m_value,static_cast<int>(vl));
		}
		InterlockedIncrement (&m_times);
#else	 //Linux
			if (vl)
			{
				InterlockedExchangeAdd(&m_value,vl);
			}
			InterlockedIncrement (&m_times);
			
#endif //_ANDROID_NDK_

#endif //WIN32
		
	}

	double getValue()
	{
		return m_rate;
	}

private:
	void calValue(void* )
	{	
		Sleep(m_interval);
#ifdef WIN32
		long t = InterlockedExchangeAdd(&m_times,0);
		long v = InterlockedExchangeAdd(&m_value,0);
#else
#ifdef _ANDROID_NDK_
		int t = InterlockedExchangeAdd(&m_times,0);
		int v = InterlockedExchangeAdd(&m_value,0);
#else	//Linux
		U32 t = InterlockedExchangeAdd(&m_times,0);
		U32 v = InterlockedExchangeAdd(&m_value,0);
#endif //_ANDROID_NDK_
#endif //WIN32		

		if ((m_times < t) || (m_value < v))
		{
			memset(m_ArrayValue,0,sizeof(ArrayValue)*m_arrryNum);
			t = 0;
			v = 0;
			Sleep(1000);
		}

		m_i %= m_arrryNum;

		U32 times  = t - m_ArrayValue[m_i].times;
		U32 values = v - m_ArrayValue[m_i].values;

		m_ArrayValue[m_i].times = t;
		m_ArrayValue[m_i].values = v;

		m_i++;

		if(times!=0)
			m_rate = (double)values  / (double)times;
		else
			m_rate = 0;

		if(m_name != "")
			printf("%s %f\r\n",m_name.c_str(),m_rate);
	}
public: 

	double m_rate;
#ifdef WIN32
	long volatile m_times;
	long volatile m_value;
#else 
#ifdef _ANDROID_NDK_
	volatile int m_times;
	volatile int m_value;
#else	//Linux
	U32 volatile m_times;
	U32 volatile m_value;

#endif //_ANDROID_NDK_
#endif //WIN32


	U32 m_interval;
	U32 m_totaltime; 
	U32 m_i;
	U32 m_arrryNum;
	GMDaemonThread<AverageCounterByTime,Class_Mem_Thread_Fun> m_calthread;
	ArrayValue* m_ArrayValue;
	std::string m_name;
};

#endif // AverageCounterByTime_h__
