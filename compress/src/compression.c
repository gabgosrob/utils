#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include "binary_tree.h"
#include "priority_queue.h"
#include "stack.h"
#include "bit_state.h"
#include "bit_writer.h"

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
    // TODO: write original file byte size (used in decoding to know when to stop)
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
    uint64_t compressed_byte_count = bit_writer->written_bites;
    free(bit_writer);

    // print out stats
    t = clock() - t;
    double time_taken_s = ((double)t) / CLOCKS_PER_SEC;
    printf("Compression complete in %.3fs.\n", time_taken_s);
    printf("Space saved: %.2f%%\n", (1.0 - (double)compressed_byte_count / (double)original_byte_count) * 100.0);

    fclose(file);
    fclose(compressed_file);
    // TODO: Move to a compression.c file and import here instead
}

void uncompress(char *filepath)
{
    // TODO: implement decoding

    // check if file is a .smol file

    // TODO: Move to a compression.c file and import here instead
}