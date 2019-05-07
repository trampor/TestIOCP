#include "stdafx.h"
#include "RtspServer.h"


CRtspServer::CRtspServer()
{
	m_pTempDataBuf = NULL;
	m_nTempDataSize = 0;
	m_nAnaPhase = 0;

	m_RtspState = RTSP_STATE_IDLE;
}


CRtspServer::~CRtspServer()
{
}

int CRtspServer::ReadedDataProcess(unsigned char* pdatabuf, int datasize)
{
	pdatabuf[datasize] = 0;
	//OutputDebugStringA((char*)pdatabuf);

	unsigned char method[10];
	char uri[500];

	int readpos = 0, linesize ;
	int ret;
	do
	{
		linesize = 0;
		ret = read_line(pdatabuf, datasize, readpos, m_LineBuf, LINEBUFSIZE, linesize);
		if (ret < 0)
			break;

		if (linesize > 0)
		{
			if (m_nAnaPhase == 0)
			{
				ret = parse_command_line((char*)m_LineBuf, linesize, uri, sizeof(uri), (char*)method, sizeof(method), &methodcode);
				if (ret == 0)
				{
					memset(&m_RtspReplyMessage, 0, sizeof(RTSPMessageHeader));
					OutputDebugStringA((char*)method);
					OutputDebugStringA((char*)uri);
					m_nAnaPhase = 1;
				}
				else
					OutputDebugStringA((char*)m_LineBuf);
			}
			else
			{
				parse_nocommand_line((char*)m_LineBuf,(char*)method);
			}
		}
		else
		{
			if (m_nAnaPhase == 1)
			{
				//收到完整的rtsp请求,下一步处理
				OutputDebugString(_T("CRtspServer::ReadedDataProcess parse rtsp request suc"));
				m_nAnaPhase = 0;
				switch (methodcode)
				{
					case DESCRIBE:
						break;
					case ANNOUNCE:
						break;
					case OPTIONS:
						rtsp_send_reply(RTSP_STATUS_OK, "Public: ANNOUNCE, PAUSE, SETUP, PLAY, TEARDOWN, RECORD\r\n", m_RtspReplyMessage.seq);
						break;
					case SETUP:
						rtsp_setup_reply(uri,uri); 
						break;
					case PLAY:
						rtsp_play_reply(RTSP_STATUS_OK);
						break;
					case PAUSE:
						break;
					case TEARDOWN:
						rtsp_teardown_reply(RTSP_STATUS_OK);
						break;
					case GET_PARAMETER:
						break;
					case SET_PARAMETER:
						break;
					case REDIRECT:
						break;
					case RECORD:
						break;
					case UNKNOWN:
						break;
					default:
						break;
				}
			}
			else
			{
				OutputDebugString(_T("CRtspServer::ReadedDataProcess unexpected rtsp end"));//意外结束的rtsp请求
			}
		}

		if (readpos == datasize) //数据读完了
			break;
	} while (1);

	m_BufList.PushTail(pdatabuf);
	return 0;
}

int CRtspServer::read_line(unsigned char* pdatabuf, int datasize,int& readpos, unsigned char* plinebuf, int linebufsize, int& linelen)
{
	int idx = readpos;
	int ret = 0;
	bool breturn = false,bnext = false;
	linelen = 0;

	if (m_nTempDataSize >= linebufsize)
	{
		if (m_pTempDataBuf != NULL)
		{
			delete m_pTempDataBuf;
			m_pTempDataBuf = NULL;
		}
		m_nTempDataSize = 0;
	}
	if (m_nTempDataSize > 0)
	{
		memcpy(plinebuf, m_pTempDataBuf, m_nTempDataSize);
		if (m_pTempDataBuf != NULL)
		{
			delete m_pTempDataBuf;
			m_pTempDataBuf = NULL;
		}
		m_nTempDataSize = 0;
		linelen = m_nTempDataSize;
		if (plinebuf[m_nTempDataSize - 1] == '\r')
		{
			breturn = true;
		}
		else if (plinebuf[m_nTempDataSize - 1] == '\n')
		{
			bnext = true;
		}
	}

	do {
		if (pdatabuf[idx] == '\r') {
			idx++;
			if (bnext) //遇到完整的行尾\n\r
			{
				plinebuf[linelen - 1] = '\0';
				linelen -= 1;
				readpos = idx;
				return 0;
			}
			else
			{
				if (1 + linelen > linebufsize)
					linelen = 0;
				breturn = true;
				plinebuf[linelen] = '\r';
				linelen += 1;
			}
		}
		else if (pdatabuf[idx] == '\n') {
			idx++;
			if (breturn) //遇到完整的行尾\r\n
			{
				plinebuf[linelen - 1] = '\0';
				linelen -= 1;
				readpos = idx;
				return 0;
			}
			else
			{
				if (1 + linelen > linebufsize)
					linelen = 0;
				bnext = true;
				plinebuf[linelen] = '\r';
				linelen += 1;
			}
		}
		else
		{
			if (breturn)
			{
				breturn = false;
			}
			else if (bnext)
			{
				bnext = false;
			}
			if (1 + linelen > linebufsize)
				linelen = 0;
			plinebuf[linelen] = pdatabuf[idx];
			linelen++;
			idx++;
		}
	} while (idx < datasize);

	if (linelen > 0)
	{
		m_nTempDataSize = linelen;
		m_pTempDataBuf = new unsigned char[m_nTempDataSize];
		memcpy(m_pTempDataBuf, plinebuf, m_nTempDataSize);
		linelen = 0;
	}

	return -1; //没有找到完整行
}


int CRtspServer::parse_command_line(const char *line, int linelen, char *uri, int urisize, char *method, int methodsize, enum RTSPMethod *methodcode)
{
	const char *linept, *searchlinept;
	linept = strchr(line, ' ');

	if (!linept) {
		OutputDebugString(_T("CRtspServer::parse_command_line Error parsing method string"));
		return -1;
	}

	if (linept - line > methodsize - 1) {
		OutputDebugString(_T("CRtspServer::parse_command_line Method string too long"));
		return -2;
	}
	memcpy(method, line, linept - line);
	method[linept - line] = '\0';
	linept++;
	if (!strcmp(method, "ANNOUNCE"))
		*methodcode = ANNOUNCE;
	else if (!strcmp(method, "DESCRIBE"))
		*methodcode = DESCRIBE;
	else if (!strcmp(method, "OPTIONS"))
		*methodcode = OPTIONS;
	else if (!strcmp(method, "RECORD"))
		*methodcode = RECORD;
	else if (!strcmp(method, "PLAY"))
		*methodcode = PLAY;
	else if (!strcmp(method, "SETUP"))
		*methodcode = SETUP;
	else if (!strcmp(method, "PAUSE"))
		*methodcode = PAUSE;
	else if (!strcmp(method, "GET_PARAMETER"))
		*methodcode = GET_PARAMETER;
	else if (!strcmp(method, "SET_PARAMETER"))
		*methodcode = SET_PARAMETER;
	else if (!strcmp(method, "REDIRECT"))
		*methodcode = REDIRECT;
	else if (!strcmp(method, "TEARDOWN"))
		*methodcode = TEARDOWN;
	else
		*methodcode = UNKNOWN;

	/* Check method with the state  */
	if (m_RtspState == RTSP_STATE_IDLE) {
		if ((*methodcode != ANNOUNCE) && (*methodcode != OPTIONS)) {
			//av_log(s, AV_LOG_ERROR, "Unexpected command in Idle State %s\n",line);
			return -3;// AVERROR_PROTOCOL_NOT_FOUND;
		}
	}
	else if (m_RtspState == RTSP_STATE_PAUSED) {
		if ((*methodcode != OPTIONS) && (*methodcode != RECORD)	&& (*methodcode != SETUP)) {
			//av_log(s, AV_LOG_ERROR, "Unexpected command in Paused State %s\n",line);
			return -3;// AVERROR_PROTOCOL_NOT_FOUND;
		}
	}
	else if (m_RtspState == RTSP_STATE_STREAMING) {
		if ((*methodcode != PAUSE) && (*methodcode != OPTIONS) && (*methodcode != TEARDOWN)) {
			//av_log(s, AV_LOG_ERROR, "Unexpected command in Streaming State %s\n", line);
			return -3;// AVERROR_PROTOCOL_NOT_FOUND;
		}
	}
	else {
		//av_log(s, AV_LOG_ERROR, "Unexpected State [%d]\n", rt->state);
		return -3;// AVERROR_PROTOCOL_NOT_FOUND;
	}

	searchlinept = strchr(linept, ' ');
	if (!searchlinept) {
		OutputDebugString(_T("CRtspServer::parse_command_line Error parsing message URI\n"));
		return -3;
	}
	if (searchlinept - linept > urisize - 1) {
		OutputDebugString(_T("CRtspServer::parse_command_line uri string length exceeded buffer size\n"));
		return -4;
	}
	memcpy(uri, linept, searchlinept - linept);
	uri[searchlinept - linept] = '\0';

	//if (strcmp(rt->control_uri, uri)) 
	{
		char host[128], path[512], auth[128];
		int port;
		char ctl_host[128], ctl_path[512], ctl_auth[128];
		int ctl_port;
		av_url_split(NULL, 0, auth, sizeof(auth), host, sizeof(host), &port,path, sizeof(path), uri);
		//av_url_split(NULL, 0, ctl_auth, sizeof(ctl_auth), ctl_host,sizeof(ctl_host), &ctl_port, ctl_path, sizeof(ctl_path),rt->control_uri);
		//if (strcmp(host, ctl_host))
		//	av_log(s, AV_LOG_INFO, "Host %s differs from expected %s\n",host, ctl_host);
		//if (strcmp(path, ctl_path) && *methodcode != SETUP)
		//	av_log(s, AV_LOG_WARNING, "WARNING: Path %s differs from expected %s\n", path, ctl_path);
		//if (*methodcode == ANNOUNCE) {
		//	av_log(s, AV_LOG_INFO, "CRtspServer::parse_command_line Updating control URI to %s\n", uri);
		//	av_strlcpy(rt->control_uri, uri, sizeof(rt->control_uri));
		//}
	}

	linept = searchlinept + 1;
	if (!av_strstart(linept, "RTSP/1.0", NULL)) {
		OutputDebugString(_T("CRtspServer::parse_command_line Error parsing protocol or version\n"));
		return -5;
	}
	return 0;
}

void CRtspServer::parse_nocommand_line(const char *buf, const char *method)
{
	const char *p = buf;

	/* NOTE: we do case independent match for broken servers */
	if (av_stristart(p, "Session:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Session"));
		int t;
		get_word_sep(m_RtspReplyMessage.session_id, sizeof(m_RtspReplyMessage.session_id), ";", &p);
		if (av_stristart(p, ";timeout=", &p) &&
			(t = strtol(p, NULL, 10)) > 0) {
			m_RtspReplyMessage.timeout = t;
		}
	}
	else if (av_stristart(p, "Content-Length:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Content-Length"));
		m_RtspReplyMessage.content_length = strtol(p, NULL, 10);
	}
	else if (av_stristart(p, "Transport:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Transport"));
		rtsp_parse_transport( p);
	}
	else if (av_stristart(p, "CSeq:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line CSeq"));
		m_RtspReplyMessage.seq = strtol(p, NULL, 10);
	}
	else if (av_stristart(p, "Range:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Range"));
		rtsp_parse_range_npt(p, &m_RtspReplyMessage.range_start, &m_RtspReplyMessage.range_end);
	}
	else if (av_stristart(p, "RealChallenge1:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line RealChallenge1"));
		p += strspn(p, SPACE_CHARS);
		av_strlcpy(m_RtspReplyMessage.real_challenge, p, sizeof(m_RtspReplyMessage.real_challenge));
	}
	else if (av_stristart(p, "Server:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Server"));
		p += strspn(p, SPACE_CHARS);
		av_strlcpy(m_RtspReplyMessage.server, p, sizeof(m_RtspReplyMessage.server));
	}
	else if (av_stristart(p, "Notice:", &p) || av_stristart(p, "X-Notice:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Notice"));
		m_RtspReplyMessage.notice = strtol(p, NULL, 10);
	}
	else if (av_stristart(p, "Location:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Location"));
		p += strspn(p, SPACE_CHARS);
		av_strlcpy(m_RtspReplyMessage.location, p, sizeof(m_RtspReplyMessage.location));
	}
	else if (av_stristart(p, "WWW-Authenticate:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line WWW-Authenticate"));
		//p += strspn(p, SPACE_CHARS);
		//ff_http_auth_handle_header(&rt->auth_state, "WWW-Authenticate", p);
	}
	else if (av_stristart(p, "Authentication-Info:", &p) ) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Authentication-Info"));
		//p += strspn(p, SPACE_CHARS);
		//ff_http_auth_handle_header(&rt->auth_state, "Authentication-Info", p);
	}
	else if (av_stristart(p, "Content-Base:", &p) ) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Content-Base"));
		//p += strspn(p, SPACE_CHARS);
		//if (method && !strcmp(method, "DESCRIBE"))
		//	av_strlcpy(rt->control_uri, p, sizeof(rt->control_uri));
	}
	else if (av_stristart(p, "RTP-Info:", &p) ) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line RTP-Info"));
		//p += strspn(p, SPACE_CHARS);
		//if (method && !strcmp(method, "PLAY"))
		//	rtsp_parse_rtp_info(rt, p);
	}
	else if (av_stristart(p, "Public:", &p) ) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Public"));
	/*	if (strstr(p, "GET_PARAMETER") && method && !strcmp(method, "OPTIONS"))
			rt->get_parameter_supported = 1;*/
	}
	else if (av_stristart(p, "x-Accept-Dynamic-Rate:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line x-Accept-Dynamic-Rate"));
		//p += strspn(p, SPACE_CHARS);
		//rt->accept_dynamic_rate = atoi(p);
	}
	else if (av_stristart(p, "Content-Type:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line Content-Type"));
		p += strspn(p, SPACE_CHARS);
		av_strlcpy(m_RtspReplyMessage.content_type, p, sizeof(m_RtspReplyMessage.content_type));
	}
	else if (av_stristart(p, "User-Agent:", &p)) {
		OutputDebugString(_T("CRtspServer::parse_nocommand_line User-Agent"));
		//p += strspn(p, SPACE_CHARS);
		//av_strlcpy(m_RtspReplyMessage.content_type, p, sizeof(m_RtspReplyMessage.content_type));
	}
	else
	{
		OutputDebugStringA(p);
	}
}

/* XXX: only one transport specification is parsed */
void CRtspServer::rtsp_parse_transport(const char *p)
{
	char transport_protocol[16];
	char profile[16];
	char lower_transport[16];
	char parameter[16];
	RTSPTransportField *th;
	char buf[256];

	m_RtspReplyMessage.nb_transports = 0;

	for (;;) {
		p += strspn(p, SPACE_CHARS);
		if (*p == '\0')
			break;

		th = &m_RtspReplyMessage.transports[m_RtspReplyMessage.nb_transports];
		th->mode_record = 1;
		get_word_sep(transport_protocol, sizeof(transport_protocol),
			"/", &p);
		if (!av_strcasecmp(transport_protocol, "rtp")) {
			get_word_sep(profile, sizeof(profile), "/;,", &p);
			lower_transport[0] = '\0';
			/* rtp/avp/<protocol> */
			if (*p == '/') {
				get_word_sep(lower_transport, sizeof(lower_transport),
					";,", &p);
			}
			th->transport = RTSP_TRANSPORT_RTP;
		}
		else if (!av_strcasecmp(transport_protocol, "x-pn-tng") ||
			!av_strcasecmp(transport_protocol, "x-real-rdt")) {
			/* x-pn-tng/<protocol> */
			get_word_sep(lower_transport, sizeof(lower_transport), "/;,", &p);
			profile[0] = '\0';
			th->transport = RTSP_TRANSPORT_RDT;
		}
		else if (!av_strcasecmp(transport_protocol, "raw")) {
			get_word_sep(profile, sizeof(profile), "/;,", &p);
			lower_transport[0] = '\0';
			/* raw/raw/<protocol> */
			if (*p == '/') {
				get_word_sep(lower_transport, sizeof(lower_transport),
					";,", &p);
			}
			th->transport = RTSP_TRANSPORT_RAW;
		}
		if (!av_strcasecmp(lower_transport, "TCP"))
			th->lower_transport = RTSP_LOWER_TRANSPORT_TCP;
		else
			th->lower_transport = RTSP_LOWER_TRANSPORT_UDP;

		if (*p == ';')
			p++;
		/* get each parameter */
		while (*p != '\0' && *p != ',') {
			get_word_sep(parameter, sizeof(parameter), "=;,", &p);
			if (!strcmp(parameter, "port")) {
				if (*p == '=') {
					p++;
					rtsp_parse_range(&th->port_min, &th->port_max, &p);
				}
			}
			else if (!strcmp(parameter, "client_port")) {
				if (*p == '=') {
					p++;
					rtsp_parse_range(&th->client_port_min,
						&th->client_port_max, &p);
				}
			}
			else if (!strcmp(parameter, "server_port")) {
				if (*p == '=') {
					p++;
					rtsp_parse_range(&th->server_port_min,
						&th->server_port_max, &p);
				}
			}
			else if (!strcmp(parameter, "interleaved")) {
				if (*p == '=') {
					p++;
					rtsp_parse_range(&th->interleaved_min,
						&th->interleaved_max, &p);
				}
			}
			else if (!strcmp(parameter, "multicast")) {
				if (th->lower_transport == RTSP_LOWER_TRANSPORT_UDP)
					th->lower_transport = RTSP_LOWER_TRANSPORT_UDP_MULTICAST;
			}
			else if (!strcmp(parameter, "ttl")) {
				if (*p == '=') {
					char *end;
					p++;
					th->ttl = strtol(p, &end, 10);
					p = end;
				}
			}
			else if (!strcmp(parameter, "destination")) {
				if (*p == '=') {
					p++;
					get_word_sep(buf, sizeof(buf), ";,", &p);
					get_sockaddr( buf, &th->destination);
				}
			}
			else if (!strcmp(parameter, "source")) {
				if (*p == '=') {
					p++;
					get_word_sep(buf, sizeof(buf), ";,", &p);
					av_strlcpy(th->source, buf, sizeof(th->source));
				}
			}
			else if (!strcmp(parameter, "mode")) {
				if (*p == '=') {
					p++;
					get_word_sep(buf, sizeof(buf), ";, ", &p);
					if (!strcmp(buf, "record") ||
						!strcmp(buf, "receive"))
						th->mode_record = 1;
				}
			}

			while (*p != ';' && *p != '\0' && *p != ',')
				p++;
			if (*p == ';')
				p++;
		}
		if (*p == ',')
			p++;

		m_RtspReplyMessage.nb_transports++;
		if (m_RtspReplyMessage.nb_transports >= RTSP_MAX_TRANSPORTS)
			break;
	}
}

int CRtspServer::rtsp_send_reply(enum RTSPStatusCode code, const char *extracontent, uint16_t seq)
{
	char message[4096];
	int index = 0;
	while (status_messages[index].code) {
		if (status_messages[index].code == code) {
			sprintf_s(message, sizeof(message), "RTSP/1.0 %d %s\r\n",
				code, status_messages[index].message);
			break;
		}
		index++;
	}
	if (!status_messages[index].code)
		return -1;// AVERROR(EINVAL);
	av_strlcatf(message, sizeof(message), "CSeq: %d\r\n", seq);
	av_strlcatf(message, sizeof(message), "Server: %s\r\n", "Beijing Cti Streaming Server");
	if (extracontent)
		av_strlcat(message, extracontent, sizeof(message));
	av_strlcat(message, "\r\n", sizeof(message));
	//av_log(s, AV_LOG_TRACE, "Sending response:\n%s", message);
	//OutputDebugStringA((char*)message);
	Write((unsigned char*)message, strlen(message));

	return 0;
}

int CRtspServer::rtsp_setup_reply(char* host, char *controlurl)
{
	int ret = 0;
	char url[1024];
	char responseheaders[1024];
	int localport = -1;
	int transportidx = 0;
	int streamid = 0;

	if (!m_RtspReplyMessage.nb_transports) {
		//av_log(s, AV_LOG_ERROR, "No transport defined in SETUP\n");
		return -1;// AVERROR_INVALIDDATA;
	}
	for (transportidx = 0; transportidx < m_RtspReplyMessage.nb_transports;	transportidx++) {
		if (!m_RtspReplyMessage.transports[transportidx].mode_record ||
			(m_RtspReplyMessage.transports[transportidx].lower_transport != RTSP_LOWER_TRANSPORT_UDP &&
			m_RtspReplyMessage.transports[transportidx].lower_transport != RTSP_LOWER_TRANSPORT_TCP)) {
			//av_log(s, AV_LOG_ERROR, "mode=record/receive not set or transport protocol not supported (yet)\n");
			return -1;// AVERROR_INVALIDDATA;
		}
	}
	if (m_RtspReplyMessage.nb_transports > 1)
		;// av_log(s, AV_LOG_WARNING, "More than one transport not supported, using first of all\n");
	localport = 1000;

	if (m_RtspReplyMessage.transports[0].lower_transport == RTSP_LOWER_TRANSPORT_TCP) {
		sprintf_s(responseheaders, sizeof(responseheaders), "Transport: "
			"RTP/AVP/TCP;unicast;mode=receive;interleaved=%d-%d"
			"\r\n", m_RtspReplyMessage.transports[0].interleaved_min,
			m_RtspReplyMessage.transports[0].interleaved_max);
	}
	else {
		localport = 1000;
		sprintf_s(responseheaders, sizeof(responseheaders), "Transport: "
			"RTP/AVP/UDP;unicast;mode=receive;source=%s;"
			"client_port=%d-%d;server_port=%d-%d\r\n",
			host, m_RtspReplyMessage.transports[0].client_port_min,
			m_RtspReplyMessage.transports[0].client_port_max, localport,
			localport + 1);
	}

	/* Establish sessionid if not previously set */
	/* Put this in a function? */
	/* RFC 2326: session id must be at least 8 digits */
	while (strlen(session_id) < 8)
		av_strlcatf(session_id, SESSIONID_LENGTH, "%u", 123456789);

	av_strlcatf(responseheaders, sizeof(responseheaders), "Session: %s\r\n",session_id);
	/* Send Reply */
	rtsp_send_reply(RTSP_STATUS_OK, responseheaders, m_RtspReplyMessage.seq);

	m_RtspState = RTSP_STATE_PAUSED;
	return 0;
}
int CRtspServer::rtsp_play_reply(enum RTSPStatusCode code)
{
	char responseheaders[1024];
	memset(responseheaders, 0, 1024);
	av_strlcatf(responseheaders, sizeof(responseheaders), "Session: %s\r\n", session_id);

	rtsp_send_reply(RTSP_STATUS_OK, responseheaders, m_RtspReplyMessage.seq);

	m_RtspState = RTSP_STATE_STREAMING;
	return 0;
}

int CRtspServer::rtsp_teardown_reply(enum RTSPStatusCode code)
{
	char responseheaders[1024];
	memset(responseheaders, 0, 1024);
	av_strlcatf(responseheaders, sizeof(responseheaders), "Session: %s\r\n", session_id);

	rtsp_send_reply(RTSP_STATUS_OK, responseheaders, m_RtspReplyMessage.seq);

	m_RtspState = RTSP_STATE_IDLE;
	return 0;
}