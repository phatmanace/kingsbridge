INC=-I/usr/local/include -I/opt/local/include
LIB=-L/usr/lib -L/usr/local/lib
COPTS=-Wall
ALL_LIBS=-lyajl -lcurl  -lzlog  -lncurses -lcjson -lm
CFLAGS=-D xDEBUG

include Makefile.details
