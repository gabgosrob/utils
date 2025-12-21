CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinc

compress.exe: src/main.c src/compression.c src/binary_tree.c src/bit_state.c src/priority_queue.c src/stack.c src/bit_writer.c src/bit_reader.c
	$(CC) $(CFLAGS) src/main.c src/compression.c src/binary_tree.c src/bit_state.c src/priority_queue.c src/stack.c src/bit_writer.c src/bit_reader.c -o compress.exe

clean:
	del compress.exe
