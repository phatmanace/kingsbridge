#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "string_utils.h"

char* makeRepeatedString(char repeatingChar, int count)
{
	char* tmp = malloc(count);

	memset(tmp, 0, count);
	int x = 0;
	for (x = 0; x < count; x++) {
		*(tmp + x) = repeatingChar;
	}
	*(tmp + count) = '\0';
	return tmp;
}

bool isvalueinarray(int val, int *arr, int size){
	    int i;
	        for (i=0; i < size; i++) {
			if (arr[i] == val)
					    return true;
	    }
	    return false;
}
char* substring(const char* instr, int len)
{
	if (instr == NULL) {
		return NULL;
	}
	char *ret = malloc((sizeof(char*) * len) + 1);
	int _len = len;
	if (strlen(instr) < len) {
		_len = strlen(instr);
	}
	strncpy(ret, instr, _len);
	ret[_len + 1] = '\0';
	return ret;
}
s_segments* splitIntoSegments(char *instr, int width)
{

	char* from[1] = {"&#x27;"};
	char* to[1]   = {"'"};

	char* teststr = searchReplace(instr, from, to, 1);
	printf("Replaced: %s\n", teststr);

	s_segments* segs = malloc(sizeof(segs)); 
	segs->count = 0;

	segs->segments = malloc(sizeof(ss_string) * 50);


	int _p = 0;
	int _l = strlen(teststr);

	int* pointers = malloc(sizeof(int) * 200);
	int ray_size = 1;
	int ray_pointer = 0;


	for(_p = 0;_p < _l - 3;_p++){
		char otherString[] = "000";
		memcpy(otherString, teststr + _p, 3);
		if(strcmp(otherString, "<p>") == 0){
			pointers[ray_pointer++] = _p;
		}	
	}


	int _last_wb    = -1;
	int _last_start =  0;
	int _last_stop  =  0;
	for(_p = 0;_p < _l ;_p++){
		if(teststr[_p] == ' '){
			_last_wb = _p;
		}
		if(
		_p - _last_stop > width
		|| isvalueinarray(_p, pointers, ray_pointer)
		){
			if(isvalueinarray(_p, pointers, ray_pointer)){
				_last_stop = _p;
			
			}else{
				_last_stop = _last_wb;
			}
			ss_string* s = malloc(sizeof(ss_string));
			 s->string = substring(teststr + _last_start, (_last_stop - _last_start));
			 s->length = strlen(s->string);
			 segs->segments[segs->count] = s;
			 segs->count++;

			if(isvalueinarray(_p, pointers, ray_pointer)){
				ss_string* sp = malloc(sizeof(ss_string));
				sp->string = "";
				sp->length = 0;
				 segs->segments[segs->count] = sp;
				 segs->count++;


			}
			if(isvalueinarray(_p, pointers, ray_pointer)){
				_last_start = _last_stop + 5;
			}else{
				_last_start = _last_stop + 1;
			}

		}
	}
        ss_string* sl = malloc(sizeof(ss_string));
        sl->string = substring(teststr + _last_start, (strlen(teststr) - _last_start));
	 sl->length = strlen(sl->string);
	 segs->segments[segs->count] = sl;
	 segs->count++;
	 return segs;

}

#define MAX_SOURCE_SIZE (0x100000)

char * searchReplace(char * string,
		     char *toReplace[],
		     char *replacements[],
		     int numReplacements)
{
	int i = 0;
	char *locOfToRep;
	char *toRep;
	char *rep;
	int lenToRep, lenStr, lenAfterLocRep;
	static char buffer[MAX_SOURCE_SIZE];

	for (i = 0; i < numReplacements; ++i) {
		toRep = toReplace[i];
		rep = replacements[i];
		//if str not in the string, exit.
		if (!(locOfToRep = strstr(string, toRep))) {
			exit(EXIT_FAILURE);
		}
		while(1){
			if (!(locOfToRep = strstr(string, toRep))) {
				break;
			}
			lenToRep = strlen(toRep);
			lenStr = strlen(string);
			lenAfterLocRep = strlen(locOfToRep);
			sprintf(buffer, "%.*s%s%s",
				lenStr - lenAfterLocRep,
				string, rep, locOfToRep + lenToRep);
			string = buffer;
		}
	}
	return buffer;
}
