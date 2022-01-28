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

    void get_descendants(deque<Node *> *destination, Node *t1)
    {
        if (hi != nullptr && hi != t1)
        {
            destination->push_back(hi);
            hi->get_descendants(destination, t1);
        }

        if (lo != nullptr && lo != t1)
        {
            destination->push_back(lo);
            lo->get_descendants(destination, t1);
        }
    }

    void get_ancestors(deque<tuple<Node *, PATH, int>> *destination)
    {
        Parent *p = head->right;
        while (p != tail)
        {
            destination->push_back(make_tuple(p->node, p->path, p->path == HI ? p->node->hlen : p->node->llen));
            p->node->get_ancestors(destination);
            p = p->right;
        }
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
            cout << item->val << " ";
        }

        string h = hi == nullptr ? "0" : hi->item == nullptr ? "T"
                                                             : to_string(hi->item->val);

        string l = lo == nullptr ? "0" : lo->item == nullptr ? "T"
                                                             : to_string(lo->item->val);

        cout << "[\033[32mhlen: " << hlen << ", \033[31mllen: " << llen << ", \033[33mplen: " << plen;
        cout << "\033[0m][\033[30m\033[42mHI " << h;
        cout << ",\033[41m LO " << l << "\033[0m]";

        cout << "[";

        Parent *q = head->right;
        while (q != tail)
        {
            string path = (q->path == HI) ? "HI" : "LO";
            cout << "(" << q->node->item->val << " " << path << ")";
            q = q->right;
        }

        cout << "]" << endl;
    }

    Item *item;
    Node *lo, *hi;
    int plen = 0, llen = 0, hlen = 0, parent_len = 0;
    Parent *head, *tail;
};

#endif
