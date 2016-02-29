OS := $(shell uname)
LIBS1 = "ACE"
ifeq ($(OS),Darwin)
	LIBS1="OSX"
else
	LIBS1="Linux"
endif

INC=-I/usr/local/include -I/opt/local/include
LIB=-L/usr/local/lib
default: all

all: 
	echo ${LIBS1}
	${CC} -g -o string_utils.o  -c string_utils.c  -Wall 
	${CC} -g -o bin/ll_sort_test    ll_sort_test.c  -Wall
	${CC} -g -o comment_tree.o  -c comment_tree.c  -Wall -lzlog
	${CC} -g -o t_sample.o      -c t_sample.c      -Wall
	${CC} -g -o bin/comment_test    comment_tester.c  t_sample.o comment_tree.o   -Wall -lzlog
	${CC} -g -o bin/curses_demo     curses_sampler.c     -lncurses  -Wall -lzlog
	${CC} -g -o bin/curses_tree     crs_tree_test.c  t_sample.o comment_tree.o ${INC} ${LIB}  -lzlog  -lncurses  -Wall
	#${CC} -g -o hnfetcher.o     -c hnfetcher.c      -lcurl -lconfig -lyajl -Wall
	#${CC} -g -o grabber        grabber.c hnfetcher.o -lcurl -lconfig -lyajl -lncurses -lzlog -lpthread -Wall
	#${CC} -g -o demo           cmoka.c hnfetcher.o   -lcurl -lconfig -lyajl -lncurses -lzlog -lpthread -Wall


clean:
	rm -f *.o a.out grabber demo proxy sha1_example comment_test curses_demo
	rm -f *.o bin/a.out bin/grabber bin/demo bin/proxy bin/sha1_example bin/comment_test bin/curses_demo
	
.PHONY: all clean test
