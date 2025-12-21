#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include "binary_tree.h"
#include "priority_queue.h"
#include "stack.h"
#include "bit_state.h"
#include "bit_writer.h"
#include "bit_reader.h"

#define SYMBOL_COUNT (UCHAR_MAX + 1)

void compress(char *filepath)
{
    clock_t t = clock();

    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        printf("Error opening file.");
        exit(-1);
    }

    unsigned long frequencies[SYMBOL_COUNT] = {0};
    uint64_t original_byte_count = 0;
    int c = fgetc(file);
    while (c != EOF)
    {
        frequencies[c]++;
        original_byte_count++;
        c = fgetc(file);
    }

    // build array of binary tree nodes
    BtNode *frequency_nodes[SYMBOL_COUNT] = {NULL};
    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        frequency_nodes[i] = bt_create_node(i, frequencies[i]);
    }

    // build priority queue from binary tree nodes
    PqNode *pq_root = NULL;
    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        // only create node if there are those chars in the file
        if (frequencies[i] > 0)
        {
            PqNode *node = pq_create_node(frequencies[i], frequency_nodes[i]);
            pq_insert_min(&pq_root, node);
        }
    }

    // build huffman coding tree
    while (pq_root && pq_root->next)
    {
        BtNode *small_node = pq_pop_min(&pq_root);
        BtNode *big_node = pq_pop_min(&pq_root);

        unsigned long frequency_sum = small_node->value + big_node->value;
        // char doesnt matter because only leaf nodes
        // care about their char
        BtNode *new_parent = bt_create_node(0, frequency_sum);
        new_parent->left = small_node;
        new_parent->right = big_node;

        PqNode *pq_parent_node = pq_create_node(new_parent->value, new_parent);
        pq_insert_min(&pq_root, pq_parent_node);
    }
    BtNode *huffman_root = pq_root->item;
    free(pq_root);

    // create char to bitmap array by doing a DFS
    // on the huffman tree
    BitMap char_to_bitmap[SYMBOL_COUNT] = {0};
    StackNode *stack_root = NULL;
    BitState *state = bs_create_bit_state(huffman_root);
    stack_push(&stack_root, stack_create_node(state));
    while (stack_root)
    {
        BitState *state = stack_pop(&stack_root);
        BtNode *node = state->item;

        if (bt_is_leaf(node))
        {
            // in case first node is a leaf (only 1 symbol in tree)
            if (state->bits.len == 0)
            {
                state->bits.len = 1;
            }
            char_to_bitmap[node->character] = state->bits;
        }
        else
        {
            BitState *right_bitstate = bs_create_bit_state(node->right);
            // add a '1' bit
            right_bitstate->bits.bits = (state->bits.bits << 1) | 1;
            right_bitstate->bits.len = state->bits.len + 1;

            BitState *left_bitstate = bs_create_bit_state(node->left);
            // add a '0' bit
            left_bitstate->bits.bits = state->bits.bits << 1;
            left_bitstate->bits.len = state->bits.len + 1;

            stack_push(&stack_root, stack_create_node(right_bitstate));
            stack_push(&stack_root, stack_create_node(left_bitstate));
        }
        free(state);
    }
    bt_free_tree(huffman_root);

    char compressed_file_name[512];
    snprintf(compressed_file_name, sizeof(compressed_file_name), "%s.smol", filepath);
    FILE *compressed_file = fopen(compressed_file_name, "wb");
    if (!compressed_file)
    {
        printf("Error creating compressed file.");
        exit(-1);
    }

    // write header to compressed file
    // header contains byte count, number of symbols, then blocks of
    // char+len+bits for bitmap
    fwrite(&original_byte_count, sizeof(original_byte_count), 1, compressed_file);
    uint16_t number_of_symbols = 0;
    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        if (char_to_bitmap[i].len > 0)
        {
            number_of_symbols++;
        }
    }
    fwrite(&number_of_symbols, sizeof(number_of_symbols), 1, compressed_file);
    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        if (char_to_bitmap[i].len > 0)
        {
            unsigned char curr_char = i;
            uint8_t len = char_to_bitmap[i].len;
            uint64_t bits = char_to_bitmap[i].bits;
            fwrite(&curr_char, sizeof(curr_char), 1, compressed_file);
            fwrite(&len, sizeof(len), 1, compressed_file);
            fwrite(&bits, sizeof(bits), 1, compressed_file);
        }
    }

    // write compressed data
    BitWriter *bit_writer = bw_create_bit_writer(compressed_file);
    rewind(file);
    c = fgetc(file);
    while (c != EOF)
    {
        bw_write_bits(bit_writer, char_to_bitmap[c]);
        c = fgetc(file);
    }
    bw_flush_buffer(bit_writer);
    uint64_t compressed_byte_count = bit_writer->written_bytes;
    free(bit_writer);

    // print out stats
    t = clock() - t;
    double time_taken_s = ((double)t) / CLOCKS_PER_SEC;
    printf("Compression complete in %.3fs.\n", time_taken_s);
    printf("Space saved: %.2f%%\n", (1.0 - (double)compressed_byte_count / (double)original_byte_count) * 100.0);

    fclose(file);
    fclose(compressed_file);
}

void uncompress(char *filepath)
{
    clock_t t = clock();

    // TODO:check if file is a .smol file
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        printf("Error opening file.");
        exit(-1);
    }

    // read header
    BitMap char_to_bitmap[SYMBOL_COUNT] = {0};
    uint64_t original_byte_count;
    fread(&original_byte_count, sizeof(original_byte_count), 1, file);
    uint16_t number_of_symbols;
    fread(&number_of_symbols, sizeof(number_of_symbols), 1, file);
    for (uint16_t i = 0; i < number_of_symbols; i++)
    {
        unsigned char curr_char;
        uint8_t len;
        uint64_t bits;
        fread(&curr_char, sizeof(curr_char), 1, file);
        fread(&len, sizeof(len), 1, file);
        fread(&bits, sizeof(bits), 1, file);
        char_to_bitmap[curr_char].len = len;
        char_to_bitmap[curr_char].bits = bits;
    }

    // rebuild huffman tree
    BtNode *huffman_root = bt_create_node(0, 0);
    for (int c = 0; c < SYMBOL_COUNT; c++)
    {
        BtNode *curr_node = huffman_root;
        BitMap bitmap = char_to_bitmap[c];
        // only process chars that are in the file
        if (bitmap.len > 0)
        {
            // iterate through bits in MSB order
            for (int i = bitmap.len - 1; i >= 0; i--)
            {
                // get MSB
                int bit = (bitmap.bits >> i) & 1;

                if (bit == 0)
                {
                    if (!curr_node->left)
                    {
                        BtNode *new_node = bt_create_node(0, 0);
                        curr_node->left = new_node;
                    }
                    curr_node = curr_node->left;
                }
                else if (bit == 1)
                {
                    if (!curr_node->right)
                    {
                        BtNode *new_node = bt_create_node(0, 0);
                        curr_node->right = new_node;
                    }
                    curr_node = curr_node->right;
                }
            }
            curr_node->character = c;
        }
    }

    char uncompressed_file_name[512];
    int new_file_len = (int)(strlen(filepath) - 5);
    snprintf(uncompressed_file_name, sizeof(uncompressed_file_name), "%.*s", new_file_len, filepath);
    FILE *uncompressed_file = fopen(uncompressed_file_name, "wb");
    if (!uncompressed_file)
    {
        printf("Error creating uncompressed file");
        exit(-1);
    }

    // uncompress by walking down huffman tree for every char
    BitReader *bit_reader = br_create_bit_reader(file);
    uint64_t written_bytes = 0;
    BtNode *curr_node = huffman_root;
    while (written_bytes < original_byte_count)
    {
        uint8_t bit = br_read_bit(bit_reader);
        if (bit == 0)
        {
            curr_node = curr_node->left;
        }
        else if (bit == 1)
        {
            curr_node = curr_node->right;
        }

        if (bt_is_leaf(curr_node))
        {

            fwrite(&curr_node->character, sizeof(curr_node->character), 1, uncompressed_file);
            written_bytes++;
            curr_node = huffman_root;
        }
    }
    bt_free_tree(huffman_root);
    free(bit_reader);

    // print out stats
    t = clock() - t;
    double time_taken_s = ((double)t) / CLOCKS_PER_SEC;
    printf("Uncompression complete in %.3fs.\n", time_taken_s);

    fclose(uncompressed_file);
    fclose(file);
}