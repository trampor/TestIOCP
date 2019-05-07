#pragma once

#define int64_t __int64
#define uint16_t unsigned short
#define uint32_t unsigned int
#define UINT64_C unsigned __int64
#define uint8_t unsigned char
#define SPACE_CHARS " \t\r\n"
/**
* @brief Undefined timestamp value
*
* Usually reported by demuxer that work on containers that do not provide
* either pts or dts.
*/

#define AV_NOPTS_VALUE ((int64_t)UINT64_C(0x8000000000000000))

#define FFDIFFSIGN(x,y) (((x)>(y)) - ((x)<(y)))

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMAX3(a,b,c) FFMAX(FFMAX(a,b),c)
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FFMIN3(a,b,c) FFMIN(FFMIN(a,b),c)

#define MKTAG(a,b,c,d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))
#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))

#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

void rtsp_parse_range(int *min_ptr, int *max_ptr, const char **pp);
void get_word_sep(char *buf, int buf_size, const char *sep, const char **pp);
int av_stristart(const char *str, const char *pfx, const char **ptr);
int av_strcasecmp(const char *a, const char *b);
int get_sockaddr(const char *buf, struct sockaddr_storage *sock);
void av_url_split(char *proto, int proto_size,char *authorization, int authorization_size,char *hostname, int hostname_size,
	int *port_ptr, char *path, int path_size, const char *url);
int av_strstart(const char *str, const char *pfx, const char **ptr);
size_t av_strlcpy(char *dst, const char *src, size_t size);
void rtsp_parse_range_npt(const char *p, int64_t *start, int64_t *end);
size_t av_strlcat(char *dst, const char *src, size_t size);
size_t av_strlcatf(char *dst, size_t size, const char *fmt, ...);

