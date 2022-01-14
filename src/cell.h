#ifndef CELL_H
#define CELL_H

// Parent class of Node and Item
// This is used because we need to point to cell and node
// at the same time
class Cell
{
public:
    Cell *up, *down;
};

#endif