#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H

typedef struct StackNode
{
    void *item;
    struct StackNode *next;
} StackNode;

StackNode *stack_create_node(void *item);

void stack_push(StackNode **root, StackNode *node);

void *stack_pop(StackNode **root);

#endif