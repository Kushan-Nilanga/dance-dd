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

#define LOG

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
        if (curr->hi != nullptr && curr->hi != t1)
            curr->hi->add_parent(curr, HI);
        if (curr->lo != nullptr && curr->lo != t1)
            curr->lo->add_parent(curr, LO);

        curr->llen = curr->lo != nullptr ? curr->lo->llen + curr->lo->hlen : 0;
        curr->hlen = curr->hi != nullptr ? curr->hi->llen + curr->hi->hlen : 0;

        if (curr->hi != nullptr)
        {
            curr->hi->plen++;
            curr->hlen = curr->hi->llen + curr->hi->hlen;
        }

        if (curr->lo != nullptr)
        {
            curr->lo->plen++;
            curr->llen = curr->lo->llen + curr->lo->hlen;
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

                for (auto a = curr->head->right; a != curr->tail; a = a->right)
                    match->add_parent(a->node, a->path);

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
    vector<Node *> merged;

    DanceDD(deque<deque<int>> sets)
    {
        // placeholder item
        placeholder = new Item();

        // setting item headers
        Item *prev = placeholder;
        for (auto val : sets.front())
        {
            prev = new Item(val, prev);
        }
        sets.pop_front();

        // terminal-1 node
        t1 = new Node();
        t1->hlen = 1;

        // building the zdd
        root = _build(placeholder->right, &sets);
        root->plen = 1;

        // updating the lens
        Item *i = placeholder->right;
        while (i != placeholder)
        {

            Node *j = (Node *)i->down;
            while (j != (Node *)i)
            {
                j->item->len += j->plen * j->hlen;
                j = (Node *)j->down;
            }
            i = i->right;
        }
    }

    tuple<
        deque<tuple<Node *, PATH>> *,
        vector<Node *> *,
        vector<Node *> *,
        deque<Node *> *>
    cover(Item *i)
    {
#ifdef LOG
        cout << "covering " << i->val << endl;
#endif

        i->left->right = i->right;
        i->right->left = i->left;

        auto *ancestors = new deque<tuple<Node *, PATH>>;
        auto *descendants = new deque<Node *>;
        auto *C = new vector<Node *>;
        auto *H = new vector<Node *>;
        Node *p = (Node *)i->down;
        while (p != (Node *)i)
        {
            // getting ancestors
            p->get_ancestors(ancestors);

            // getting descendants
            if (p->hi != nullptr && p->hi != t1)
            {
                descendants->push_back(p->hi);
                p->hi->get_descendants(descendants, t1);
            }

            // collecting nodes whose hi nodes should be hidden;
            C->push_back(p);
            p = (Node *)p->down;
        }

        // covering ancestors
        for (auto [p, t] : *ancestors)
        {
            int l = p->hlen;

            // update plen hlen assuming all the hi of node is removed
            if (t == HI)
            {
                p->hlen = p->hi != nullptr ? p->hi->hlen + p->hi->llen : 0;
                if (p->hi != nullptr && p->hi->item == i)
                    p->hlen = p->hi->llen;
            }
            else
            {
                p->llen = p->lo != nullptr ? p->lo->hlen + p->lo->llen : 0;
                if (p->lo != nullptr && p->lo->item == i)
                    p->llen = p->lo->llen;
            }

            p->item->len = p->item->len - p->plen * (l - p->hlen);

            if (l > 0 && p->hlen == 0)
            {
                H->push_back(p);
            }
        }

        for (auto p : *H)
        {
            p->up->down = p->down;
            p->down->up = p->up;

            auto a = p->head->right;
            while (a != p->tail)
            {
                if (p->lo != nullptr && p->lo != t1)
                    p->lo->add_parent(a->node, a->path);
                if (a->path == HI)
                    a->node->hi = p->lo;
                else
                    a->node->lo = p->lo;
                a = a->right;
            }

            p->hi->remove_parent(p, HI);
            p->lo->remove_parent(p, LO);
        }

        // covering descendants
        for (auto p : *descendants)
        {
            int l = p->plen;
            p->plen -= 1;

            // len(i) ← len(i) − (l − plen(p)) · hlen(p)
            p->item->len = p->item->len - (l - p->plen) * p->hlen;

            if (p->plen == 0)
            {
                p->up->down = p->down;
                p->down->up = p->up;
                p->hi->remove_parent(p, HI);
                p->lo->remove_parent(p, LO);
            }
        }

        // removing C nodes and pointing parents to lo and hi nodes and addding parents to them
        for (auto p : *C)
        {
            auto a = p->head->right;
            while (a != p->tail)
            {
                if (p->lo != nullptr && p->lo != t1)
                    p->lo->add_parent(a->node, a->path);

                if (a->path == LO)
                    a->node->lo = p->lo;
                else
                    a->node->hi = p->lo;
                a = a->right;
            }
        }

        return make_tuple(ancestors, H, C, descendants);
    }

    void uncover(
        Item *i,
        deque<tuple<Node *, PATH>> *ancestors,
        vector<Node *> *H,
        vector<Node *> *C,
        deque<Node *> *descendants)
    {
#ifdef LOG
        cout << "uncovering " << i->val << endl;
#endif

        i->left->right = i;
        i->right->left = i;

        for (auto p : *C)
        {
            if (p->hi != nullptr && p->hi != t1)
                p->hi->add_parent(p, HI);
            if (p->lo != nullptr && p->lo != t1)
                p->lo->add_parent(p, LO);

            auto a = p->head->right;
            while (a != p->tail)
            {
                p->lo->remove_parent(a->node, a->path);
                if (a->path == LO)
                    a->node->lo = p;
                else
                    a->node->hi = p;
                a = a->right;
            }
        }

        // uncover ancestors
        for (auto p : *H)
        {
            p->up->down = p;
            p->down->up = p;

            auto a = p->head->right;
            while (a != p->tail)
            {
                p->lo->remove_parent(a->node, a->path);
                if (a->path == LO)
                    a->node->lo = p;
                else
                    a->node->hi = p;

                if (p->hi != nullptr && p->hi != t1)
                    p->hi->add_parent(p, HI);
                if (p->lo != nullptr && p->lo != t1)
                    p->lo->add_parent(p, LO);
                a = a->right;
            }
        }

        for (auto [p, t] : *ancestors)
        {
            int l = p->hlen;

            if (t == HI)
                p->hlen = p->hi->hlen + p->hi->llen;
            else
                p->llen = p->lo->hlen + p->lo->llen;

            p->item->len = p->item->len + p->plen * (p->hlen - l);
        }

        // uncover descendants
        for (auto p : *descendants)
        {
            int l = p->plen;
            p->plen += 1;
            p->item->len = p->item->len + (p->plen - l) * p->hlen;

            if (l == 0 && p->plen > 0)
            {
                p->up->down = p;
                p->down->up = p;

                if (p->hi != nullptr && p->hi != t1)
                    p->hi->add_parent(p, HI);
                if (p->lo != nullptr && p->lo != t1)
                    p->lo->add_parent(p, LO);
            }
        }
    }

    void print_options(vector<deque<Node *>> R)
    {
        cout << " | ";
        for (auto j : R)
        {
            for (auto k : j)
            {
                cout << k->item->val << " ";
            }
            cout << "| ";
        }
        cout << endl;
    }

    void print_option(deque<Node *> R)
    {
        cout << " | ";
        for (auto k : R)
        {
            cout << k->item->val << " ";
        }
        cout << "| ";
        cout << endl;
    }

    void search(vector<deque<Node *>> R, vector<vector<deque<Node *>>> *sols)
    {
        if (placeholder->right == placeholder)
        {
            cout << "SOLUTION FOUND ";
            print_options(R);
            sols->push_back(R);
            return;
        }

        Item *i = placeholder->right;
        // Item *_i = placeholder->right;

        // if (i == placeholder)
        //     return;

        // while (_i != placeholder)
        // {
        //     if (i->len > _i->len)
        //         i = _i;
        //     _i = _i->right;
        // }

        auto [ans1, h1, c1, des1] = cover(i);

        auto p = (Node *)i->down;
        while (p != (Node *)i)
        {
            auto ops = new vector<deque<Node *>>;
            p->options(ops, t1);
#ifdef LOG
            cout << "options with (" << p->item->val << ") ";
            print_options(*ops);
#endif

            for (auto x : *ops)
            {
                deque<tuple<Item *,
                            deque<tuple<Node *, PATH>> *,
                            vector<Node *> *,
                            vector<Node *> *,
                            deque<Node *> *>>
                    X;

#ifdef LOG
                cout << "pushed to solution";
                print_option(x);
#endif
                R.push_back(x);

                for (auto p_ : x)
                {
                    auto [ans, h, c, des] = cover(p_->item);
                    X.push_front(make_tuple(p_->item, ans, h, c, des));
                }

                search(R, sols);

                for (auto [i, ans, h, c, des] : X)
                    uncover(i, ans, h, c, des);

                R.pop_back();

#ifdef LOG
                cout << "removed from solution";
                print_option(x);
#endif
            }
            p = (Node *)p->down;
        }
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
            }

            p = p->right;
            cout << endl;
        }
    }
};

#endif
