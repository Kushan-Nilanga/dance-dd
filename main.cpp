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
    cout << "----------- DIAGRAM ------------\n";
    dd->print();

    vector<std::deque<Node *>> R;
    cout << "----------- SEARCH ------------\n";
    dd->search(R);
    return 0;
}