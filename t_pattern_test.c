#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "stdbool.h"
#include "string_utils.h"



void _log(const char* msg)
{
	printf("Pattern() %s\n", msg);
}

void ParseNPrint(char* instr){
	int screen_width = 70;
	s_segments* segs = splitIntoSegments(instr, screen_width);
	 int _p = 0;

	 for(_p = 0;_p < segs->count;_p++){
		printf("[%3d] %s\n", _p, segs->segments[_p]->string);
	 }
	 if(instr != NULL){
		 printf("Whole String length is %3zu\n" , strlen(instr));
	 }
	 printf("Segment Count is %d\n", segs->count);
	

	 free(segs);
}

int main()
{
	_log("Starting");

	char* instr = "I&#x27;m disappointed by the overall tone here. It&#x27;s as if this were some kind of failure that everybody is piling up on.<p>- She made three passes to fix the existing code base.<p>- After that didn&#x27;t work, with the CTO&#x27;s involvement, this Go solution was devised.<p>- It took only 2 weeks to ramp up with Go and build a replacement that solved the problem.<p>- The number of instances needed dropped from 4 to 2<p>This sounds like a resounding success.<p>So congratulations Alexandra and Digg. And thank you for sharing. The article was well written and I learned some things about how the Node.js event system works under the hood.";
	ParseNPrint(instr);
	_log("pass 1 complete");
	instr = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	_log("pass 2 complete");
	ParseNPrint(instr);
	instr = "xxxxx  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	ParseNPrint(instr);
	_log("pass 3 complete");
	instr = NULL;
	ParseNPrint(instr);
	_log("pass 3 complete");
	char* teststr = "https:&#x2F;&#x2F;en.wikipedia.org&#x2F;wiki&#x2F;";
	{
		char* se[1] = {"&#"};
		char* re[1] = {"X"};
		printf("Decode: of %s is %s\n", teststr, url_decode(searchReplace(teststr, se, re, 1)));
	}

	//char* foo = {0};
	//strcpy(foo, "bar");

	return 0;
}
