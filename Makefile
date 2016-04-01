INC=-I/usr/local/include -I/opt/local/include
LIB=-L/usr/lib -L/usr/local/lib
COPTS=-Wall
ALL_LIBS=-lyajl -lcurl  -lzlog  -lncurses -lcjson -lm
CFLAGS=-D xDEBUG
all: comment_tree.o queue.o string_utils.o ll_sort_test comment_fetch.o bin/comment_test bin/comment_demo bin/curses_tree bin/ll_test_queue bin/t_curses_multi_test bin/t_patt_test



comment_tree.o: comment_tree.c string_utils.o
	${CC} ${INC}  -g -o comment_tree.o string_utils.o  -c comment_tree.c ${COPTS}
string_utils.o: string_utils.c
	${CC} ${INC}  -g -o string_utils.o  -c string_utils.c ${COPTS}
t_sample.o: t_sample.c
	${CC} ${INC}  -g -o t_sample.o  -c t_sample.c ${COPTS}
queue.o: queue.c
	${CC} ${INC}  -g -o queue.o  -c queue.c ${COPTS} ${CFLAGS}
comment_fetch.o: comment_fetch.c
	${CC} ${INC} -g -o comment_fetch.o  -c comment_fetch.c ${COPTS} -Wno-nonnull
bin/comment_test: comment_tester.c t_sample.o comment_tree.o string_utils.o
	${CC} ${INC} -g -o bin/comment_test    comment_tester.c  t_sample.o comment_tree.o string_utils.o  ${LIB} ${COPTS} -lzlog
bin/ll_test_queue: l_test_queue.c queue.o
	${CC} -pthread -g -o bin/l_test_queue    l_test_queue.c  queue.o  ${INC} ${LIB} ${COPTS} 
bin/t_curses_multi_test: t_curses_multi_test.c queue.o comment_tree.o string_utils.o
	${CC} -pthread -g -o bin/t_curses_multi_test  t_curses_multi_test.c string_utils.o comment_tree.o  queue.o  ${INC} ${LIB} ${COPTS}  -lcurl -lyajl -Wno-nonnull -lzlog -ljansson -lcjson -lm
bin/comment_demo: curses_sampler.c 
	${CC} -g -o bin/comment_demo   curses_sampler.c    ${INC} ${LIB} ${COPTS} -lzlog -lncurses
bin/curses_tree: crs_tree_test.c t_sample.o comment_fetch.o  comment_tree.o string_utils.o
	${CC} -g -o bin/curses_tree   crs_tree_test.c t_sample.o comment_fetch.o string_utils.o comment_tree.o \
			  ${INC} ${LIB} ${ALL_LIBS} ${COPTS}
ll_sort_test: ll_sort_test.c
	${CC}  -g -o bin/ll_sort_test  -c ll_sort_test.c ${COPTS}
bin/t_patt_test: t_pattern_test.c string_utils.o  
	${CC}  -g -o bin/t_patt_test string_utils.o  t_pattern_test.c ${COPTS}
clean:
	-rm -fr *.o bin/comment_test bin/curses_tree bin/ll_sort_test bin/l_test_queue* bin/*dSYM bin/comment_demo bin/curses_tree* bin/t_patt_test bin/t_curses_multi_test *dSYM*
