CC = gcc
CFLAGS = -Wall
LDFLAGS_SERVER = -lssl -lcrypto

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LDFLAGS_SERVER)

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client
