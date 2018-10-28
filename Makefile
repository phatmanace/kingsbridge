INC=-I/usr/local/include -I/opt/local/include 
LIB=-L/usr/lib -L/usr/local/lib
COPTS=-Wno-unused-command-line-argument -g -O0
ALL_LIBS=-lyajl -lcurl  -lzlog  -lncurses -lcjson -lm
CFLAGS= -D_GNU_SOURCE ${COPTS}

include Makefile.details
