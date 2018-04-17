#include "stdafx.h"
#include "meetingHostCmd.h"
#include "clientTestSession.h"

clientTestsession::clientTestsession()
{
}

clientTestsession::~clientTestsession(void)
{
}
int clientTestsession::onGetSharePesp(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	meetingHost_getShare_resp cmd;
	memset(&cmd,0,sizeof(cmd));
	cmd.UnSerialize(data,len);

	printf("获得分享地址:结果%d：user:%s,%s:%d,name:%s\n",cmd.m_result,cmd.m_userid,
		cmd.m_shareip,cmd.m_shareport,cmd.m_name);
	return 0;
}
int clientTestsession::OnGetshareReq(const char* data,U32 len,U32 srcsid,const AsynModel::SptrISender& sender)
{
	__host_local_getshare_req *preq = (__host_local_getshare_req*)data;

	meetingHost_getShare_req cmd;
	strncpy(cmd.m_userid,preq->userid,sizeof(cmd.m_userid));

	AsynModel::ConnID cid;
	strncpy(cid.openhost.openUDPIP,preq->ip,sizeof(cid.openhost.openUDPIP));
	cid.openhost.openUdpPort = preq->port;

	char buf[128];
	int slen;
	slen = cmd.Serialize(buf,sizeof(buf));

    AsynModel::AsynResultRet ret = PostMsg(cid, 0,MEETINGHOST_CMD_GETSHARE_REQ, (char*)buf, slen, false,SENMSGDEFAULTTIMROUT,AsynModel::POSTMSG_NO_SUBSCRIBE,AsynModel::UDP);
	if (0 != ret)
	{
		Log::writeError(MEETING_HOST_AGENT_LOG_SUBTYPE, 2000,"[host_agent]发送获取分享地址命令失败");
	}
	return 0;
}