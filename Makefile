CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: all

all: server

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean veryclean:
	$(RM) server