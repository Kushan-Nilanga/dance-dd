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

void print_sols(vector<vector<deque<Node *>>> *sols)
{
    for (auto sol : *sols)
    {
        cout << "| ";
        for (auto j : sol)
        {
            for (auto k : j)
            {
                cout << k->item->val << " ";
            }
            cout << "| ";
        }
        cout << endl;
    }
}

vector<string> data_file{"/data/cycle/exact_cover/Ion.ec"};
string location = "/Users/donkushanathalage/Desktop/d3x-c++/d3x";

// Main function
int main()
{
    ifstream file(location + "/example.ec");
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

    vector<deque<Node *>> R;
    auto *sols = new vector<vector<deque<Node *>>>;
    dd->search(R, sols);

    cout << "----------- SOLUTIONS ------------\n";
    print_sols(sols);   
    return 0;
}