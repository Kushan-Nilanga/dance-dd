#ifndef PARENT_H
#define PARENT_H

#include "path.h"

class Node;

class Parent
{
public:
    Parent() {}
    Parent(Node *node, PATH path) : path(path), node(node) {}
    PATH path;
    Node *node;
    Parent *left, *right;
};

#endif