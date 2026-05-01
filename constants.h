#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <utility>
#include <numeric>
#include <random>
#include <chrono>

#define ROW_TARGET_WORD 0
#define ROW_DEFINITION 1
#define ROW_WORD_INPUT 3
#define ROW_MORSE_CODE 4
#define ROW_ERROR 5

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::pair;

vector<pair<string,string>> easy_list;
vector<pair<string,string>> medium_list;
vector<pair<string,string>> hard_list;

typedef struct Word {
    string word, difficulty, definition;
    double score;
} word;

// for verification
const map<char, std::string> MORSE = {
    {'e', "."}, {'t', "-"},
    {'i', ".."}, {'a', ".-"}, {'n', "-."}, {'m', "--"},
    {'s', "..."}, {'u', "..-"}, {'r', ".-."}, {'w', ".--"}, {'d', "-.."}, {'k', "-.-"}, {'g', "--."}, {'o', "---"},
    {'h', "...."}, {'v', "...-"}, {'f', "..-."}, {'l', ".-.."}, {'p', ".--."}, {'j', ".---"},
    {'b', "-..."}, {'x', "-..-"}, {'c', "-.-."}, {'y', "-.--"}, {'z', "--.."}, {'q', "--.-"}
};

// for categorization 
const map<char, double> DEPTH = {
    {'e',1},{'t',1},
    {'a',2},{'i',2},{'n',2},{'m',2},
    {'s',3},{'u',3},{'r',3},{'w',3},{'d',3},{'k',3},{'g',3},{'o',3},
    {'h',4},{'v',4},{'f',4},{'l',4},{'p',4},{'j',4},
    {'b',4},{'x',4},{'c',4},{'y',4},{'z',4},{'q',4}
};

// for colors
const map<char, pair<int,int>> TREE_POS = {
    {'h',{7,0}},  {'s',{7,2}},  {'i',{7,4}},  {'e',{7,6}},
    {'t',{7,10}}, {'m',{7,12}}, {'o',{7,14}},
    {'v',{9,2}},  {'u',{9,4}},
    {'g',{9,12}}, {'q',{9,14}},
    {'f',{11,4}}, {'z',{11,12}},
    {'l',{12,2}}, {'r',{12,4}}, {'a',{12,6}},
    {'n',{12,10}},{'k',{12,12}},{'y',{12,14}},
    {'c',{14,12}},
    {'p',{15,4}}, {'w',{15,6}},
    {'d',{15,10}},{'x',{15,12}},
    {'j',{17,6}},
    {'b',{17,10}}
};

typedef struct player {
    int words_completed;
    int cur_streak;
    int longest_streak;
    int errors;
} Player;

Player player;