CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinc

com.exe: src/main.c src/binary_tree.c src/bit_state.c src/priority_queue.c src/stack.c
	$(CC) $(CFLAGS) src/main.c src/binary_tree.c src/bit_state.c src/priority_queue.c src/stack.c -o com.exe

clean:
	del com.exe
