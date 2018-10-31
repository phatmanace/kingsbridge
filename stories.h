typedef struct Story{
	char* title;
	char* hash;
	struct Story* next;
}Story;

Story* newStory();
void appendStory(Story* head, char*  _newtitle, char* _newhash);
int StorySize(Story* head);
void printStory(Story* head);
void printl(int depth, char* prefix, Story* head);
void MoveStory(Story** destref, Story** sourceRef);
void FrontBackSplit(Story* source, Story** fronthalf, Story** backhalf, int depth);
