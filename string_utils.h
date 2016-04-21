
#ifndef STRING_UTILS_H
#define STRING_UTILS_H
typedef struct _ss_string {
	char* string;
	int length;
}ss_string;

typedef struct _s_segment {
	int count;
	ss_string** segments;
	char* debugText;

}s_segments;

typedef struct _listolinks{
	char** links;
}listolinks;

typedef struct _hlink {
	ss_string link_address;
	ss_string link_text;
}hlink;
bool isvalueinarray(int val, int *arr, int size);
char* makeRepeatedString(char repeatingChar, int count);
char* substring(const char* instr, int len);
char * searchReplace(char * string, char *toReplace[], char *replacements[], int numReplacements);
s_segments* splitIntoSegments(char *instr, int width);
void freeSegs(s_segments* segs);
char *url_decode(char *str);
char *url_encode(char *str);
char* dedup(char *msgin);
char* extract_links(char *msgin, listolinks *hyperlinks, int* linkcount);

#endif

