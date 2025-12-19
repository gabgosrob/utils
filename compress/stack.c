#include <stdlib.h>
#include "stack.h"

StackNode *stack_create_node(void *item)
{
    StackNode *node = malloc(sizeof(StackNode));
    if (!node)
    {
        return NULL;
    }

    node->item = item;
    node->next = NULL;

    return node;
}

void stack_push(StackNode **root, StackNode *node)
{
    StackNode *root_node = *root;

    node->next = root_node;
    *root = node;
}

void *stack_pop(StackNode **root)
{
    StackNode *root_node = *root;

    // null stack
    if (!root_node)
    {
        return NULL;
    }

    void *to_return = root_node->item;
    *root = root_node->next;
    free(root_node);
    return to_return;
}