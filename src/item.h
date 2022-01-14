#ifndef ITEM_H
#define ITEM_H

#include "cell.h"

// Header of the nodes
class Item : public Cell
{
public:
    Item()
    {
        right = this;
        left = this;
        up = this;
        down = this;
    }

    Item(int val, Item *prev)
    {
        this->val = val;
        up = this;
        down = this;

        // setting links
        left = prev;
        right = prev->right;
        prev->right->left = this;
        prev->right = this;
    }

    Item *left, *right;
    int val, len = 0;
};

#endif