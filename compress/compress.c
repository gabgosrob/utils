#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include "binary_tree.h"

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

    BtNode *frequency_nodes[SYMBOL_COUNT] = {NULL};
    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        frequency_nodes[i] = bt_create_node(i, frequencies[i]);
    }

    for (int i = 0; i < SYMBOL_COUNT; i++)
    {
        printf("%d - ", i);
        printf("%c: %lu\n", frequency_nodes[i]->character, frequency_nodes[i]->value);
    }

    fclose(file);
}