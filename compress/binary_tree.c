#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

BtNode *bt_create_node(unsigned char character, unsigned long value)
{
    BtNode *node = malloc(sizeof(BtNode));
    if (!node)
    {
        return NULL;
    }

    node->character = character;
    node->value = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void bt_free_tree(BtNode *root)
{
    if (!root)
    {
        return;
    }

    bt_free_tree(root->left);
    bt_free_tree(root->right);
    free(root);
}

int bt_is_leaf(BtNode *node)
{
    return node->left == NULL && node->right == NULL;
}