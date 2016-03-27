#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "stdbool.h"
#include "string_utils.h"

typedef struct _ss_string{
	char* string;
	int length;
}ss_string;

typedef struct _s_segment{
	int count;
	ss_string** segments;
	
}s_segments;

bool isvalueinarray(int val, int *arr, int size){
	    int i;
	        for (i=0; i < size; i++) {
			if (arr[i] == val)
					    return true;
	    }
	    return false;
}

void _log(const char* msg)
{
	printf("Pattern() %s\n", msg);
}

int main()
{
	_log("Starting");

	char* teststr = "I&#x27;m disappointed by the overall tone here. It&#x27;s as if this were some kind of failure that everybody is piling up on.<p>- She made three passes to fix the existing code base.<p>- After that didn&#x27;t work, with the CTO&#x27;s involvement, this Go solution was devised.<p>- It took only 2 weeks to ramp up with Go and build a replacement that solved the problem.<p>- The number of instances needed dropped from 4 to 2<p>This sounds like a resounding success.<p>So congratulations Alexandra and Digg. And thank you for sharing. The article was well written and I learned some things about how the Node.js event system works under the hood.";

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
			printf("%d/-> %s\n", ray_pointer, otherString);			
		}else{			
			printf(" %s\n", otherString);			
		}
	}

	for(_p = 0;_p < ray_pointer;_p++){
		printf("Found at position %d\n", pointers[_p]);
	}

	
	int _last_wb    = -1;
	int _last_start =  0;
	int _last_stop  =  0;
	int max_width = 70;
	for(_p = 0;_p < _l ;_p++){
		if(teststr[_p] == ' '){
			_last_wb = _p;
		}
		if(
		_p - _last_stop > max_width
		|| isvalueinarray(_p, pointers, ray_pointer)
		){
			if(isvalueinarray(_p, pointers, ray_pointer)){
				_last_stop = _p;
			
			}else{
				_last_stop = _last_wb;
			}
			printf("%s\n", substring(teststr + _last_start, (_last_stop - _last_start)));
			if(isvalueinarray(_p, pointers, ray_pointer)){
				printf("\n");
			}
			if(isvalueinarray(_p, pointers, ray_pointer)){
				_last_start = _last_stop + 5;
			}else{
				_last_start = _last_stop + 1;
			}

		}
	}
	printf("%s\n", substring(teststr + _last_start, (strlen(teststr) - _last_start)));



	return 0;
}
