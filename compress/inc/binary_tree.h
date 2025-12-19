#ifndef BINARY_TREE_H
#define BINARY_TREE_H

typedef struct BtNode
{
    unsigned long value;
    unsigned char character;
    struct BtNode *left;
    struct BtNode *right;
} BtNode;

BtNode *bt_create_node(unsigned char character, unsigned long value);

void bt_free_tree(BtNode *root);

int bt_is_leaf(BtNode *node);

#endif