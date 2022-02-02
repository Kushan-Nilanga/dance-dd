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

    void get_ancestors(deque<tuple<Node *, PATH>> *destination)
    {
        Parent *p = head->right;
        while (p != tail)
        {
            destination->push_back(make_tuple(p->node, p->path));
            p->node->get_ancestors(destination);
            p = p->right;
        }
    }

    void add_parent(Node *node, PATH path)
    {
        Parent *a = head->right;
        while (a != tail && a != nullptr)
        {
            if (a->node == node && a->path == path)
                return;
            a = a->right;
        }

        Parent *p = new Parent(node, path);
        p->left = tail->left;
        p->right = tail;
        tail->left->right = p;
        tail->left = p;
        parent_len++;
    }

    void remove_parent(Node *node, PATH path)
    {
        if (this == nullptr)
            return;
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

    void ancestor_options(vector<deque<Node *>> *ops, deque<Node *> op, Parent *p)
    {
        if (p->path == HI)
            op.push_front(p->node);

        auto a = p->node->head->right;
        if (a == p->node->tail)
        {
            ops->push_back(op);
            return;
        }

        while (a != p->node->tail)
        {
            ancestor_options(ops, op, a);
            a = a->right;
        }
    }

    void descendant_options(vector<deque<Node *>> *ops, deque<Node *> op, Node *node, Node *t1)
    {
        if (node == nullptr)
            return;

        if (node == t1)
        {
            ops->push_back(op);
            return;
        }

        descendant_options(ops, op, node->lo, t1);
        op.push_back(node);
        descendant_options(ops, op, node->hi, t1);
    }

    void options(vector<deque<Node *>> *options, Node *t1)
    {
        auto ans_op = new vector<deque<Node *>>;
        auto des_op = new vector<deque<Node *>>;
        deque<Node *> def;

        auto a = this->head->right;
        while (a != this->tail)
        {
            ancestor_options(ans_op, def, a);
            a = a->right;
        }
        def.push_back(this);
        descendant_options(des_op, def, this->hi, t1);

        if (this->head->right == this->tail)
        {
            for (auto de : *des_op)
                options->push_back(de);
            return;
        }

        // combine ancestoral options with descendent options
        bool allow_empty_ancestor = true;
        for (auto an : *ans_op)
        {
            if (an.size() > 0 || allow_empty_ancestor == true)
            {
                for (auto de : *des_op)
                {
                    deque<Node *> v;
                    v.insert(v.begin(), an.begin(), an.end());
                    v.insert(v.end(), de.begin(), de.end());
                    options->push_back(v);
                }
            }

            if (an.size() == 0)
                allow_empty_ancestor = false;
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
        cout << "\033[0m][\033[42m HI " << h;
        cout << " \033[41m LO " << l << " \033[0m]";

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
