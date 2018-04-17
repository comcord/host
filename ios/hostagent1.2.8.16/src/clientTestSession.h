#pragma once

#include "meetingHostCmd.h"
#include "clientTestCmd.h"

class clientTestsession: public AsynModel::Session
{
public:
	clientTestsession();
	~clientTestsession(void);
	
	BEGIN_MSG_MAP(clientTestsession)
		MSG_HANDLER(MEETINGHOST_CMD_GETSHARE_RESP,onGetSharePesp)
		MSG_HANDLER(__HOST_LOCAL_GETSHARE,OnGetshareReq)
	END_MSG_MAP()

	BEGIN_TIMER_MAP(clientTestsession)
		//TIMER_HANDLER(1,respTimerOutTimer);
	END_TIMER_MAP()

	int onGetSharePesp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);
	int OnGetshareReq(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender);

};

