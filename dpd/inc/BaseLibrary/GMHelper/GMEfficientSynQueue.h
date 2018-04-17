#ifndef GM_EFFICIENT_SYN_QUEUE_H_TEMPLATE_2008_GM
#define GM_EFFICIENT_SYN_QUEUE_H_TEMPLATE_2008_GM
#include "GMMemAlloc.h"
template <class T
	,template<class> class GMMemAllocMothed = GMListMemAllocMethod>
class GMCompletePortList
{
	typedef struct _PER_IO_DATA : public GMMemAlloc<_PER_IO_DATA, GMMemAllocMothed>
	{
		OVERLAPPED ol;
		char buf[sizeof(T)];
		char nOperationType;

#define OP_DATA_ARRIVED 1
#define OP_CLOSE 2
	}PER_IO_DATA, *PPER_IO_DATA;

public:
	GMCompletePortList()
	{
		m_hComption = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,0);
	}
	~GMCompletePortList()
	{
		GetAllCompletePackageAndGiveUp();
		CloseHandle(m_hComption);
	}
	bool cancel()
	{
		PPER_IO_DATA pPerIO = new PER_IO_DATA();
		if(pPerIO == NULL)
			return false;

		pPerIO->nOperationType = OP_CLOSE;
		return PostQueuedCompletionStatus(m_hComption,0,(unsigned int)m_pPerHandle,&(pPerIO->ol)) ;	
	}

	//����ֵ:1   ȡ������
	//		 0   ȡ��
	//		 -1  ��ȡ����
	int front(T & tOut)
	{
		
		PPER_IO_DATA	 pPerIO1;
		DWORD			 dwTrans;
		BOOL bOK = ::GetQueuedCompletionStatus(m_hComption,&dwTrans,
			NULL,(LPOVERLAPPED*)&pPerIO1, WSA_INFINITE);

		if(!bOK)
		{
			if(pPerIO1 != NULL)
				delete pPerIO1;
			return -1;
		}

		switch(pPerIO1->nOperationType)
		{
			case OP_DATA_ARRIVED:
				if(pPerIO1 != NULL)
				{
					memcpy((T*)&tOut,(T*)&pPerIO1->buf,sizeof(T));
					delete pPerIO1;
				}
				return 1;

			case OP_CLOSE:		
				cancel();
				if(pPerIO1 != NULL)
					delete pPerIO1;
				return 0;

			default:
				if(pPerIO1 != NULL)
					delete pPerIO1;
				return -1;
		}	
	}
	void GetAllCompletePackageAndGiveUp()
	{
		PPER_IO_DATA	 pPerIO1;
		DWORD			 dwTrans;
		while(true)
		{
			BOOL bOK = ::GetQueuedCompletionStatus(m_hComption,&dwTrans,
				NULL,(LPOVERLAPPED*)&pPerIO1, 1000);

			if(!bOK)
			{
				if(pPerIO1 != NULL)
					delete pPerIO1;
				return;
			}

			switch(pPerIO1->nOperationType)
			{
			case OP_DATA_ARRIVED:

			case OP_CLOSE:		
				
				if(pPerIO1 != NULL)
					delete pPerIO1;

				continue;
			default:
				if(pPerIO1 != NULL)
					delete pPerIO1;
				return;
			}	
		}	
	}

	bool push_back(const T & t)
	{
		PPER_IO_DATA pPerIO = new PER_IO_DATA();
		if(pPerIO == NULL)
			return false;

		pPerIO->nOperationType = OP_DATA_ARRIVED;
		memcpy(pPerIO->buf,(void *)&t,sizeof(T));

		return ::PostQueuedCompletionStatus(m_hComption,0,NULL,&(pPerIO->ol)) ;	

	}
private:
	HANDLE				m_hComption;		
};
/*
*	ͬ�����У��̰߳�ȫ����д����������ŵ����е�β�����������Ӷ���ͷ����
*/
template<class T,template<class> class GMMemAllocMothed = GMListMemAllocMethod>//, class GMMemAllocMothed = GMListMemAllocMethod>
class GMEfficientSynQueue
{
public:
	GMEfficientSynQueue(void):hasBeenDestroy(false)
	{
	}
	~GMEfficientSynQueue(void)
	{
	}

	void destroy()
	{
		if( !hasBeenDestroy)
		{
			hasBeenDestroy = true;
			m_CompleteList.cancel();	
		}
	}
	//������д������
	bool pushBack(const T& obj)
	{
		if( !hasBeenDestroy)	
			return m_CompleteList.push_back(obj);
		else	
			return false;
	}

	//�Ӷ��ж������� Ĭ�ϴ�ͷ��ʼ�����ɹ�����ture,ʧ�ܷ���fals
	//����ֵ:1   ȡ������
	//		 0   ȡ��
	//		 -1  ��ȡ����
	int popFront(T & o)
	{	
		if( !hasBeenDestroy)
			return m_CompleteList.front(o);
		else
			return 0;
	}

protected:
	GMCompletePortList<T,GMMemAllocMothed> m_CompleteList;
	volatile bool	hasBeenDestroy;				//�Ƿ��Ѿ�ȡ����Ϣ����
};

#endif
