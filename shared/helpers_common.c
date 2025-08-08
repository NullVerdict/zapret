#include "helpers_common.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <libgen.h>
#include <fcntl.h>

int unique_size_t(size_t *pu, int ct)
{
    int i, j;
    size_t u;
    for (i = j = 0; j < ct; i++)
    {
        u = pu[j++];
        for (; j < ct && pu[j] == u; j++);
        pu[i] = u;
    }
    return i;
}

static int cmp_size_t_qsort(const void *a, const void *b)
{
    const size_t *pa = (const size_t*)a;
    const size_t *pb = (const size_t*)b;
    return (*pa < *pb) ? -1 : (*pa > *pb);
}

void qsort_size_t(size_t *array, size_t ct)
{
    qsort(array, ct, sizeof(*array), cmp_size_t_qsort);
}

void rtrim(char *s)
{
	if (s)
		for (char *p = s + strlen(s) - 1; p >= s && (*p == '\n' || *p == '\r'); p--) *p = '\0';
}

void replace_char(char *s, char from, char to)
{
	for (; *s; s++) if (*s == from) *s = to;
}

const char *strncasestr(const char *s, const char *find, size_t slen)
{
	char c, sc;
	size_t len;

	c = *find++;
	if (c == '\0')
		return s;
	len = strlen(find);
	for (;;)
	{
		if (slen-- < 1) return NULL;
		sc = *s++;
		if (sc == '\0') return NULL;
		if (toupper(c) != toupper(sc))
			continue;
		if (len > slen)
			return NULL;
		if (strncasecmp(s, find, len) == 0)
			return s-1;
	}
}

bool load_file(const char *filename, void *buffer, size_t *buffer_size)
{
	FILE *F = fopen(filename, "rb");
	if (!F) return false;
	*buffer_size = fread(buffer, 1, *buffer_size, F);
	if (ferror(F)) { fclose(F); return false; }
	fclose(F);
	return true;
}

bool load_file_nonempty(const char *filename, void *buffer, size_t *buffer_size)
{
	bool b = load_file(filename, buffer, buffer_size);
	return b && *buffer_size;
}

bool save_file(const char *filename, const void *buffer, size_t buffer_size)
{
	FILE *F = fopen(filename, "wb");
	if (!F) return false;
	fwrite(buffer, 1, buffer_size, F);
	if (ferror(F)) { fclose(F); return false; }
	fclose(F);
	return true;
}

bool append_to_list_file(const char *filename, const char *s)
{
	FILE *F = fopen(filename, "at");
	if (!F) return false;
	bool bOK = fprintf(F, "%s\n", s) > 0;
	fclose(F);
	return bOK;
}

void expand_bits(void *target, const void *source, unsigned int source_bitlen, unsigned int target_bytelen)
{
	unsigned int target_bitlen = target_bytelen<<3;
	unsigned int bitlen = target_bitlen<source_bitlen ? target_bitlen : source_bitlen;
	unsigned int bytelen = bitlen>>3;
	if ((target_bytelen-bytelen)>=1) memset((uint8_t*)target+bytelen,0,target_bytelen-bytelen);
	memcpy(target,source,bytelen);
	if ((bitlen &= 7)) ((uint8_t*)target)[bytelen] = ((const uint8_t*)source)[bytelen] & (~((1 << (8-bitlen)) - 1));
}

bool strip_host_to_ip(char *host)
{
	size_t l;
	char *h,*p;
	uint8_t addr[16];

	for (h = host ; *h==' ' || *h=='\t' ; h++);
	l = strlen(h);
	if (l>=2)
	{
		if (*h=='[')
		{
			for (p=++h ; *p && *p!=']' ;  p++);
			if (*p==']')
			{
				l = p-h;
				memmove(host,h,l);
				host[l]=0;
				return inet_pton(AF_INET6, host, addr)>0;
			}
		}
		else
		{
			if (inet_pton(AF_INET6, h, addr)>0)
			{
				if (host!=h)
				{
					l = strlen(h);
					memmove(host,h,l);
					host[l]=0;
				}
				return true;
			}
			else
			{
				for (p=h ; *p && *p!=':' ;  p++);
				l = p-h;
				if (host!=h) memmove(host,h,l);
				host[l]=0;
				return inet_pton(AF_INET, host, addr)>0;
			}
		}
	}
	return false;
}

void ntop46(const struct sockaddr *sa, char *str, size_t len)
{
	if (!len) return;
	*str = 0;
	switch (sa->sa_family)
	{
	case AF_INET:
		inet_ntop(sa->sa_family, &((const struct sockaddr_in*)sa)->sin_addr, str, len);
		break;
	case AF_INET6:
		inet_ntop(sa->sa_family, &((const struct sockaddr_in6*)sa)->sin6_addr, str, len);
		break;
	default:
		snprintf(str, len, "UNKNOWN_FAMILY_%d", sa->sa_family);
	}
}

void ntop46_port(const struct sockaddr *sa, char *str, size_t len)
{
	char ip[40];
	ntop46(sa, ip, sizeof(ip));
	switch (sa->sa_family)
	{
	case AF_INET:
		snprintf(str, len, "%s:%u", ip, ntohs(((const struct sockaddr_in*)sa)->sin_port));
		break;
	case AF_INET6:
		snprintf(str, len, "[%s]:%u", ip, ntohs(((const struct sockaddr_in6*)sa)->sin6_port));
		break;
	default:
		snprintf(str, len, "%s", ip);
	}
}

void print_sockaddr(const struct sockaddr *sa)
{
	char ip_port[48];
	ntop46_port(sa, ip_port, sizeof(ip_port));
	printf("%s", ip_port);
}