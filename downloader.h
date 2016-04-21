#include <pthread.h>
#include "queue.h"
#include "comment_tree.h"
#include "queue.h"
#include "string_utils.h"
#include "curl/curl.h"
#include <stdlib.h>
typedef void (*msgcall)(char*, ...);

struct string {
	char *ptr;
	size_t len;
};

struct thread_args {
	QH* queue;
	int* count;
	int threadid;
	int tcount;
	ND** noderay;
	int last_pushed_elem;
	msgcall callback;
	pthread_mutex_t lock;
	int error_code;
};

void downloadURL(pthread_mutex_t lock, struct thread_args* args, unsigned int self, CURL* curl);
void init_string(struct string *s);
