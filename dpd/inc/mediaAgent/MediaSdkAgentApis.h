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
	char			m_recv_send_data_ip[16];	//与MediaPlay进行数据交互的地址
	unsigned short	m_recv_send_data_port;		
	char			m_local_session_ip[16];		//与媒体服务器进行数据交互的地址
	unsigned short	m_local_session_port;
	char			m_rc_addr[1024];					//路由地址

	char			m_log_config_file_dir[260];	//日志配置文件名（带全路径）
	char			m_log_files_path[260];					//日志文件保存目录

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

/**********新接口***************/
typedef enum _msagent_CallBackEvent_enum
{
	msagent_CallBackEvent_rateChange,             //码率调整
	msagent_CallBackEvent_getAudioInfoStatistic,  //获得音频空音信息
	msagent_CallBackEvent_getcpuInfo              //获得cpu信息
}msagent_CallBackEvent_enum;

//msagent_CallBackEvent_rateChange 对应的数据结构
typedef enum _msagent_CallBackEvent_rateChangeMode
{
	msagent_CallBackEvent_rateChangeMode_cpu,       //cpu高调整
	msagent_CallBackEvent_rateChangeMode_daption    //自适应调整
}msagent_CallBackEvent_rateChangeMode;
typedef struct _msagent_CallBackEvent_rateChange_st
{
	unsigned int							m_vedioRate;         //希望调整的视频码率: bps
	msagent_CallBackEvent_rateChangeMode    m_changeMode;        //调整模式
}msagent_CallBackEvent_rateChange_st;

//音频空帧统计,所有的时间单位都是ms ，msagent_CallBackEvent_getAudioInfoStatistic
typedef struct
{
	MICROPHONE_NUM micid;                  
	//采集相关
	int curEncTime;             ///<当前编码时间
	int maxEncTime;             ///<历史最大编码时间
	int minEcnTime;             ///<历史最小编码时间
	int curAecTime;             ///<当前回声消除时间
	int maxAecTime;             ///<历史最大回声消除时间
	int minAecTime;             ///<历史最小回声消除时间
	int captureQueueCache;      ///<采集队列缓冲时间

	//播放相关
	int emptyPacketRate;        ///<空音包比率(rate/10000)
	int onePacket;              ///<连续一个空音的包个数
	int twoPackets;             ///<连续两个空音包的个数
	int threePackets;           ///<连续三个空音包的个数
	int fourToTenPackets;       ///<连续4到10个空音包的个数
	int moreThanTenPackets;     ///<连续超过十个空音包的个数
	int curDecTime;             ///<当前解码时间
	int maxDecTime;             ///<历史最大解码时间
	int minDecTime;             ///<历史最小解码时间
	int audioTrackBufCache;     ///<播放缓冲区缓冲时间	
	int receiveQueueCache;      ///<接收队列缓冲时间

	unsigned int frameRate;              //帧率
	unsigned int vedioRate;              //视频码率(bps)
	unsigned short vedioWidth;           //视频分辨率
	unsigned short vedioHeight;
}AudioInfoStatistic;

//msagent_CallBackEvent_getcpuInfo
typedef struct
{
	double  m_rate;               //cpu使用率
	int     m_memRate;            //内存使用率
}ms_agent_cpuInfo;

//媒体agent通知上层的事件
typedef void (*msagent_CallBackEvent)(msagent_CallBackEvent_enum cbEvent,void *pData, void* pUserData);

typedef struct msagent_MediaSdkAgentInfo
{
	char			*m_local_session_ip;		//与媒体服务器进行数据交互的地址
	unsigned short	m_local_session_port;
	char			*m_rc_addr;					//路由地址
	char            *m_mic1RecvMediaIp;
	int             m_mic1RecvMediaPort;
	char            *m_mic2RecvMediaIp;
	int             m_mic2RecvMediaPort;
	char            *m_sendMediaIp;
	int             m_sendMediaPort;
	char            *m_useId;
	char            *token;

	char			*m_log_config_file_dir;	//日志配置文件名（带全路径）
	char			*m_log_files_path;					//日志文件保存目录

	msagent_CallBackEvent pcbEvent;        //时间回调函数
	void            *puserData;             //发生事件回调时的用户数据

}msagent_MediaSdkAgentInfo;

typedef struct msagent_qosInfo_mic
{
	int worktype;      //工作模式，0-下行，1- 采集(上行)
	int upNetspeed;    //上行速率
	int downNetspeed;  //下行速率
	int upLossRate;    //上行丢包率
	int upLossFecRate; //上行fec后丢包率
	int downLossRate;  //下行丢包率
	int downFecRate;   //下行fec后丢包率
}msagent_qosInfo_mic;
typedef struct msagent_qosInfo
{
	msagent_qosInfo_mic mic1;
	msagent_qosInfo_mic mic2;

	char *pstrQOS;      //json字符串，描述界面显示
	int   QOSlen;
}msagent_qosInfo;

//发送的媒体格式
typedef struct _msagent_mediaFormat
{
	int  m_vedioRate;       //视频码率
	int  m_vedioFrameRate;  //视频帧率
	int  m_audioRate;       //音频码率
}msagent_mediaFormat;

typedef void (*msagent_ErrorReportCallBack)(CALL_BACK_TYPE call_back_type, int error_code, void* pUserData);
MEDIA_SDK_API int msagent_MediaSdkAgentInit(msagent_MediaSdkAgentInfo* pInfo); 

//参加会议
MEDIA_SDK_API int msagent_JoinMeeting(const char* meeting_id, msagent_ErrorReportCallBack cb, void* pUserData);
//退出会议
MEDIA_SDK_API int msagent_ExitMeeting(const char* meeting_id, void* pUserData);
//发言
MEDIA_SDK_API int msagent_Speak(const char* meeting_id, MICROPHONE_NUM microphone_num, void* pUserData);
//停止发言, 没有异步回调,同步返回结果;
MEDIA_SDK_API int msagent_StopSpeak(const char* meeting_id, MICROPHONE_NUM microphone_num);

/*获得mic的网络数据*/
MEDIA_SDK_API int msagent_GetQos(msagent_qosInfo *pQosInfo);

typedef void (*msagent_recvMediaCallBack)(MICROPHONE_NUM microphone_num, MEDIA_DATA_TYPE media_data_type,char *pframe,int len);
MEDIA_SDK_API int msagent_SetRecvMediaCB(msagent_recvMediaCallBack cbParam);
MEDIA_SDK_API void msagent_sendFrame( MEDIA_DATA_TYPE media_data_type,char *pframe,int len);

#ifdef __IOS__
MEDIA_SDK_API int msagent_MediaSdkAgentRelease();
#endif

/*在每次媒体格式发生变化时调用。包括每次开始发言时也得调用*/
MEDIA_SDK_API  void  msagent_setMediaFormat(msagent_mediaFormat *pmediaFormat);
/***********************************下面的接口废弃************************************/
typedef void (*ErrorReportCallBack)(CALL_BACK_TYPE call_back_type, int error_code, void* pUserData);

//参加会议
MEDIA_SDK_API int JoinMeeting(const char* meeting_id, ErrorReportCallBack cb, void* pUserData);
//退出会议
MEDIA_SDK_API int ExitMeeting(const char* meeting_id, void* pUserData);
//发言
MEDIA_SDK_API int Speak(const char* meeting_id, MICROPHONE_NUM microphone_num, void* pUserData);
//停止发言, 没有异步回调,同步返回结果;
MEDIA_SDK_API int StopSpeak(const char* meeting_id, MICROPHONE_NUM microphone_num);

#endif
