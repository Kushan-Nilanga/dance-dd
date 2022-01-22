#ifndef DANCE_DD_H
#define DANCE_DD_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <chrono>
#include <stack>

#include "node.h"
#include "path.h"
#include "parent.h"
#include "item.h"
#include "cell.h"

using namespace std;

// ZDD datastructure incorporating dancing links
class DanceDD
{
private:
    // checking if 2 graphs are isometric
    bool _isosubgraph(Node *curr_node, Node *subject_node)
    {
        if (curr_node == nullptr && subject_node == nullptr)
            return true;

        if (curr_node == t1 && subject_node == t1)
            return true;

        if (curr_node == nullptr ^ subject_node == nullptr)
            return false;

        if (curr_node == t1 ^ subject_node == t1)
            return false;

        if (curr_node->item != subject_node->item)
            return false;

        return _isosubgraph(curr_node->hi, subject_node->hi) && _isosubgraph(curr_node->lo, subject_node->lo);
    }

    Node *_build(Item *item, deque<deque<int>> *rows)
    {
        if (rows->size() == 0)
            return nullptr;

        if (item == placeholder)
            return t1;

        deque<deque<int>> hi_list, lo_list;
        for (int i = 0; i < rows->size(); i++)
        {
            int val = item->val;
            deque<int> row = (*rows)[i];
            
            if (row.size() > 0 && val == row.front())
            {
                row.pop_front();
                hi_list.push_back(row);
            }
            else
                lo_list.push_back(row);
        }

        Node *curr_hi = _build(item->right, &hi_list);
        Node *curr_lo = _build(item->right, &lo_list);
        

        // Rule 1 - reduction
        if (curr_hi == nullptr)
            return curr_lo;

        Node *curr = new Node(item);
        curr->hi = curr_hi;
        curr->lo = curr_lo;
        curr->hi->add_parent(curr, HI);
        if (curr->lo != nullptr)
            curr->lo->add_parent(curr, LO);
        
        curr->llen = curr->lo != nullptr ? curr->lo->llen + curr->lo->hlen: 0;
        curr->hlen = curr->hi != nullptr ? curr->hi->llen + curr->hi->hlen : 0;
        
        curr->hlen = 0;
        if(curr->hi != nullptr)
        {
            curr->hi->plen++;
            curr->hlen =curr->hi->llen + curr->hi->hlen;
            if(curr->hi != t1)
                curr->hi->item->len = curr->hi->plen * (curr->hi->hlen + curr->hi->llen);
        }
        
        curr->llen = 0;
        if(curr->lo != nullptr)
        {
            curr->lo->plen++;
            curr->llen = curr->lo->llen + curr->lo->hlen;
            if(curr->lo != t1)
                curr->lo->item->len = curr->lo->plen * (curr->lo->hlen + curr->lo->llen);
        }

        // Rule 2 - merge
        Cell *p = item->down;
        while (p != item)
        {
            if (curr != p && _isosubgraph(curr, (Node *)p))
            {
                // remove newly created node
                curr->up->down = curr->down;
                curr->down->up = curr->up;
                auto match = (Node *)p;
                match->plen += curr->plen;
                match->extend_parents(curr->head, curr->tail, curr->parent_len);
                curr = match;
                                
                merged.push_back(curr);
                break;
            }
            p = p->down;
        }

        return curr;
    }

public:
    Item *placeholder;
    Node *t1, *root;
    vector<Node*> merged;

    DanceDD(deque<int> items)
    {
        // placeholder item
        placeholder = new Item();

        // setting item headers
        Item *prev = placeholder;
        for (auto val : items)
        {
            prev = new Item(val, prev);
        }

        // terminal-1 node
        t1 = new Node();
        t1->hlen = 1;
    }

    void build(deque<deque<int>> sets)
    {
        // building the zdd
        root = _build(placeholder->right, &sets);
        root->plen = 1;
        root->item->len = root->plen * (root->hlen + root->llen);
    }

    void get_ancestors(vector<tuple<Node *, PATH, int>> *vec, Node *node)
    {
        if (node == nullptr || node == t1)
            return;

        Parent *i = node->head->right;
        if (i == node->tail)
            return;

        while (i != node->tail)
        {
            if (i->node != nullptr && i->node != t1)
                vec->push_back(make_tuple(i->node, i->path, i->path == HI ? i->node->hlen : i->node->llen ));
            i = i->right;
        }

        i = node->head->right;
        while (i != node->tail)
        {
            if (i->node != nullptr && i->node != t1)
                get_ancestors(vec, i->node);
            i = i->right;
        }
    }

    void cover_upper(vector<Node *> C)
    {
        vector<tuple<Node *, PATH, int>> V;
        for (auto i : C)
            get_ancestors(&V, i);

        vector<Node *> H;
        for (auto [p, t, n] : V)
        {
            int l = p->hlen;

            // Update hlen(p) and llen(p) assuming all high edges of q ∈ C were removed
            if (t == HI)
                p->hlen -= 1;
            else
                p->llen -= 1;

            Item *i = p->item;
            int i_len = i->len - p->plen * (l - p->hlen);
            
            printf("(%d)i->len = %d - %d * (%d - %d) = %d\n", i->val, i->len, p->plen, l, p->hlen, i_len);
            i->len = i_len;

            if (l > 0 && p->hlen == 0)
                H.push_back(p);
        }

        for (auto p : H)
        {
            p->up->down = p->down;
            p->down->up = p->up;

            Parent *s = p->head->right;
            while (s != p->tail)
            {
                if (p->lo != nullptr && p->hi != t1)
                {
                    p->lo->add_parent(s->node, s->path);
                    if (s->path == LO)
                        s->node->lo = p->lo;
                    else
                        s->node->hi = p->lo;
                }
                s=s->right;
            }

            if (p->hi != nullptr && p->hi != t1)
                p->hi->remove_parent(p, HI);

            if (p->lo != nullptr && p->hi != t1)
                p->lo->remove_parent(p, LO);
        }
    }
    
    void get_descendants(deque<Node *> *descendants, Node *node)
    {
        if (node == nullptr || node == t1)
            return;
        
        get_descendants(descendants, node->lo);
        get_descendants(descendants, node->hi);

        if (node->hi != nullptr && node->hi != t1)
            descendants->push_front(node->hi);

        if (node->lo != nullptr && node->lo != t1)
            descendants->push_front(node->lo);
        
    }


    void cover_lower(vector<Node *> C)
    {
        deque<Node *> V;
        for (auto i : C)
        {
            V.push_front(i->hi);
            get_descendants(&V, i->hi);
        }

        for (auto p : V)
        {
            int l = p->plen;

            // Update plen(p) assuming all high edges of a € C were deleted.
            p->plen -= 1;
            
            Item *i = p->item;
            int i_len = i->len - (l - p->plen) * p->hlen;
            printf("(%d) i->len = %d - (%d - %d) * %d = %d\n", i->val, i->len, l, p->plen, p->hlen, i_len);
            i->len = i_len;
            
            if (p->plen == 0)
            {
                p->up->down = p->down;
                p->down->up = p->up;

                if (p->hi != nullptr && p->hi != t1)
                    p->hi->remove_parent(p, HI);

                if (p->lo != nullptr && p->hi != t1)
                    p->lo->remove_parent(p, LO);
            }
        }
    }

    void cover(Item *i)
    {
        i->left->right = i->right;
        i->right->left = i->left;

        vector<Node *> C;
        Node *p = (Node *)i->down;
        while (p != (Node *)i)
        {
            C.push_back(p);
            p = (Node *)p->down;
        }

        cover_upper(C);
        cover_lower(C);

        for (auto p : C)
        {
            Parent *a = p->head->right;
            while (a != p->tail)
            {
                if (p->lo != nullptr && p->hi != t1)
                {
                    p->lo->add_parent(a->node, a->path);

                    if (a->path == LO)
                        a->node->lo = p->lo;
                    else
                        a->node->hi = p->lo;
                }
                a = a->right;
            }

            if (p->hi != nullptr && p->hi != t1)
                p->hi->remove_parent(p, HI);

            if (p->lo != nullptr && p->hi != t1)
                p->lo->remove_parent(p, LO);
        }
    }

    void uncover_upper(vector<Node *> C)
    {
        // TODO
    }

    void uncover_lower(vector<Node *> C)
    {
        // TODO
    }

    void uncover(Item *i)
    {
        i->left->right = i;
        i->right->left = i;

        vector<Node *> C;
        Node *p = (Node *)i->down;
        while (p != (Node *)i)
        {
            C.push_back(p);
            p = (Node *)p->down;
        }

        for (auto p : C)
        {
            p->hi->add_parent(p, HI);
            p->lo->add_parent(p, LO);

            Parent *a = p->head->left;
            while (a != p->tail)
            {
                p->lo->remove_parent(a->node, a->path);
                if (a->path == LO)
                    a->node->lo = p;
                else
                    a->node->hi = p;
            }
        }

        uncover_lower(C);
        uncover_upper(C);
    }
    
    void search(vector<vector<int>> R){}

    int size()
    {
        int size = 0;
        Item *p = placeholder->right;
        while (p != placeholder)
        {
            size += p->len;
            p = p->right;
        }
        return size;
    }

    void print()
    {
        Item *p = placeholder->right;
        
        while (p != placeholder)
        {
            p->print();
            Node *n = (Node *)p->down;
            while (n != (Node *)p)
            {
                n->print();
                n = (Node *)n->down;
                cout << endl;
            }

            p = p->right;
        }
        cout << "T1 " << t1 << endl;
//        t1->print();
    }
};

#endif
