#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include "cell.h"
#include "node.h"


using namespace std;

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
        len = 0;
    }

    Item(int val, Item *prev)
    {
        this->val = val;
        up = this;
        down = this;
        len = 0;

        // setting links
        left = prev;
        right = prev->right;
        prev->right->left = this;
        prev->right = this;
    }
    
    void print()
    {
        cout << "Item "<< val << " len: "<< len <<endl;
    }

    Item *left, *right;
    int val, len;
    // len is number of paths through the item and not number of nodes
};

#endif
