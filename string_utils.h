
typedef struct _ss_string{
	char* string;
	int length;
}ss_string;

typedef struct _s_segment{
	int count;
	ss_string** segments;
	
}s_segments;
bool isvalueinarray(int val, int *arr, int size);
char* makeRepeatedString(char repeatingChar, int count);
char* substring(const char* instr, int len);
char * searchReplace(char * string, char *toReplace[], char *replacements[], int numReplacements);
s_segments* splitIntoSegments(char *instr, int width);
