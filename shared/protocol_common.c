#include "protocol_common.h"
#include <string.h>
#include <strings.h>

// Forward declarations for functions that must be provided by the including project
extern int HttpReplyCode(const uint8_t *data, size_t len);
extern bool HttpExtractHeader(const uint8_t *data, size_t len, const char *header, char *buf, size_t buf_len);
extern bool FindNLD(const uint8_t *data, size_t len, unsigned int level, const uint8_t **result, size_t *result_len);

// DPI redirects are global redirects to another domain
bool HttpReplyLooksLikeDPIRedirect(const uint8_t *data, size_t len, const char *host)
{
	char loc[256],*redirect_host, *p = NULL;
	int code;
	
	if (!host || !*host) return false;
	
	code = HttpReplyCode(data,len);
	
	if ((code!=302 && code!=307) || !HttpExtractHeader(data,len,"\nLocation:",loc,sizeof(loc))) return false;

	// something like : https://censor.net/badpage.php?reason=denied&source=RKN
		
	if (!strncmp(loc,"http://",7))
		redirect_host=loc+7;
	else if (!strncmp(loc,"https://",8))
		redirect_host=loc+8;
	else
		return false;
		
	// somethinkg like : censor.net/badpage.php?reason=denied&source=RKN
	
	for(p=redirect_host; *p && *p!='/' ; p++);
	*p=0;
	if (!*redirect_host) return false;

	// somethinkg like : censor.net
	
	// extract 2nd level domains
	const char *dhost, *drhost;
	if (!FindNLD((uint8_t*)host,strlen(host),2,(const uint8_t**)&dhost,NULL) || !FindNLD((uint8_t*)redirect_host,strlen(redirect_host),2,(const uint8_t**)&drhost,NULL))
		return false;

	// compare 2nd level domains		
	return strcasecmp(dhost, drhost)!=0;
}
