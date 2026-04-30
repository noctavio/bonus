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

const map<char, std::string> MORSE = {
    {'e', "."}, {'t', "-"},
    {'i', ".."}, {'a', ".-"}, {'n', "-."}, {'m', "--"},
    {'s', "..."}, {'u', "..-"}, {'r', ".-."}, {'w', ".--"}, {'d', "-.."}, {'k', "-.-"}, {'g', "--."}, {'o', "---"},
    {'h', "...."}, {'v', "...-"}, {'f', "..-."}, {'l', ".-.."}, {'p', ".--."}, {'j', ".---"},
    {'b', "-..."}, {'x', "-..-"}, {'c', "-.-."}, {'y', "-.--"}, {'z', "--.."}, {'q', "--.-"}
};

const map<char, double> DEPTH = {
    {'e',1},{'t',1},
    {'a',2},{'i',2},{'n',2},{'m',2},
    {'s',3},{'u',3},{'r',3},{'w',3},{'d',3},{'k',3},{'g',3},{'o',3},
    {'h',4},{'v',4},{'f',4},{'l',4},{'p',4},{'j',4},
    {'b',4},{'x',4},{'c',4},{'y',4},{'z',4},{'q',4}
};

typedef struct player {
    int words_completed;
    int cur_streak;
    int longest_streak;
    int errors;
} Player;

Player player;