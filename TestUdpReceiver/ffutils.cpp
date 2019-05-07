//#pragma once
#include "stdafx.h"
#include "utils.h"


static const char months[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/**
* Locale-independent conversion of ASCII isdigit.
*/
int av_isdigit(int c)
{
	return c >= '0' && c <= '9';
}

/**
* Locale-independent conversion of ASCII isgraph.
*/
int av_isgraph(int c)
{
	return c > 32 && c < 127;
}

/**
* Locale-independent conversion of ASCII isspace.
*/
int av_isspace(int c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
		c == '\v';
}

int av_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		c ^= 0x20;
	return c;
}
int av_isxdigit(int c)
{
	c = av_tolower(c);
	return av_isdigit(c) || (c >= 'a' && c <= 'f');
}


int av_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		c ^= 0x20;
	return c;
}

int av_strstart(const char *str, const char *pfx, const char **ptr)
{
	while (*pfx && *pfx == *str) {
		pfx++;
		str++;
	}
	if (!*pfx && ptr)
		*ptr = str;
	return !*pfx;
}

int av_stristart(const char *str, const char *pfx, const char **ptr)
{
	while (*pfx && av_toupper((unsigned)*pfx) == av_toupper((unsigned)*str)) {
		pfx++;
		str++;
	}
	if (!*pfx && ptr)
		*ptr = str;
	return !*pfx;
}

char *av_stristr(const char *s1, const char *s2)
{
	if (!*s2)
		return (char*)(intptr_t)s1;

	do
		if (av_stristart(s1, s2, NULL))
			return (char*)(intptr_t)s1;
	while (*s1++);

	return NULL;
}

char *av_strnstr(const char *haystack, const char *needle, size_t hay_length)
{
	size_t needle_len = strlen(needle);
	if (!needle_len)
		return (char*)haystack;
	while (hay_length >= needle_len) {
		hay_length--;
		if (!memcmp(haystack, needle, needle_len))
			return (char*)haystack;
		haystack++;
	}
	return NULL;
} 

inline size_t av_strlcpy(char *dst, const char *src, size_t size)
{
	size_t len = 0;
	while (++len < size && *src)
		*dst++ = *src++;
	if (len <= size)
		*dst = 0;
	return len + strlen(src) - 1;
}

size_t av_strlcat(char *dst, const char *src, size_t size)
{
	size_t len = strlen(dst);
	if (size <= len + 1)
		return len + strlen(src);
	return len + av_strlcpy(dst + len, src, size - len);
}

int av_strcasecmp(const char *a, const char *b)
{
	uint8_t c1, c2;
	do {
		c1 = av_tolower(*a++);
		c2 = av_tolower(*b++);
	} while (c1 && c1 == c2);
	return c1 - c2;
}

int av_strncasecmp(const char *a, const char *b, size_t n)
{
	uint8_t c1, c2;
	if (n <= 0)
		return 0;
	do {
		c1 = av_tolower(*a++);
		c2 = av_tolower(*b++);
	} while (--n && c1 && c1 == c2);
	return c1 - c2;
}

void av_url_split(char *proto, int proto_size,
	char *authorization, int authorization_size,
	char *hostname, int hostname_size,
	int *port_ptr, char *path, int path_size, const char *url)
{
	const char *p, *ls, *ls2, *ls3, *at, *at2, *col, *brk;

	if (port_ptr)
		*port_ptr = -1;
	if (proto_size > 0)
		proto[0] = 0;
	if (authorization_size > 0)
		authorization[0] = 0;
	if (hostname_size > 0)
		hostname[0] = 0;
	if (path_size > 0)
		path[0] = 0;

	/* parse protocol */
	if ((p = strchr(url, ':'))) {
		av_strlcpy(proto, url, FFMIN(proto_size, p + 1 - url));
		p++; /* skip ':' */
		if (*p == '/')
			p++;
		if (*p == '/')
			p++;
	}
	else {
		/* no protocol means plain filename */
		av_strlcpy(path, url, path_size);
		return;
	}

	/* separate path from hostname */
	ls = strchr(p, '/');
	ls2 = strchr(p, '?');
	ls3 = strchr(p, '@');
	if (ls3 && ls3 > ls && (!ls2 || ls2 > ls3))
		ls = strchr(ls3, '/');
	if (!ls)
		ls = ls2;
	else if (ls && ls2)
		ls = FFMIN(ls, ls2);
	if (ls)
		av_strlcpy(path, ls, path_size);
	else
		ls = &p[strlen(p)];  // XXX

	/* the rest is hostname, use that to parse auth/port */
	if (ls != p) {
		/* authorization (user[:pass]@hostname) */
		at2 = p;
		while ((at = strchr(p, '@')) && at < ls) {
			av_strlcpy(authorization, at2,
				FFMIN(authorization_size, at + 1 - at2));
			p = at + 1; /* skip '@' */
		}

		if (*p == '[' && (brk = strchr(p, ']')) && brk < ls) {
			/* [host]:port */
			av_strlcpy(hostname, p + 1,
				FFMIN(hostname_size, brk - p));
			if (brk[1] == ':' && port_ptr)
				*port_ptr = atoi(brk + 2);
		}
		else if ((col = strchr(p, ':')) && col < ls) {
			av_strlcpy(hostname, p,
				FFMIN(col + 1 - p, hostname_size));
			if (port_ptr)
				*port_ptr = atoi(col + 1);
		}
		else
			av_strlcpy(hostname, p,
			FFMIN(ls + 1 - p, hostname_size));
	}
}

void get_word_until_chars(char *buf, int buf_size,
	const char *sep, const char **pp)
{
	const char *p;
	char *q;

	p = *pp;
	p += strspn(p, SPACE_CHARS);
	q = buf;
	while (!strchr(sep, *p) && *p != '\0') {
		if ((q - buf) < buf_size - 1)
			*q++ = *p;
		p++;
	}
	if (buf_size > 0)
		*q = '\0';
	*pp = p;
}

void get_word_sep(char *buf, int buf_size, const char *sep,	const char **pp)
{
	if (**pp == '/') (*pp)++;
	get_word_until_chars(buf, buf_size, sep, pp);
}

void get_word(char *buf, int buf_size, const char **pp)
{
	get_word_until_chars(buf, buf_size, SPACE_CHARS, pp);
}

void rtsp_parse_range(int *min_ptr, int *max_ptr, const char **pp)
{
	const char *q;
	char *p;
	int v;

	q = *pp;
	q += strspn(q, SPACE_CHARS);
	v = strtol(q, &p, 10);
	if (*p == '-') {
		p++;
		*min_ptr = v;
		v = strtol(p, &p, 10);
		*max_ptr = v;
	}
	else {
		*min_ptr = v;
		*max_ptr = v;
	}
	*pp = p;
}
/* get a positive number between n_min and n_max, for a maximum length
of len_max. Return -1 if error. */
int date_get_num(const char **pp,
	int n_min, int n_max, int len_max)
{
	int i, val, c;
	const char *p;

	p = *pp;
	val = 0;
	for (i = 0; i < len_max; i++) {
		c = *p;
		if (!av_isdigit(c))
			break;
		val = (val * 10) + c - '0';
		p++;
	}
	/* no number read ? */
	if (p == *pp)
		return -1;
	if (val < n_min || val > n_max)
		return -1;
	*pp = p;
	return val;
}

int date_get_month(const char **pp) {
	int i = 0;
	for (; i < 12; i++) {
		if (!av_strncasecmp(*pp, months[i], 3)) {
			const char *mo_full = months[i] + 3;
			int len = strlen(mo_full);
			*pp += 3;
			if (len > 0 && !av_strncasecmp(*pp, mo_full, len))
				*pp += len;
			return i;
		}
	}
	return -1;
}
char *av_small_strptime(const char *p, const char *fmt, struct tm *dt)
{
	int c, val;

	while ((c = *fmt++)) {
		if (c != '%') {
			if (av_isspace(c))
				for (; *p && av_isspace(*p); p++);
			else if (*p != c)
				return NULL;
			else p++;
			continue;
		}

		c = *fmt++;
		switch (c) {
		case 'H':
		case 'J':
			val = date_get_num(&p, 0, c == 'H' ? 23 : INT_MAX, c == 'H' ? 2 : 4);

			if (val == -1)
				return NULL;
			dt->tm_hour = val;
			break;
		case 'M':
			val = date_get_num(&p, 0, 59, 2);
			if (val == -1)
				return NULL;
			dt->tm_min = val;
			break;
		case 'S':
			val = date_get_num(&p, 0, 59, 2);
			if (val == -1)
				return NULL;
			dt->tm_sec = val;
			break;
		case 'Y':
			val = date_get_num(&p, 0, 9999, 4);
			if (val == -1)
				return NULL;
			dt->tm_year = val - 1900;
			break;
		case 'm':
			val = date_get_num(&p, 1, 12, 2);
			if (val == -1)
				return NULL;
			dt->tm_mon = val - 1;
			break;
		case 'd':
			val = date_get_num(&p, 1, 31, 2);
			if (val == -1)
				return NULL;
			dt->tm_mday = val;
			break;
		case 'T':
			p = av_small_strptime(p, "%H:%M:%S", dt);
			if (!p)
				return NULL;
			break;
		case 'b':
		case 'B':
		case 'h':
			val = date_get_month(&p);
			if (val == -1)
				return NULL;
			dt->tm_mon = val;
			break;
		case '%':
			if (*p++ != '%')
				return NULL;
			break;
		default:
			return NULL;
		}
	}

	return (char*)p;
}

int64_t av_gettime(void)
{
#if HAVE_GETTIMEOFDAY
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif HAVE_GETSYSTEMTIMEASFILETIME
	FILETIME ft;
	int64_t t;
	GetSystemTimeAsFileTime(&ft);
	t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
	return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else
	return -1;
#endif
}

#if !HAVE_GMTIME_R && !defined(gmtime_r)
struct tm *gmtime_r(const time_t* clock, struct tm *result)
{
	errno_t err = gmtime_s(result,clock);

	return result;
}
#endif

#if !HAVE_LOCALTIME_R && !defined(localtime_r)
struct tm *localtime_r(const time_t* clock, struct tm *result)
{
	errno_t err = localtime_s(result, clock);

	return result;
}
#endif

time_t av_timegm(struct tm *tm)
{
	time_t t;

	int y = tm->tm_year + 1900, m = tm->tm_mon + 1, d = tm->tm_mday;

	if (m < 3) {
		m += 12;
		y--;
	}

	t = 86400LL *
		(d + (153 * m - 457) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 719469);

	t += 3600 * tm->tm_hour + 60 * tm->tm_min + tm->tm_sec;

	return t;
}

int av_parse_time(int64_t *timeval, const char *timestr, int duration)
{
	const char *p, *q;
	int64_t t, now64;
	time_t now;
	struct tm dt = { 0 }, tmbuf;
	int today = 0, negative = 0, microseconds = 0, suffix = 1000000;
	int i;
	static const char * const date_fmt[] = {
		"%Y - %m - %d",
		"%Y%m%d",
	};
	static const char * const time_fmt[] = {
		"%H:%M:%S",
		"%H%M%S",
	};
	static const char * const tz_fmt[] = {
		"%H:%M",
		"%H%M",
		"%H",
	};

	p = timestr;
	q = NULL;
	*timeval = INT64_MIN;
	if (!duration) {
		now64 = av_gettime();
		now = now64 / 1000000;

		if (!av_strcasecmp(timestr, "now")) {
			*timeval = now64;
			return 0;
		}

		/* parse the year-month-day part */
		for (i = 0; i < FF_ARRAY_ELEMS(date_fmt); i++) {
			q = av_small_strptime(p, date_fmt[i], &dt);
			if (q)
				break;
		}

		/* if the year-month-day part is missing, then take the
		* current year-month-day time */
		if (!q) {
			today = 1;
			q = p;
		}
		p = q;

		if (*p == 'T' || *p == 't')
			p++;
		else
			while (av_isspace(*p))
				p++;

		/* parse the hour-minute-second part */
		for (i = 0; i < FF_ARRAY_ELEMS(time_fmt); i++) {
			q = av_small_strptime(p, time_fmt[i], &dt);
			if (q)
				break;
		}
	}
	else {
		/* parse timestr as a duration */
		if (p[0] == '-') {
			negative = 1;
			++p;
		}
		/* parse timestr as HH:MM:SS */
		q = av_small_strptime(p, "%J:%M:%S", &dt);
		if (!q) {
			/* parse timestr as MM:SS */
			q = av_small_strptime(p, "%M:%S", &dt);
			dt.tm_hour = 0;
		}
		if (!q) {
			char *o;
			/* parse timestr as S+ */
			errno = 0;
			t = strtoll(p, &o, 10);
			if (o == p) /* the parsing didn't succeed */
				return -1;//(EINVAL);
			if (errno == ERANGE)
				return -1;//(ERANGE);
			q = o;
		}
		else {
			t = dt.tm_hour * 3600 + dt.tm_min * 60 + dt.tm_sec;
		}
	}

	/* Now we have all the fields that we can get */
	if (!q)
		return -1;//(EINVAL);

	/* parse the .m... part */
	if (*q == '.') {
		int n;
		q++;
		for (n = 100000; n >= 1; n /= 10, q++) {
			if (!av_isdigit(*q))
				break;
			microseconds += n * (*q - '0');
		}
		while (av_isdigit(*q))
			q++;
	}

	if (duration) {
		if (q[0] == 'm' && q[1] == 's') {
			suffix = 1000;
			microseconds /= 1000;
			q += 2;
		}
		else if (q[0] == 'u' && q[1] == 's') {
			suffix = 1;
			microseconds = 0;
			q += 2;
		}
		else if (*q == 's')
			q++;
	}
	else {
		int is_utc = *q == 'Z' || *q == 'z';
		int tzoffset = 0;
		q += is_utc;
		if (!today && !is_utc && (*q == '+' || *q == '-')) {
			struct tm tz = { 0 };
			int sign = (*q == '+' ? -1 : 1);
			q++;
			p = q;
			for (i = 0; i < FF_ARRAY_ELEMS(tz_fmt); i++) {
				q = av_small_strptime(p, tz_fmt[i], &tz);
				if (q)
					break;
			}
			if (!q)
				return -1;//(EINVAL);
			tzoffset = sign * (tz.tm_hour * 60 + tz.tm_min) * 60;
			is_utc = 1;
		}
		if (today) { /* fill in today's date */
			struct tm dt2 = is_utc ? *gmtime_r(&now, &tmbuf) : *localtime_r(&now, &tmbuf);
			dt2.tm_hour = dt.tm_hour;
			dt2.tm_min = dt.tm_min;
			dt2.tm_sec = dt.tm_sec;
			dt = dt2;
		}
		dt.tm_isdst = is_utc ? 0 : -1;
		t = is_utc ? av_timegm(&dt) : mktime(&dt);
		t += tzoffset;
	}

	/* Check that we are at the end of the string */
	if (*q)
		return -1;//(EINVAL);

	if (INT64_MAX / suffix < t)
		return -1;//(ERANGE);
	t *= suffix;
	if (INT64_MAX - microseconds < t)
		return -1;//(ERANGE);
	t += microseconds;
	*timeval = negative ? -t : t;
	return 0;
}

/** Parse a string p in the form of Range:npt=xx-xx, and determine the start
*  and end time.
*  Used for seeking in the rtp stream.
*/
void rtsp_parse_range_npt(const char *p, int64_t *start, int64_t *end)
{
	char buf[256];

	p += strspn(p, SPACE_CHARS);
	if (!av_stristart(p, "npt=", &p))
		return;

	*start = AV_NOPTS_VALUE;
	*end = AV_NOPTS_VALUE;

	get_word_sep(buf, sizeof(buf), "-", &p);
	if (av_parse_time(start, buf, 1) < 0)
		return;
	if (*p == '-') {
		p++;
		get_word_sep(buf, sizeof(buf), "-", &p);
		if (av_parse_time(end, buf, 1) < 0)
			;// av_log(NULL, AV_LOG_DEBUG, "Failed to parse interval end specification '%s'\n", buf);
	}
}

int get_sockaddr(const char *buf, struct sockaddr_storage *sock)
{
	struct addrinfo hints = { 0 }, *ai = NULL;
	int ret;

	hints.ai_flags = AI_NUMERICHOST;
	if ((ret = getaddrinfo(buf, NULL, &hints, &ai))) {
		//av_log(s, AV_LOG_ERROR, "getaddrinfo(%s): %s\n",buf,gai_strerror(ret));
		return -1;
	}
	memcpy(sock, ai->ai_addr, FFMIN(sizeof(*sock), ai->ai_addrlen));
	freeaddrinfo(ai);
	return 0;
}

size_t av_strlcatf(char *dst, size_t size, const char *fmt, ...)
{
	size_t len = strlen(dst);
	va_list vl;

	va_start(vl, fmt);
	len += vsnprintf(dst + len, size > len ? size - len : 0, fmt, vl);
	va_end(vl);

	return len;
}
