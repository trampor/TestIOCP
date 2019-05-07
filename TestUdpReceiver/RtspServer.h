#pragma once
#include "TcpWorker.h"
#include "utils.h"

#define LINEBUFSIZE 1024
#define SESSIONID_LENGTH 32

#define RTSP_DEFAULT_PORT   554
#define RTSPS_DEFAULT_PORT  322
#define RTSP_MAX_TRANSPORTS 8
#define RTSP_TCP_MAX_PACKET_SIZE 1472
#define RTSP_DEFAULT_NB_AUDIO_CHANNELS 1
#define RTSP_DEFAULT_AUDIO_SAMPLERATE 44100
#define RTSP_RTP_PORT_MIN 5000
#define RTSP_RTP_PORT_MAX 65000


enum RTSPMethod {
	DESCRIBE,
	ANNOUNCE,
	OPTIONS,
	SETUP,
	PLAY,
	PAUSE,
	TEARDOWN,
	GET_PARAMETER,
	SET_PARAMETER,
	REDIRECT,
	RECORD,
	UNKNOWN = -1,
};

/**
* Network layer over which RTP/etc packet data will be transported.
*/
enum RTSPLowerTransport {
	RTSP_LOWER_TRANSPORT_UDP = 0,           /**< UDP/unicast */
	RTSP_LOWER_TRANSPORT_TCP = 1,           /**< TCP; interleaved in RTSP */
	RTSP_LOWER_TRANSPORT_UDP_MULTICAST = 2, /**< UDP/multicast */
	RTSP_LOWER_TRANSPORT_NB,
	RTSP_LOWER_TRANSPORT_HTTP = 8,          /**< HTTP tunneled - not a proper
											transport mode as such,
											only for use via AVOptions */
											RTSP_LOWER_TRANSPORT_HTTPS,             /**< HTTPS tunneled */
											RTSP_LOWER_TRANSPORT_CUSTOM = 16,       /**< Custom IO - not a public
																					option for lower_transport_mask,
																					but set in the SDP demuxer based
																					on a flag. */
};

/**
* Packet profile of the data that we will be receiving. Real servers
* commonly send RDT (although they can sometimes send RTP as well),
* whereas most others will send RTP.
*/
enum RTSPTransport {
	RTSP_TRANSPORT_RTP, /**< Standards-compliant RTP */
	RTSP_TRANSPORT_RDT, /**< Realmedia Data Transport */
	RTSP_TRANSPORT_RAW, /**< Raw data (over UDP) */
	RTSP_TRANSPORT_NB
};

/**
* Transport mode for the RTSP data. This may be plain, or
* tunneled, which is done over HTTP.
*/
enum RTSPControlTransport {
	RTSP_MODE_PLAIN,   /**< Normal RTSP */
	RTSP_MODE_TUNNEL   /**< RTSP over HTTP (tunneling) */
};

/**
* Client state, i.e. whether we are currently receiving data (PLAYING) or
* setup-but-not-receiving (PAUSED). State can be changed in applications
* by calling av_read_play/pause().
*/
enum RTSPClientState {
	RTSP_STATE_IDLE,    /**< not initialized */
	RTSP_STATE_STREAMING, /**< initialized and sending/receiving data */
	RTSP_STATE_PAUSED,  /**< initialized, but not receiving data */
	RTSP_STATE_SEEKING, /**< initialized, requesting a seek */
};

/** RTSP handling */
enum RTSPStatusCode {
	RTSP_STATUS_INVALID = 0,
	RTSP_STATUS_CONTINUE = 100,
	RTSP_STATUS_OK = 200,
	RTSP_STATUS_CREATED = 201,
	RTSP_STATUS_LOW_ON_STORAGE_SPACE = 250,
	RTSP_STATUS_MULTIPLE_CHOICES = 300,
	RTSP_STATUS_MOVED_PERMANENTLY = 301,
	RTSP_STATUS_MOVED_TEMPORARILY = 302,
	RTSP_STATUS_SEE_OTHER = 303,
	RTSP_STATUS_NOT_MODIFIED = 304,
	RTSP_STATUS_USE_PROXY = 305,
	RTSP_STATUS_BAD_REQUEST = 400,
	RTSP_STATUS_UNAUTHORIZED = 401,
	RTSP_STATUS_PAYMENT_REQUIRED = 402,
	RTSP_STATUS_FORBIDDEN = 403,
	RTSP_STATUS_NOT_FOUND = 404,
	RTSP_STATUS_METHOD = 405,
	RTSP_STATUS_NOT_ACCEPTABLE = 406,
	RTSP_STATUS_PROXY_AUTH_REQUIRED = 407,
	RTSP_STATUS_REQ_TIME_OUT = 408,
	RTSP_STATUS_GONE = 410,
	RTSP_STATUS_LENGTH_REQUIRED = 411,
	RTSP_STATUS_PRECONDITION_FAILED = 412,
	RTSP_STATUS_REQ_ENTITY_2LARGE = 413,
	RTSP_STATUS_REQ_URI_2LARGE = 414,
	RTSP_STATUS_UNSUPPORTED_MTYPE = 415,
	RTSP_STATUS_PARAM_NOT_UNDERSTOOD = 451,
	RTSP_STATUS_CONFERENCE_NOT_FOUND = 452,
	RTSP_STATUS_BANDWIDTH = 453,
	RTSP_STATUS_SESSION = 454,
	RTSP_STATUS_STATE = 455,
	RTSP_STATUS_INVALID_HEADER_FIELD = 456,
	RTSP_STATUS_INVALID_RANGE = 457,
	RTSP_STATUS_RONLY_PARAMETER = 458,
	RTSP_STATUS_AGGREGATE = 459,
	RTSP_STATUS_ONLY_AGGREGATE = 460,
	RTSP_STATUS_TRANSPORT = 461,
	RTSP_STATUS_UNREACHABLE = 462,
	RTSP_STATUS_INTERNAL = 500,
	RTSP_STATUS_NOT_IMPLEMENTED = 501,
	RTSP_STATUS_BAD_GATEWAY = 502,
	RTSP_STATUS_SERVICE = 503,
	RTSP_STATUS_GATEWAY_TIME_OUT = 504,
	RTSP_STATUS_VERSION = 505,
	RTSP_STATUS_UNSUPPORTED_OPTION = 551,
};

/**
* This describes a single item in the "Transport:" line of one stream as
* negotiated by the SETUP RTSP command. Multiple transports are comma-
* separated ("Transport: x-read-rdt/tcp;interleaved=0-1,rtp/avp/udp;
* client_port=1000-1001;server_port=1800-1801") and described in separate
* RTSPTransportFields.
*/
typedef struct RTSPTransportField {
	/** interleave ids, if TCP transport; each TCP/RTSP data packet starts
	* with a '$', stream length and stream ID. If the stream ID is within
	* the range of this interleaved_min-max, then the packet belongs to
	* this stream. */
	int interleaved_min, interleaved_max;

	/** UDP multicast port range; the ports to which we should connect to
	* receive multicast UDP data. */
	int port_min, port_max;

	/** UDP client ports; these should be the local ports of the UDP RTP
	* (and RTCP) sockets over which we receive RTP/RTCP data. */
	int client_port_min, client_port_max;

	/** UDP unicast server port range; the ports to which we should connect
	* to receive unicast UDP RTP/RTCP data. */
	int server_port_min, server_port_max;

	/** time-to-live value (required for multicast); the amount of HOPs that
	* packets will be allowed to make before being discarded. */
	int ttl;

	/** transport set to record data */
	int mode_record;

	struct sockaddr_storage destination; /**< destination IP address */
	char source[INET6_ADDRSTRLEN + 1]; /**< source IP address */

	/** data/packet transport protocol; e.g. RTP or RDT */
	enum RTSPTransport transport;

	/** network layer transport protocol; e.g. TCP or UDP uni-/multicast */
	enum RTSPLowerTransport lower_transport;
} RTSPTransportField;

/**
* This describes the server response to each RTSP command.
*/
typedef struct RTSPMessageHeader {
	/** length of the data following this header */
	int content_length;

	enum RTSPStatusCode status_code; /**< response code from server */

	/** number of items in the 'transports' variable below */
	int nb_transports;

	/** Time range of the streams that the server will stream. In
	* AV_TIME_BASE unit, AV_NOPTS_VALUE if not used */
	int64_t range_start, range_end;

	/** describes the complete "Transport:" line of the server in response
	* to a SETUP RTSP command by the client */
	RTSPTransportField transports[RTSP_MAX_TRANSPORTS];

	int seq;                         /**< sequence number */

	/** the "Session:" field. This value is initially set by the server and
	* should be re-transmitted by the client in every RTSP command. */
	char session_id[512];

	/** the "Location:" field. This value is used to handle redirection.
	*/
	char location[4096];

	/** the "RealChallenge1:" field from the server */
	char real_challenge[64];

	/** the "Server: field, which can be used to identify some special-case
	* servers that are not 100% standards-compliant. We use this to identify
	* Windows Media Server, which has a value "WMServer/v.e.r.sion", where
	* version is a sequence of digits (e.g. 9.0.0.3372). Helix/Real servers
	* use something like "Helix [..] Server Version v.e.r.sion (platform)
	* (RealServer compatible)" or "RealServer Version v.e.r.sion (platform)",
	* where platform is the output of $uname -msr | sed 's/ /-/g'. */
	char server[64];

	/** The "timeout" comes as part of the server response to the "SETUP"
	* command, in the "Session: <xyz>[;timeout=<value>]" line. It is the
	* time, in seconds, that the server will go without traffic over the
	* RTSP/TCP connection before it closes the connection. To prevent
	* this, sent dummy requests (e.g. OPTIONS) with intervals smaller
	* than this value. */
	int timeout;

	/** The "Notice" or "X-Notice" field value. See
	* http://tools.ietf.org/html/draft-stiemerling-rtsp-announce-00
	* for a complete list of supported values. */
	int notice;

	/** The "reason" is meant to specify better the meaning of the error code
	* returned
	*/
	char reason[256];

	/**
	* Content type header
	*/
	char content_type[64];
} RTSPMessageHeader;

static const struct RTSPStatusMessage {
	enum RTSPStatusCode code;
	const char *message;
} status_messages[] = {
	{ RTSP_STATUS_OK, "OK" },
	{ RTSP_STATUS_METHOD, "Method Not Allowed" },
	{ RTSP_STATUS_BANDWIDTH, "Not Enough Bandwidth" },
	{ RTSP_STATUS_SESSION, "Session Not Found" },
	{ RTSP_STATUS_STATE, "Method Not Valid in This State" },
	{ RTSP_STATUS_AGGREGATE, "Aggregate operation not allowed" },
	{ RTSP_STATUS_ONLY_AGGREGATE, "Only aggregate operation allowed" },
	{ RTSP_STATUS_TRANSPORT, "Unsupported transport" },
	{ RTSP_STATUS_INTERNAL, "Internal Server Error" },
	{ RTSP_STATUS_SERVICE, "Service Unavailable" },
	{ RTSP_STATUS_VERSION, "RTSP Version not supported" },
	{ RTSP_STATUS_INVALID, "NULL" }
};

class CRtspServer : public CTcpWorker
{
public:
	CRtspServer();
	virtual ~CRtspServer();

	virtual int ReadedDataProcess(unsigned char* pdatabuf, int datasize);

private:
	int read_line(unsigned char* pdatabuf, int datasize, int& readpos, unsigned char* plinebuf, int linebufsize, int& linelen);
	int parse_command_line(const char *line, int linelen, char *uri, int urisize, char *method, int methodsize, enum RTSPMethod *methodcode);
	void parse_nocommand_line(const char *buf, const char *method);
	void rtsp_parse_transport(const char *p);
	int rtsp_send_reply(enum RTSPStatusCode code, const char *extracontent, uint16_t seq);
	int rtsp_setup_reply(char* host, char *controlurl);
	int rtsp_play_reply(enum RTSPStatusCode code);
	int rtsp_teardown_reply(enum RTSPStatusCode code);

private:
	unsigned char *m_pTempDataBuf, m_LineBuf[LINEBUFSIZE];
	int m_nTempDataSize;
	int m_nAnaPhase; //0 等待开始分析 1 分析过程中

	RTSPClientState m_RtspState;
	RTSPMessageHeader m_RtspReplyMessage;
	enum RTSPMethod methodcode;

	char session_id[SESSIONID_LENGTH];
};

