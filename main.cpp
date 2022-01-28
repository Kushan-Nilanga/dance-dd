#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <deque>
#include <vector>
#include <list>
#include <tuple>
#include <sstream>
#include <chrono>
#include <cassert>

#include "src/dance_dd.h"

using namespace std;
using namespace std::chrono;

deque<int> text_to_array(string text)
{
    deque<int> set;
    stringstream text_stream(text);
    string item;

    while (getline(text_stream, item, ' '))
        set.push_back(stoi(item));

    return set;
};

// vector<string> data_files{
//     "./data/cycle/exact_cover/grafo8720.100.ec",
//     "./data/cycle/exact_cover/grafo8564.100.ec",
//     "./data/cycle/exact_cover/UsSignal.ec",
//     "./data/cycle/exact_cover/grafo7785.100.ec",
//     "./data/cycle/exact_cover/grafo7797.100.ec",
//     "./data/cycle/exact_cover/grafo8513.100.ec",
//     "./data/cycle/exact_cover/Deltacom.ec",
//     "./data/cycle/exact_cover/eil51.ec",
//     "./data/cycle/exact_cover/Ion.ec",
//     "./data/cycle/exact_cover/UsCarrier.ec",
//     "./data/cycle/exact_cover/grafo8373.100.ec",
//     "./data/cycle/exact_cover/grafo8674.100.ec",
//     "./data/cycle/exact_cover/att48.ec",
//     "./data/cycle/exact_cover/grafo8224.100.ec",
//     "./data/cycle/exact_cover/Interoute.ec",
//     "./data/cycle/exact_cover/Missouri.ec",
//     "./data/cycle/exact_cover/grafo8549.100.ec",
//     "./data/cycle/exact_cover/grafo8510.100.ec",
//     "./data/partition/exact_cover/VisionNet.ec",
//     "./data/partition/exact_cover/grafo121.20.ec",
//     "./data/partition/exact_cover/grafo298.20.ec",
//     "./data/partition/exact_cover/Funet.ec",
//     "./data/partition/exact_cover/grid44.ec",
//     "./data/partition/exact_cover/grafo160.30.ec",
//     "./data/partition/exact_cover/burma14.ec",
//     "./data/partition/exact_cover/grafo186.30.ec",
//     "./data/partition/exact_cover/grafo165.30.ec",
//     "./data/partition/exact_cover/grafo318.20.ec",
//     "./data/partition/exact_cover/ulysses16.ec",
//     "./data/partition/exact_cover/grafo244.20.ec",
//     "./data/partition/exact_cover/grafo190.20.ec",
//     "./data/partition/exact_cover/grafo291.20.ec",
//     "./data/partition/exact_cover/grafo189.30.ec",
//     "./data/partition/exact_cover/grafo251.20.ec",
//     "./data/partition/exact_cover/grafo115.30.ec",
//     "./data/partition/exact_cover/grafo176.30.ec",
//     "./data/partition/exact_cover/grafo117.20.ec",
//     "./data/partition/exact_cover/grafo172.30.ec",
//     "./data/partition/exact_cover/grafo196.30.ec",
//     "./data/partition/exact_cover/Darkstrand.ec",
//     "./data/partition/exact_cover/grafo135.30.ec",
//     "./data/partition/exact_cover/grafo215.20.ec",
//     "./data/partition/exact_cover/grafo270.20.ec",
//     "./data/partition/exact_cover/grafo171.30.ec"};

vector<string> data_file{"/data/cycle/exact_cover/simple.ec"};
string location = "/Users/donkushanathalage/Desktop/d3x-c++/d3x";

void test(DanceDD *dd, deque<deque<int>> sets);

// Main function
int main()
{
    ifstream file(location + "/data/cycle/exact_cover/simple.ec");
    string text;

    // all sets in the
    deque<deque<int>> sets;

    // reading datafile line by line
    while (getline(file, text))
        sets.push_back(text_to_array(text));

    file.close();

    // Building Dance DD
    DanceDD *dd = new DanceDD(sets);
    Item *i = dd->placeholder->right->right->right->right;
    cout << "Covering " << i->val << endl;
    dd->cover(i);
    dd->print();
    return 0;
}

void test(DanceDD *dd, deque<deque<int>> sets)
{
    // test 1 - item links - right
    int _count = 1;
    auto p = dd->placeholder->right;
    while (p != dd->placeholder)
    {
        assert(p->val == _count);
        _count++;
        p = p->right;
    }

    // test 2 - item links - left
    p = dd->placeholder->left;
    _count = p->val;
    while (p != dd->placeholder)
    {
        assert(p->val == _count);
        _count--;
        p = p->left;
    }

    // test 3 - len fields
    auto items = sets.front();
    sets.pop_front();
    int lens[items.size()] = {};
    for (auto set : sets)
    {
        for (auto item : set)
            lens[item - 1]++;
    }

    p = dd->placeholder->right;
    while (p != dd->placeholder)
    {
        assert(lens[p->val - 1] == p->len);
        p = p->right;
    }
}
