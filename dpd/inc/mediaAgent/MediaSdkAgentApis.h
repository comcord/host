#pragma once
#ifndef MEDIA_AGENT_APIS_
#define MEDIA_AGENT_APIS_

typedef enum _MICROPHONE_NUM
{
	MICROPHONENO = 0,
	MICROPHONE1 = 1,
	MICROPHONE2
}MICROPHONE_NUM;

typedef enum _MEDIA_DATA_TYPE
{
	VIDEO_DATA,
	AUDIO_DATA
}MEDIA_DATA_TYPE;

typedef enum _CALL_BACK_TYPE
{
	JOIN_MEETING,
	EXIT_MEETING,
	SPEAK,
}CALL_BACK_TYPE;


struct MediaSdkAgentInfo
{
	char			m_recv_send_data_ip[16];	//��MediaPlay�������ݽ����ĵ�ַ
	unsigned short	m_recv_send_data_port;		
	char			m_local_session_ip[16];		//��ý��������������ݽ����ĵ�ַ
	unsigned short	m_local_session_port;
	char			m_rc_addr[1024];					//·�ɵ�ַ

	char			m_log_config_file_dir[260];	//��־�����ļ�������ȫ·����
	char			m_log_files_path[260];					//��־�ļ�����Ŀ¼

};

#ifdef __cplusplus
#	ifdef WIN32
#		ifdef MEDIASDKAGENTDLL_EXPORTS
#			define MEDIA_SDK_API extern "C" __declspec(dllexport)
#		else
#			define MEDIA_SDK_API extern "C" __declspec(dllimport)
#		endif
#	else
#		define MEDIA_SDK_API extern "C"
#	endif
#else
#define MEDIA_SDK_API
#endif

/**********�½ӿ�***************/
typedef enum _msagent_CallBackEvent_enum
{
	msagent_CallBackEvent_rateChange,             //���ʵ���
	msagent_CallBackEvent_getAudioInfoStatistic,  //�����Ƶ������Ϣ
	msagent_CallBackEvent_getcpuInfo              //���cpu��Ϣ
}msagent_CallBackEvent_enum;

//msagent_CallBackEvent_rateChange ��Ӧ�����ݽṹ
typedef enum _msagent_CallBackEvent_rateChangeMode
{
	msagent_CallBackEvent_rateChangeMode_cpu,       //cpu�ߵ���
	msagent_CallBackEvent_rateChangeMode_daption    //����Ӧ����
}msagent_CallBackEvent_rateChangeMode;
typedef struct _msagent_CallBackEvent_rateChange_st
{
	unsigned int							m_vedioRate;         //ϣ����������Ƶ����: bps
	msagent_CallBackEvent_rateChangeMode    m_changeMode;        //����ģʽ
}msagent_CallBackEvent_rateChange_st;

//��Ƶ��֡ͳ��,���е�ʱ�䵥λ����ms ��msagent_CallBackEvent_getAudioInfoStatistic
typedef struct
{
	MICROPHONE_NUM micid;                  
	//�ɼ����
	int curEncTime;             ///<��ǰ����ʱ��
	int maxEncTime;             ///<��ʷ������ʱ��
	int minEcnTime;             ///<��ʷ��С����ʱ��
	int curAecTime;             ///<��ǰ��������ʱ��
	int maxAecTime;             ///<��ʷ����������ʱ��
	int minAecTime;             ///<��ʷ��С��������ʱ��
	int captureQueueCache;      ///<�ɼ����л���ʱ��

	//�������
	int emptyPacketRate;        ///<����������(rate/10000)
	int onePacket;              ///<����һ�������İ�����
	int twoPackets;             ///<���������������ĸ���
	int threePackets;           ///<���������������ĸ���
	int fourToTenPackets;       ///<����4��10���������ĸ���
	int moreThanTenPackets;     ///<��������ʮ���������ĸ���
	int curDecTime;             ///<��ǰ����ʱ��
	int maxDecTime;             ///<��ʷ������ʱ��
	int minDecTime;             ///<��ʷ��С����ʱ��
	int audioTrackBufCache;     ///<���Ż���������ʱ��	
	int receiveQueueCache;      ///<���ն��л���ʱ��

	unsigned int frameRate;              //֡��
	unsigned int vedioRate;              //��Ƶ����(bps)
	unsigned short vedioWidth;           //��Ƶ�ֱ���
	unsigned short vedioHeight;
}AudioInfoStatistic;

//msagent_CallBackEvent_getcpuInfo
typedef struct
{
	double  m_rate;               //cpuʹ����
	int     m_memRate;            //�ڴ�ʹ����
}ms_agent_cpuInfo;

//ý��agent֪ͨ�ϲ���¼�
typedef void (*msagent_CallBackEvent)(msagent_CallBackEvent_enum cbEvent,void *pData, void* pUserData);

typedef struct msagent_MediaSdkAgentInfo
{
	char			*m_local_session_ip;		//��ý��������������ݽ����ĵ�ַ
	unsigned short	m_local_session_port;
	char			*m_rc_addr;					//·�ɵ�ַ
	char            *m_mic1RecvMediaIp;
	int             m_mic1RecvMediaPort;
	char            *m_mic2RecvMediaIp;
	int             m_mic2RecvMediaPort;
	char            *m_sendMediaIp;
	int             m_sendMediaPort;
	char            *m_useId;
	char            *token;

	char			*m_log_config_file_dir;	//��־�����ļ�������ȫ·����
	char			*m_log_files_path;					//��־�ļ�����Ŀ¼

	msagent_CallBackEvent pcbEvent;        //ʱ��ص�����
	void            *puserData;             //�����¼��ص�ʱ���û�����

}msagent_MediaSdkAgentInfo;

typedef struct msagent_qosInfo_mic
{
	int worktype;      //����ģʽ��0-���У�1- �ɼ�(����)
	int upNetspeed;    //��������
	int downNetspeed;  //��������
	int upLossRate;    //���ж�����
	int upLossFecRate; //����fec�󶪰���
	int downLossRate;  //���ж�����
	int downFecRate;   //����fec�󶪰���
}msagent_qosInfo_mic;
typedef struct msagent_qosInfo
{
	msagent_qosInfo_mic mic1;
	msagent_qosInfo_mic mic2;

	char *pstrQOS;      //json�ַ���������������ʾ
	int   QOSlen;
}msagent_qosInfo;

//���͵�ý���ʽ
typedef struct _msagent_mediaFormat
{
	int  m_vedioRate;       //��Ƶ����
	int  m_vedioFrameRate;  //��Ƶ֡��
	int  m_audioRate;       //��Ƶ����
}msagent_mediaFormat;

typedef void (*msagent_ErrorReportCallBack)(CALL_BACK_TYPE call_back_type, int error_code, void* pUserData);
MEDIA_SDK_API int msagent_MediaSdkAgentInit(msagent_MediaSdkAgentInfo* pInfo); 

//�μӻ���
MEDIA_SDK_API int msagent_JoinMeeting(const char* meeting_id, msagent_ErrorReportCallBack cb, void* pUserData);
//�˳�����
MEDIA_SDK_API int msagent_ExitMeeting(const char* meeting_id, void* pUserData);
//����
MEDIA_SDK_API int msagent_Speak(const char* meeting_id, MICROPHONE_NUM microphone_num, void* pUserData);
//ֹͣ����, û���첽�ص�,ͬ�����ؽ��;
MEDIA_SDK_API int msagent_StopSpeak(const char* meeting_id, MICROPHONE_NUM microphone_num);

/*���mic����������*/
MEDIA_SDK_API int msagent_GetQos(msagent_qosInfo *pQosInfo);

typedef void (*msagent_recvMediaCallBack)(MICROPHONE_NUM microphone_num, MEDIA_DATA_TYPE media_data_type,char *pframe,int len);
MEDIA_SDK_API int msagent_SetRecvMediaCB(msagent_recvMediaCallBack cbParam);
MEDIA_SDK_API void msagent_sendFrame( MEDIA_DATA_TYPE media_data_type,char *pframe,int len);

#ifdef __IOS__
MEDIA_SDK_API int msagent_MediaSdkAgentRelease();
#endif

/*��ÿ��ý���ʽ�����仯ʱ���á�����ÿ�ο�ʼ����ʱҲ�õ���*/
MEDIA_SDK_API  void  msagent_setMediaFormat(msagent_mediaFormat *pmediaFormat);
/***********************************����Ľӿڷ���************************************/
typedef void (*ErrorReportCallBack)(CALL_BACK_TYPE call_back_type, int error_code, void* pUserData);

//�μӻ���
MEDIA_SDK_API int JoinMeeting(const char* meeting_id, ErrorReportCallBack cb, void* pUserData);
//�˳�����
MEDIA_SDK_API int ExitMeeting(const char* meeting_id, void* pUserData);
//����
MEDIA_SDK_API int Speak(const char* meeting_id, MICROPHONE_NUM microphone_num, void* pUserData);
//ֹͣ����, û���첽�ص�,ͬ�����ؽ��;
MEDIA_SDK_API int StopSpeak(const char* meeting_id, MICROPHONE_NUM microphone_num);

#endif
