#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "binary_tree.h"
#include "priority_queue.h"

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
        // create parent with some char
        // doesnt matter because only leaf
        // nodes care about their char
        BtNode *new_parent = bt_create_node(0, frequency_sum);
        new_parent->left = small_node;
        new_parent->right = big_node;

        PqNode *pq_parent_node = pq_create_node(new_parent->value, new_parent);
        pq_insert_min(&pq_root, pq_parent_node);
    }
    BtNode *huffman_root = pq_root->item;
    free(pq_root);

    // here, DFS through the binary while keeping track of L/R (0/1)
    // to assign a binary value to each character

    fclose(file);
}