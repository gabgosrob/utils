#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef struct PqNode
{
    unsigned long priority;
    void *item;
    struct PqNode *next;
} PqNode;

void pq_insert_min(PqNode **root, PqNode *node);

PqNode *pq_create_node(unsigned long priority, void *item);

void *pq_pop_min(PqNode **root);

#endif