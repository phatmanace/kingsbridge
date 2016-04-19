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
	int link_count = 0;
	listolinks hyperlinks;
	hyperlinks.links = NULL;
	char* tx = extract_links(url_decode(dedup(instr)), &hyperlinks, &link_count);
	s_segments* segs = splitIntoSegments(tx, screen_width);
	int _p = 0;

	for (_p = 0; _p < segs->count; _p++) {
		printf("[%3d] %s\n", _p, segs->segments[_p]->string);
	}
	if (instr != NULL) {
		printf("Whole String length is %3zu\n", strlen(instr));
	}
	printf("Segment Count is %d\n", segs->count);
	printf("Hyperlink count is %d\n", link_count);
	printf("Hyperlink set is %p\n", hyperlinks.links);
	printf("Hyperlink set is %s\n", hyperlinks.links[0]);
	printf("Whole String(): %s\n", tx);
	freeSegs(segs);
}

int main()
{
	_log("Starting");

	//char* instr = "Let&#x27;s suppose you were an expert in optical design for space systems -- lenses, mirrors, coolers, detectors, electronics, thermal design -- and worked at NASA on a space telescope.<p>When that space telescope (say it was Hubble or Spitzer) was launched, you would be looking for another project. That&#x27;s a small world. Maybe you would talk to a friend who works for intelligence agencies and find out what they are looking for.<p>And the reverse happens too. If you&#x27;ve explored the design space for an intelligence agency, you would be well-positioned to re-use that knowledge for a space telescope. You mention the classification barrier, but the basic physics drivers will be more apparent the second time around.<p>Case in point: Larry James (<a href=\"http:&#x2F;&#x2F;www.jpl.nasa.gov&#x2F;about&#x2F;bio_james.php\" rel=\"nofollow\">http:&#x2F;&#x2F;www.jpl.nasa.gov&#x2F;about&#x2F;bio_james.php</a>), who is the Deputy Director for JPL, but who used to be Air Force Deputy Chief of Staff for Intelligence, Surveillance and Reconnaissance at the Pentagon, and who before <i>that</i> was a Shuttle payload specialist. He&#x27;s a technical executive, but there are pure technical examples as well.<p>OK, and a second example, of a segmented mirror demonstrator: <a href=\"http:&#x2F;&#x2F;www.nps.edu&#x2F;About&#x2F;News&#x2F;NPS-New-Home-for-Giant-Segmented-Mirror-Space-Telescope-.html\" rel=\"nofollow\">http:&#x2F;&#x2F;www.nps.edu&#x2F;About&#x2F;News&#x2F;NPS-New-Home-for-Giant-Segment...</a> (&quot;NASA’s James Webb Space Telescope, named for the former NASA administrator and scheduled for launch in 2014 [<i>erp!</i>], will deploy the next generation of SMT technology on a mission to image light from the earliest galaxies in the universe.&quot;)";
	char* instr = "You went to the simple image wikipedia just to find a citation that agrees with you, while the english Wikipedia has a whole artcicle describing it in quite an opposite way: <a href=\"https:&#x2F;&#x2F;en.wikipedia.org&#x2F;wiki&#x2F;Audio_signal\" rel=\"nofollow\">https:&#x2F;&#x2F;en.wikipedia.org&#x2F;wiki&#x2F;Audio_signal</a><p>If it is confusing, is because you do not understand what a &quot;signal&quot; means. A signal is not the phenomenon itself, it is a representation of it.<p>An audio signal is a signal that represents sound, but it is not sound itself. If it were, you would not need to call it a signal, it would be simply &quot;sound&quot;. In a slightly different use of adjective, an acoustic signal is a signal (that may or not be audio, eg, ultrassound) that is carried by sound waves.<p>Actually, it only gets confusing if you try to be pedantic, as GP did. Everyone knows what an audio signal in a EM chart.";
	ParseNPrint(instr);


	return 0;
}
