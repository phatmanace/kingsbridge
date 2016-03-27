#include <stdlib.h>
#include "string.h"

char* makeRepeatedString(char repeatingChar, int count){
    char* tmp = malloc(count);
    memset(tmp, 0, count);
    int x = 0 ;
    for(x = 0;x< count;x++){
        *(tmp + x) = repeatingChar;
    }
    *(tmp + count) = '\0';
    return tmp;
}

char* substring(const char* instr, int len)
{
	if(instr == NULL){
		return NULL;
	}
	char *ret = malloc((sizeof(char*) * len) + 1);
	int _len = len;
	if(strlen(instr) < len){
		_len = strlen(instr);
		}
	strncpy(ret, instr, _len);
	ret[_len + 1] = '\0';
	return ret;
}
