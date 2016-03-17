INC=-I/usr/local/include -I/opt/local/include
LIB=-L/usr/local/lib
COPTS=-Wall
ALL_LIBS=-lyajl -lcurl  -lzlog  -lncurses

all: queue.o string_utils.o ll_sort_test comment_fetch.o bin/comment_test bin/comment_demo bin/curses_tree bin/ll_test_queue

string_utils.o: string_utils.c
	${CC}  -g -o string_utils.o  -c string_utils.c ${COPTS}
t_sample.o: t_sample.c
	${CC}  -g -o t_sample.o  -c t_sample.c ${COPTS}
queue.o: queue.c
	${CC}  -g -o queue.o  -c queue.c ${COPTS}
comment_fetch.o: comment_fetch.c
	${CC}  -g -o comment_fetch.o  -c comment_fetch.c ${COPTS} -Wno-nonnull
bin/comment_test: comment_tester.c t_sample.o comment_tree.o
	${CC} -g -o bin/comment_test    comment_tester.c  t_sample.o comment_tree.o  ${INC} ${LIB} ${COPTS} -lzlog
bin/ll_test_queue: l_test_queue.c queue.o
	${CC} -pthread -g -o bin/l_test_queue    l_test_queue.c  queue.o  ${INC} ${LIB} ${COPTS} 
bin/comment_demo: curses_sampler.c 
	${CC} -g -o bin/comment_demo   curses_sampler.c    ${INC} ${LIB} ${COPTS} -lzlog -lncurses
bin/curses_tree: crs_tree_test.c t_sample.o comment_fetch.o  comment_tree.o 
	${CC} -g -o bin/curses_tree   crs_tree_test.c t_sample.o comment_fetch.o  comment_tree.o \
			  ${INC} ${LIB} ${ALL_LIBS} ${COPTS}
ll_sort_test: ll_sort_test.c
	${CC}  -g -o bin/ll_sort_test  -c ll_sort_test.c ${COPTS}
clean:
	\rm *.o bin/comment_test bin/curses_tree bin/ll_sort_test
