CC = gcc
CFLAGS = -Wall -Wextra -g

all: server

server: server.c server.h
	$(CC) $(CFLAGS) server.c server.h node.c node.h -lssl -lcrypto -o server

clean:
	rm -f server