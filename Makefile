
default: all

all: 
	${CC} -g -o comment_tree.o  -c comment_tree.c       -Wpedantic
	${CC} -g -o comment_test   comment_tester.c comment_tree.o    -Wall
	#${CC} -g -o hnfetcher.o     -c hnfetcher.c      -lcurl -lconfig -lyajl -Wall
	#${CC} -g -o grabber        grabber.c hnfetcher.o -lcurl -lconfig -lyajl -lncurses -lzlog -lpthread -Wall
	#${CC} -g -o demo           cmoka.c hnfetcher.o   -lcurl -lconfig -lyajl -lncurses -lzlog -lpthread -Wall


clean:
	rm *.o a.out grabber demo proxy sha1_example comment_test
	
.PHONY: all clean test
