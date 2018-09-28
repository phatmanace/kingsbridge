#include <stdio.h>
#include <string.h>
#include <stdio.h>
#if defined(__APPLE__)
#  define COMMON_DIGEST_FOR_OPENSSL
#  include <CommonCrypto/CommonDigest.h>
#  define SHA1 CC_SHA1
#else
# include <openssl/sha.h>
#endif

void doHash(char* ibuf){
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	unsigned char obuf[20];
	unsigned char md[SHA_DIGEST_LENGTH];

	SHA1_Update(&ctx, ibuf, strlen(ibuf));

	SHA1_Final(&(md[0]), &ctx);

	int i=SHA_DIGEST_LENGTH;

	//printf("Sha Digest length is: %d\n", i);


        char *p=(void*)md;


	while(i--){
	//	printf("%02x-", *p++);
	}

	for (int i = 0; i < SHA_DIGEST_LENGTH / 4; i++) {
		for (int j = 0; j < 4; j++) {
		    printf( "%02X", md[i * 4 + j]);
			  //  c += 2;
		}
		printf( " ");
        //c += 1;
	    }
	 //   *(c - 1) = '\0';

	printf("\t [%s]\n", ibuf);
}
int main()
{

	doHash("One");
	doHash("one@");
	doHash("Two");
	return 0;
}
