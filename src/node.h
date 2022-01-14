#ifndef NODE_H
#define NODE_H

#include "parent.h"
#include "cell.h"
#include "item.h"
#include "path.h"


// Actual nodes of the ZDD
class Node : public Cell
{
private:
    void init_parent_list()
    {
        head = new Parent();
        tail = new Parent();

        head->right = tail;
        tail->left = head;
    }

public:
    Node()
    {
        init_parent_list();
    }

    Node(Item *item)
    {
        this->item = item;
        up = item->up;
        down = item;
        item->up->down = this;
        item->up = this;
        item->len++;
        init_parent_list();
    }

    void add_parent(Parent *parent)
    {
        parent->left = tail->left;
        parent->right = tail;
        tail->left->right = parent;
        tail->left = parent;
    }

    void extend_parents(Parent *o_head, Parent *o_tail)
    {
        o_head->right->left = tail->left;
        o_tail->left->right = tail;
        tail->left->right = o_head->right;
        tail->left = o_tail->left;
    }

    void remove_parent(Node *node, PATH path)
    {
        Parent *a = head->right;
        while (a != tail)
        {
            if (a->node == node && a->path == path)
            {
                a->right->left = a->left;
                a->left->right = a->right;
                return;
            }
            a = a->right;
        }
    }

    Item *item;
    Node *lo, *hi;
    int plen = 0, llen = 0, hlen = 0;
    Parent *head, *tail;
};

#endif