INC=-I/usr/local/include -I/opt/local/include 
LIB=-L/usr/lib -L/usr/local/lib
COPTS=-Wno-unused-command-line-argument -ggdb
ALL_LIBS=-lyajl -lcurl  -lzlog  -lncurses -lcjson -lm
CFLAGS= -D xDEBUG -D_GNU_SOURCE ${COPTS}

include Makefile.details
