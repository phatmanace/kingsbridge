#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "stdbool.h"
#include "string_utils.h"
#include <regex.h>



void _log(const char* msg)
{
	printf("Pattern() %s\n", msg);
}

void ParseNPrint(char* instr)
{
	int screen_width = 70;
	s_segments* segs = splitIntoSegments(instr, screen_width);
	int _p = 0;

	for (_p = 0; _p < segs->count; _p++) {
		printf("[%3d] %s\n", _p, segs->segments[_p]->string);
	}
	if (instr != NULL) {
		printf("Whole String length is %3zu\n", strlen(instr));
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
		regex_t preg;
		int rc;
		char *pattern = "\\&\\#x.F";
		size_t nmatch = 2;
		regmatch_t pmatch[2];
		if (0 != (rc = regcomp(&preg, pattern, 0))) {
			printf("regcomp() failed, returning nonzero (%d)\n", rc);
			exit(EXIT_FAILURE);
		}
		printf("RegComp ok...\n");
		if (0 != (rc = regexec(&preg, teststr, nmatch, pmatch, REG_ICASE | REG_EXTENDED))) {
			printf("Failed to match '%s' with '%s',returning %d.\n",
			       teststr, pattern, rc);
		}else{

			printf("Managed to find %zu subexpressions\n", preg.re_nsub);
			regfree(&preg);
			char res[1000];
			memcpy(res, teststr  + pmatch[0].rm_so, (pmatch[0].rm_eo - pmatch[0].rm_so) );
			res[(pmatch[0].rm_eo - pmatch[0].rm_so)] = 0;
			printf("First Match went from %d to %d and was %s\n", pmatch[0].rm_so, pmatch[0].rm_eo, res);

		}
	}
	{
		char* se[1] = { "&#x" };
		char* re[1] = { "%" };
		printf("Decode: of %s is %s\n", teststr, url_decode(searchReplace(teststr, se, re, 1)));
		printf("Raw Decode: of %s is %s\n", teststr, searchReplace(teststr, se, re, 1));
	}

	{
		printf("1. %s\n", dedup("One find day &#x2F I stumbled on a farm"));
		printf("2. %s\n", dedup("One find day &#x2F I &#x2F stumbled &#x2F on a farm"));

		printf("3. %s\n", url_decode(dedup("One find day &#x2F I stumbled on a farm")));
		printf("4. %s\n", url_decode(dedup("One find day &#x2F I &#x2F stumbled &#x2F on a farm")));
	}

	{
		char* msg = "from <a href=\"https:&#x2F;&#x2F;talks.golang.org&#x2F;2015&#x2F;gogo.slide#4\" rel=\"nofollow\">https:&#x2F;&#x2F;talks.golang.org&#x2F;2015&#x2F;gogo.slide#4</a><p><pre><code> &quot;Why move the compiler to Go? Not for validation; we have more pragmatic motives: Go is easier to write (correctly) than C. Go is easier to debug than C (even absent a debugger). Go is the only language you&#x27;d need to know; encourages contributions. Go has better modularity, tooling, testing, profiling, ... Go makes parallel execution trivial. Already seeing benefits, and it&#x27;s early yet. Design document: golang.org&#x2F;s&#x2F;go13compiler&quot; </code></pre> and <a href=\"https:&#x2F;&#x2F;talks.golang.org&#x2F;2015&#x2F;gogo.slide#10\" rel=\"nofollow\">2nd Link</a><p><pre><code> &quot;Why translate it, not write it from scratch? Correctness, testing. Steps: Write a custom translator from C to Go. Run the translator, iterate until success. Measure success by bit-identical output. Clean up the code by hand and by machine. Turn it from C-in-Go to idiomatic Go (still happening).&quot;</code></pre>";

		printf("5. %s\n", url_decode(dedup(msg)));
		char** hyperlinks = NULL;
		int link_count = 0;
		printf("6. Extract: %s\n", extract_links(url_decode(dedup(msg)), hyperlinks, &link_count));
		printf("    link count = %d\n", link_count);
	}

	{
		char* msg = "Tsipras&#x27; response to the leak:<p><a href=\"https:&#x2F;&#x2F;twitter.com&#x2F;wikileaks&#x2F;status&#x2F;716400800499679232\" rel=\"nofollow\">https:&#x2F;&#x2F;twitter.com&#x2F;wikileaks&#x2F;status&#x2F;716400800499679232</a><p>Mostly the Greeks take offense to the IMF, per Bloomberg [1], &quot;considering a plan to cause a credit event in Greece and destabilize Europe.&quot;<p>Also: &quot;Acc to #IMF leak, conf call was held March 19, when Velculescu was still in Athens, Hilton Hotel. Makes you wonder if Hilton is bugged.&quot; [2]<p>[1]<a href=\"http:&#x2F;&#x2F;www.bloomberg.com&#x2F;news&#x2F;articles&#x2F;2016-04-02&#x2F;imf-discussed-pressuring-germany-on-greek-debt-wikileaks-says\" rel=\"nofollow\">http:&#x2F;&#x2F;www.bloomberg.com&#x2F;news&#x2F;articles&#x2F;2016-04-02&#x2F;imf-discus...</a><p>[2] <a href=\"https:&#x2F;&#x2F;twitter.com&#x2F;YanniKouts&#x2F;status&#x2F;716226465910665216\" rel=\"nofollow\">https:&#x2F;&#x2F;twitter.com&#x2F;YanniKouts&#x2F;status&#x2F;716226465910665216</a>";
		printf("15. %s\n\n", url_decode(dedup(msg)));
		char** hyperlinks = NULL;
		int link_count = 0;
		printf("16. Extract: %s\n", extract_links(url_decode(dedup(msg)), hyperlinks, &link_count));
		printf("    link count = %d\n", link_count);
	}
	{

		int link_count = 0;
		char** hyperlinks = NULL;
		printf("7. Null Check: %s\n", extract_links(url_decode(dedup(NULL)), hyperlinks, &link_count));
		printf("    link count = %d\n", link_count);
	}

	//char* foo = {0};
	//strcpy(foo, "bar");

	return 0;
}
