#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "string.h"
#include "string_utils.h"
#include <ctype.h>
#include <stdarg.h>


#define OUT_HYL   0
#define IN_HYL    1
void _debug(const char* format, ...) { 
	#ifdef _SUDEBUG
	    va_list args;
	    va_start (args, format);
	      vfprintf (stdout, format, args);
		va_end (args);
#endif
}

char* extract_links(char *msgin, listolinks *links, int* linkcount)
{
	if (msgin == NULL) {
		return NULL;
	}
	int l = strlen(msgin);
	char msg[l + 1];
	memset(msg, l + 1, 0);
	strcpy(msg, msgin);
	char* msgout = calloc(l + 1, sizeof(char));
	char** hyperlinks = malloc(10 * sizeof(char*));


	int _hy_point = 0;
	int _hy_num = 0;
	for (_hy_point = 0; _hy_point < 10; _hy_point++) {
		hyperlinks[_hy_point] = calloc(l + 1, sizeof(char));
	}
	_hy_point = 0;
	_hy_num   = 0;

	int i, _in, _h_ad_start, _fwd_project;
	i = 0;
	_in = 0;
	_h_ad_start  = 0;
	_fwd_project = 0;
	char *p = msg;
	char* loc = strstr(msgin, "a href");
	printf("Located hyperlink at %s\n", loc);
	int state = OUT_HYL;

	while (i < (l - 3)) {
		char* hyp = malloc(sizeof(char) * 9);
		memset(hyp, 0, 9);
		strncpy(hyp, p, 9);
		if (strcmp(hyp, "<a href=\"") == 0) {
			state = IN_HYL;
			_h_ad_start = i + 9;
			//printf("Hyper was %s (i=%d), Add_start=%d, c=%c\n", hyp, i, _h_ad_start, (p+9)[0]);
			//printf("%s\n", p);
			_fwd_project = i + 9;
		}
		if (state == IN_HYL && i >= _fwd_project && p[0] == '"') {
			_fwd_project = -1;
		}
		if (state == IN_HYL &&  _fwd_project == -1 && p[0] == '>') {
			_hy_num++;
			_hy_point = 0;
			_fwd_project = i + 1;
		}
		memset(hyp, 0, 9);
		strncpy(hyp, p, 4);
		//printf("Sample at %d is %s\n", i, hyp);
		if (strcmp(hyp, "</a>") == 0) {
			//printf("Found end of hyperlink at %d\n", i);
			state = OUT_HYL;
			_hy_num++;
			_hy_point = 0;
			_fwd_project = i + 4;
		}
		if (state == IN_HYL && i >= _fwd_project && _fwd_project != -1) {
			//printf("stuffint into HL/[%d] @ %d, %c\n", _hy_num, i, p[0]);
			hyperlinks[_hy_num][_hy_point] = *p;
			hyperlinks[_hy_num][_hy_point + 1] = '\0';
			_hy_point++;
		}
		if (state == OUT_HYL && i >= _fwd_project) {
			//printf("Appending @ %d, %c\n", i, p[0]);
			msgout[_in] = *p;
			msgout[_in + 1] = '\0';
			_in++;
		}
		i++;
		p++;
		free(hyp);
	}
	while (_in < l) {
		msgout[_in] = *p;
		_in++;
		i++;
		msgout[_in] = '\0';
		p++;
	}
	#ifdef _S_DEBUG
	printf("------------------------------------------------------\n");
	printf("Hy: %s\n", hyperlinks[0]);
	printf("Hy: %s\n", hyperlinks[1]);
	printf("Hy: %s\n", hyperlinks[2]);
	printf("Hy: %s\n", hyperlinks[3]);
	printf("------------------------------------------------------\n");
	#endif 
	*linkcount = _hy_num;

	links->links = hyperlinks;

	return msgout;
}
char* dedup(char *msgin)
{
	if (msgin == NULL) {
		return NULL;
	}
	int l = strlen(msgin);
	char msg[l + 1];
	memset(msg, l + 1, 0);
	strcpy(msg, msgin);
	char* msgout = malloc(l + 1);
	int i, _in;
	i = 0;
	_in = 0;

	char *p = msg;
	int sk = 0;
	while (i < (l - 3)) {
		if (p[0] == '&' && p[1] == '#') {
			p[2] = '%';
			sk = 3;
		}

		if (sk > 0) {
			sk--;
		}
		i++;
		if (sk == 0) {
			//printf("Stuffing %c in %d & %d ", p[0], _in, _in + 1);
			msgout[_in] = *p;
			msgout[_in + 1] = '\0';
			_in++;
			//printf("%d/%c %s --> %s\n",_in, p[0],  p, msgout);
		}
		p++;
	}
	while (_in < l) {
		msgout[_in] = *p;
		_in++;
		i++;
		msgout[_in] = '\0';
		p++;
	}

	return msgout;

}


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
	memset(ret, 0, (sizeof(char*) * _len) + 1);
	strncpy(ret, instr, _len);
	ret[_len + 1] = '\0';
	return ret;
}

void freeSegs(s_segments* segs)
{

	int i = 0;

	for (i = 0; i < segs->count; i++) {
		_debug("Freeing segemeent %d\n", i);
		if(strlen(segs->segments[i]->string) > 0){
			free(segs->segments[i]->string);
		}
		free(segs->segments[i]);
	}

	_debug("Freeing Debug text\n");
	free(segs->debugText);
	_debug("Freeing Segments header\n");
	free(segs->segments);

	free(segs);

}
s_segments* _blankSeg()
{
	printf("Called into a blank segement...\n");
	s_segments* segs = malloc(sizeof(s_segments));
	ss_string* sp = malloc(sizeof(ss_string));

	sp->string = "";
	sp->length = 0;
	segs->segments = malloc(sizeof(ss_string*));
	segs->segments[0] = sp;
	segs->count = 0;
	segs->debugText = malloc(sizeof(char) * 21);
	memset(segs->debugText, 0, 20);
	strcpy(segs->debugText, "Blank Returned");
	return segs;

}
s_segments* splitIntoSegments(char *instr, int width)
{
	if (instr == NULL) {
		return _blankSeg();
	}
	_debug("split(): Attempting to malloc %zu bytes\n", strlen(instr));;
	char *targetstring = malloc(4000);
	if(targetstring == NULL){
		printf("Abort!!!!");
	}
	_debug("Malloced %p ok\n", targetstring);

	memset(targetstring, 0, strlen(instr) + 1);
	targetstring[0] = '\0';
	int ok_strings = 0;

	if (instr != NULL) {
		const char space[2] = " ";
		char* token;
		char scanstring[strlen(instr) + 1];
		memset(scanstring, 0, strlen(instr) + 1);
		strcpy(scanstring, instr);
		/* get the first token */
		token = strtok(scanstring, space);

		/* walk through other tokens */
		while ( token != NULL ) {
			if (strlen(token) > width) {
				_debug( "Too Wide...  %s, len=%zu, width=%d\n", token, strlen(token), width );
			}else{
				ok_strings++;
				_debug("concat(): %s\n", token);
				strcat(targetstring, space);
				strcat(targetstring, token);
			}
			token = strtok(NULL, space);
		}
	}
	_debug("StrLen(): is %zu\n", strlen(targetstring));
	_debug("Segemtize(): Found %d strings to move \n", ok_strings);
	if (ok_strings == 0) {
		return _blankSeg();
	}
	s_segments* segs = malloc(sizeof(s_segments));
	segs->count = 0;
	segs->segments = NULL;
	segs->debugText = malloc(sizeof(char) * 20);
	strcpy(segs->debugText, "All_is_well");
	char* from[1] = { "&#x27;" };
	char* to[1]   = { "'" };

	char* teststr = NULL;
	char* endString = NULL;
	if (strstr(targetstring, from[0]) == NULL) {
		endString = malloc(strlen(targetstring) + 1);
		strcpy(endString, targetstring);
		_debug("Skipping search replace\n");
		//printf("Didn't Replace: %s\n", teststr);
	}else{
		//printf("Replaced: %s\n", teststr);
		_debug("Segmentize(): Replaced end string via\n");
		endString = searchReplace(targetstring, from, to, 1);
	}

	// hard coded to 50 strings - this needs sorting...
	segs->segments = malloc(sizeof(ss_string) * 50);


	int _p = 0;
	int _l = strlen(endString);
	_debug("Segemtize(): Chopping up strings, length is %d \n", _l);
	_debug("Segmentize(): EndString: %s\n", endString);

	int* pointers = malloc(sizeof(int) * 200);
	int ray_pointer = 0;

	/*
	 * Find all line breaks, denoted by <p>
	 */
	for (_p = 0; _p < _l - 3; _p++) {
		char otherString[] = "000";
		memcpy(otherString, endString + _p, 3);
		if (strcmp(otherString, "<p>") == 0) {
			pointers[ray_pointer++] = _p;
		}
	}

	_debug("Segemtize(): Found all line breaks, count was %d \n", ray_pointer);

	int _last_wb    = -1;
	int _last_start =  0;
	int _last_stop  =  0;
	for (_p = 0; _p < _l; _p++) {
		if (endString[_p] == ' ') {
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
			s->string = substring(endString + _last_start, (_last_stop - _last_start));
			s->length = strlen(s->string);
			_debug("Segmentize(): Stuffing %p(%s) into position %d\n", s, s->string, segs->count);
			segs->segments[segs->count] = s;
			segs->count++;

			if (isvalueinarray(_p, pointers, ray_pointer)) {
				ss_string* sp = malloc(sizeof(ss_string));
				sp->string = "";
				sp->length = 0;
				_debug("Segementize(): Stuffing %p into position %d\n", sp, segs->count);
				segs->segments[segs->count] = sp;
				segs->count++;


			}
			if (isvalueinarray(_p, pointers, ray_pointer)) {
				_last_start = _last_stop + 3;
			}else{
				_last_start = _last_stop + 1;
			}

		}
	}
	_debug("Segementize(): Almost done with segmentization\n");

	ss_string* sl = malloc(sizeof(ss_string));
	sl->string = substring(endString + _last_start, (strlen(endString) - _last_start));
	sl->length = strlen(sl->string);
	segs->segments[segs->count] = sl;
	segs->count++;
	//free(targetstring);
	free(pointers);
	_debug("Segementize(): done with segmentization\n");
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
	char *tmpString = malloc(strlen(string) + 1);
	memset(tmpString, 0, strlen(string) + 1);
	strncpy(tmpString, string, strlen(string));
	//static char buffer[MAX_SOURCE_SIZE];
	char* buffer = malloc(MAX_SOURCE_SIZE);
	memset(buffer, 0, MAX_SOURCE_SIZE);

	for (i = 0; i < numReplacements; ++i) {
		toRep = toReplace[i];
		rep = replacements[i];
		//if str not in the string, exit.
		if (!(locOfToRep = strstr(tmpString, toRep))) {

			printf("Couldn't find |%s| in |%s|\n", toRep, tmpString);
			exit(EXIT_FAILURE);
		}
		while (1) {
			if (!(locOfToRep = strstr(tmpString, toRep))) {
				break;
			}
			lenToRep = strlen(toRep);
			lenStr = strlen(tmpString);
			lenAfterLocRep = strlen(locOfToRep);
			sprintf(buffer, "%.*s%s%s",
				lenStr - lenAfterLocRep,
				tmpString, rep, locOfToRep + lenToRep);
			tmpString = buffer;
		}
	}
	free(tmpString);
	return buffer;
}

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code)
{
	static char hex[] = "0123456789abcdef";

	return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str)
{
	char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;

	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
			*pbuf++ = *pstr;
		else if (*pstr == ' ')
			*pbuf++ = '+';
		else
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
		if (*pstr == ';') {
			pstr++;
		}
	}
	*pbuf = '\0';
	return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(char *str)
{
	if (str == NULL) {
		return NULL;
	}
	char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;

	while (*pstr) {
		if (*pstr == '%') {
			if (pstr[1] && pstr[2]) {
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} else if (*pstr == '+') {
			*pbuf++ = ' ';
		} else {
			*pbuf++ = *pstr;
		}
		pstr++;
		if (*pstr == ';') {
			pstr++;
		}
	}
	*pbuf = '\0';
	return buf;
}
