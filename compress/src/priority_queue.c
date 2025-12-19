#include <stdlib.h>
#include "priority_queue.h"

PqNode *pq_create_node(unsigned long priority, void *item)
{
    PqNode *node = malloc(sizeof(PqNode));
    if (!node)
    {
        return NULL;
    }

    node->priority = priority;
    node->item = item;
    node->next = NULL;

    return node;
}

void pq_insert_min(PqNode **root, PqNode *node)
{
    PqNode *root_node = *root;

    if (!root_node)
    {
        *root = node;
        return;
    }

    PqNode *last_node = NULL;
    PqNode *curr_node = root_node;
    while (1)
    {
        if (node->priority <= curr_node->priority)
        {
            if (!last_node)
            {
                // insert before head (last is null)
                node->next = curr_node;
                *root = node;
                return;
            }
            else
            {
                // insert between last and curr
                last_node->next = node;
                node->next = curr_node;
                return;
            }
        }
        // at the end of the queue
        if (!curr_node->next)
        {
            // insert after
            curr_node->next = node;
            return;
        }

        last_node = curr_node;
        curr_node = curr_node->next;
    }
}

void *pq_pop_min(PqNode **root)
{
    if (!root || !*root)
    {
        return NULL;
    }

    PqNode *root_node = *root;
    void *item = root_node->item;
    *root = root_node->next;
    free(root_node);

    return item;
}