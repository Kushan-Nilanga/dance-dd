#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>

#include "parent.h"
#include "cell.h"
#include "item.h"
#include "path.h"

using namespace std;

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
        init_parent_list();
    }

    void add_parent(Node *node, PATH path)
    {
        Parent *p = new Parent(node, path);
        p->left = tail->left;
        p->right = tail;
        tail->left->right = p;
        tail->left = p;
        parent_len++;
    }

    void extend_parents(Parent *o_head, Parent *o_tail, int len)
    {
        o_head->right->left = tail->left;
        o_tail->left->right = tail;
        tail->left->right = o_head->right;
        tail->left = o_tail->left;
        parent_len += len;
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
                parent_len--;
                return;
            }
            a = a->right;
        }
    }

    void print()
    {
        if (item != nullptr)
        {
            cout << item->val << " " << this << endl;
        }
        
        cout << "hlen: " << hlen << ", llen: " << llen << ", plen: " << plen << endl;

//        parents
        Parent *q = head->right;
        if (head->right != tail)
        {
            cout << "parents(" << parent_len << ")";
        }

        while (q != tail)
        {
            string path = (q->path == HI) ? "HI" : "LO";
            cout << "[" << q->node << " " << path << "] ";
            q = q->right;
        }

        if (head->right != tail)
            cout << endl;

        cout << "HI " << hi << endl;
        cout << "LO " << lo << endl;
    }

    Item *item;
    Node *lo, *hi;
    int plen = 0, llen = 0, hlen = 0, parent_len = 0;
    bool __merged = false;
    Parent *head, *tail;
};

#endif
