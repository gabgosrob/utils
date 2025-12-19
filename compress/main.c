#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include "binary_tree.h"
#include "priority_queue.h"
#include "stack.h"
#include "bit_state.h"

#define SYMBOL_COUNT (UCHAR_MAX + 1)

int main()
{
    // TODO: Change this to argv param input
    char *filepath = "test.txt";

    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        printf("Error opening file.");
        exit(-1);
    }

    unsigned long frequencies[SYMBOL_COUNT] = {0};
    int c = fgetc(file);
    while (c != EOF)
    {
        frequencies[c]++;
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

    fclose(file);
}