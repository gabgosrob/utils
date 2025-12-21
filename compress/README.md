# compress

Simple compression tool that uses [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding) to losslessly compress any file. Outputs compressed files as .smol files.

## Usage

`compress (-c | -u) <file_path>`

`-c`  
 Compress the specified file.

`-u`  
 Uncompress the specified file (must have been compressed with this tool and therefore have the .smol file type).

## What I learned

- Great review of multiple data structures and algorithms (priority queue, binary tree, stack, dfs/bfs, huffman coding)
- Learned a lot about bit manipulation (MSB vs LSB, reading/writing bits and bytes, bit shifting and bit operators to isolate bits or build a bit buffer)
- Had to learn makefiles since the project was getting annoyingly big to compile with only gcc in cli
- I am now much more comfortable/less nervous with memory management (malloc, free, pointers/pointers to pointers)
- Lots of documentation reading to figure out how to use various functions (mostly from man7)
