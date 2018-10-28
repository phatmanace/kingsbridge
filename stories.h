typedef struct Story{
	char* title;
	char* hash;
	struct Story* next;
}Story;

Story* newStory();
void appendStory(Story* head, char*  _newtitle, char* _newhash);
