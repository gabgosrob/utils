CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lws2_32

server.exe: main.c
	$(CC) $(CFLAGS) main.c -o server.exe $(LDFLAGS)

clean:
	del server.exe
