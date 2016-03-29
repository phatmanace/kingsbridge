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

bool isvalueinarray(int val, int *arr, int size)
{
	int i;

	for (i = 0; i < size; i++) {
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
	int _len = len;
	if (strlen(instr) < len) {
		_len = strlen(instr);
	}
	char *ret = malloc((sizeof(char*) * _len) + 1);
	strncpy(ret, instr, _len);
	ret[_len + 1] = '\0';
	return ret;
}

void freeSegs(s_segments* segs)
{

	int i = 0;

	for (i = 0; i < segs->count; i++) {
		if (strlen(segs->segments[i]->string) > 0) {
			free(segs->segments[i]->string);
		}
		free(segs->segments[i]);
	}

	free(segs->segments);

	free(segs);

}
s_segments* _blankSeg(){
	s_segments* segs = malloc(sizeof(segs));
	ss_string* sp = malloc(sizeof(ss_string));
	sp->string = "";
	sp->length = 0;
	segs->segments = malloc(sizeof(ss_string));
	segs->segments[0] = sp;
	segs->count = 0;
	segs->debugText = malloc(sizeof(char) * 20);
	strcpy(segs->debugText, "Blank Returned");
	return segs;

}
s_segments* splitIntoSegments(char *instr, int width)
{
	if (instr == NULL) {
		return _blankSeg();
	}
	char *targetstring = malloc(strlen(instr) + 1);

	memset(targetstring, 0, strlen(instr) + 1);
	int ok_strings = 0;

	if (instr != NULL) {
		const char s[2] = " ";
		char* token;
		char scanstring[strlen(instr) + 1];
		memset(scanstring, 0, strlen(instr) + 1);
		strcpy(scanstring, instr);
		/* get the first token */
		token = strtok(scanstring, s);

		/* walk through other tokens */
		while ( token != NULL ) {
			if (strlen(token) > width) {
				printf( "Too Wide...  %s, len=%zu, width=%d\n", token, strlen(token), width );
			}else{
				ok_strings++;
				targetstring = strcat(targetstring, s);
				targetstring = strcat(targetstring, token);
			}
			token = strtok(NULL, s);
		}
	}
	if (ok_strings == 0) {
		return _blankSeg();
	}
	s_segments* segs = malloc(sizeof(segs));
	char* from[1] = { "&#x27;" };
	char* to[1]   = { "'" };

	char* teststr = NULL;
	if (strstr(targetstring, from[0]) == NULL) {
		teststr = malloc(strlen(targetstring) + 1);
		strcpy(teststr, targetstring);
		//printf("Didn't Replace: %s\n", teststr);
	}else{
		//printf("Replaced: %s\n", teststr);
		teststr = searchReplace(targetstring, from, to, 1);
	}

	// hard coded to 50 strings - this needs sorting...
	segs->segments = malloc(sizeof(ss_string) * 50);


	int _p = 0;
	int _l = strlen(teststr);

	int* pointers = malloc(sizeof(int) * 200);
	int ray_size = 1;
	int ray_pointer = 0;


	for (_p = 0; _p < _l - 3; _p++) {
		char otherString[] = "000";
		memcpy(otherString, teststr + _p, 3);
		if (strcmp(otherString, "<p>") == 0) {
			pointers[ray_pointer++] = _p;
		}
	}


	int _last_wb    = -1;
	int _last_start =  0;
	int _last_stop  =  0;
	for (_p = 0; _p < _l; _p++) {
		if (teststr[_p] == ' ') {
			_last_wb = _p;
		}
		if (
			_p - _last_stop > width
			|| isvalueinarray(_p, pointers, ray_pointer)
			) {
			if (isvalueinarray(_p, pointers, ray_pointer)) {
				_last_stop = _p;

			}else{
				_last_stop = _last_wb;
			}
			ss_string* s = malloc(sizeof(ss_string));
			s->string = substring(teststr + _last_start, (_last_stop - _last_start));
			s->length = strlen(s->string);
			segs->segments[segs->count] = s;
			segs->count++;

			if (isvalueinarray(_p, pointers, ray_pointer)) {
				ss_string* sp = malloc(sizeof(ss_string));
				sp->string = "";
				sp->length = 0;
				segs->segments[segs->count] = sp;
				segs->count++;


			}
			if (isvalueinarray(_p, pointers, ray_pointer)) {
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

			printf("BOOM!BOOM!\n\n");
			printf("Couldn't find |%s| in |%s|\n", toRep, string);
			exit(EXIT_FAILURE);
		}
		while (1) {
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
