
struct article{
	char *link;
	char *text;
	char *storyhash;
	char *other;
	int flags;
	int init;
	struct article *next;
	struct article *previous;
};

typedef enum {RESULT_OK, RESULT_FAILED} result;

struct FetchResult{
    struct article* articles;
    result curl_result;
};
