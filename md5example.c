#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
#  include <openssl/md5.h>
#endif

int main()
{
	char ibuf[] = "compute sha1";
	char* out = malloc(sizeof(char*) * 33);
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, ibuf, strlen(ibuf));
	unsigned char digest[16];

	  MD5_Final(digest, &c);

	    for (int n = 0; n < 16; ++n) {
		snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
	    }

	    printf("Digest: %s\n", out);

	return 0;
}
