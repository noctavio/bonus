#include "constants.h"
#include <unistd.h>
#include <ncurses.h>

void wait_for_key(int key) {
    int ch;
    while ((ch = getch()) != key);
}

void clear_row(int row) {
    move(row, 0);
    clrtoeol();
}

struct MorseNode {
    char letter; // \0 if internal node (no letter at the root of a morse code binary tree)
    MorseNode* dot; // left = Z key
    MorseNode* dash; // right = X key

    MorseNode() : letter('\0'), dot(nullptr), dash(nullptr) {}
};

double scoreWord(const string& word) {
    double s = 0;
    for (char c : word) {
        auto it = DEPTH.find(c);
        s += (it != DEPTH.end()) ? it->second : 3.0;
    }
    return s + word.size() * 0.75;
}

string extractField(const string& obj, const string& key) {
    string search = "\"" + key + "\"";
    size_t kpos = obj.find(search);

    if (kpos == string::npos) return "";
    size_t vstart = obj.find("\"", kpos + search.size());

    if (vstart == string::npos) return "";
    vstart++;
    size_t vend = obj.find("\"", vstart);

    if (vend == string::npos) return "";

    return obj.substr(vstart, vend - vstart);
}

MorseNode* buildMorseTree() {
    MorseNode* root = new MorseNode();

    for (auto& [ch, code] : MORSE) {
        MorseNode* cur = root;
        for (char symbol : code) {
            if (symbol == '.') {
                if (!cur->dot)  cur->dot  = new MorseNode();
                cur = cur->dot;
            } else {
                if (!cur->dash) cur->dash = new MorseNode();
                cur = cur->dash;
            }
        }
        cur->letter = ch;
    }

    return root;
}

void freeMorseTree(MorseNode* node) {
    if (!node) return;
    freeMorseTree(node->dot);
    freeMorseTree(node->dash);
    delete node;
}

int parse_display_dist(int argc, char* argv[]) {
    if (argc < 2) { std::cerr << "Usage: ./morse <words.json>\n"; return 1; }
    std::ifstream file(argv[1]);
    if (!file.is_open()) { std::cerr << "Could not open file\n"; return 1; }

    std::ostringstream ss;
    ss << file.rdbuf();
    string content = ss.str();

    vector<Word> words;
    map<string, int> counts;
    counts["easy"] = counts["medium"] = counts["hard"] = 0;

    size_t pos = 0;
    while ((pos = content.find('{', pos)) != string::npos) {
        size_t end = content.find('}', pos);
        if (end == string::npos) break;
        string obj = content.substr(pos, end - pos + 1);

        string w = extractField(obj, "word");
        string diff = extractField(obj, "difficulty");
        string def = extractField(obj, "definition");

        if (!w.empty() && counts.find(diff) != counts.end()) {
            words.push_back({w, diff, def, scoreWord(w)});
            counts[diff]++;
        }
        pos = end + 1;
    }

    std::sort(words.begin(), words.end(), [](const Word& a, const Word& b) {
        return a.score < b.score;
    });

    auto byDiff = [&](const string& d) {
        vector<Word*> v;
        for (auto& w : words) if (w.difficulty == d) v.push_back(&w);
        return v;
    };

    auto easy = byDiff("easy");
    auto medium = byDiff("medium");
    auto hard = byDiff("hard");

    auto printRange = [](const string& name, vector<Word*>& v) {
        cout << std::fixed << std::setprecision(1);
        cout << name << ": " << v.size() << " words, score range "
                  << v.front()->score << " - " << v.back()->score << "\n";
    };

    printRange("Easy  ", easy);
    printRange("Medium", medium);
    printRange("Hard  ", hard);

    auto printExtremes = [](const string& label, vector<Word*>& v, bool top) {
        cout << "\n=== 10 " << label << " ===\n";
        cout << std::fixed << std::setprecision(1);
        int n = std::min((int)v.size(), 10);
        for (int i = 0; i < n; i++) {
            Word* w = top ? v[v.size() - n + i] : v[i];
            string defn = w->definition.size() > 100
                ? w->definition.substr(0, 100)
                : w->definition;
            cout << "  " << std::left << std::setw(20) << w->word
                      << " score: " << std::setw(5) << w->score
                      << " (" << defn << ")\n";
        }
    };

    printExtremes("EASIEST", easy, false);
    printExtremes("HARDEST", hard, true);

    // fill difficulties and shuffle
    auto populate = [](vector<Word*>& src, vector<pair<string,string>>& dest, int n) {
        vector<int> indices(src.size());
        iota(indices.begin(), indices.end(), 0);
        shuffle(indices.begin(), indices.end(), std::default_random_engine(std::chrono::steady_clock::now().time_since_epoch().count()));

        int count = std::min((int)src.size(), n);
        dest.reserve(count);
        for (int i = 0; i < count; i++)
            dest.push_back({src[indices[i]]->word, src[indices[i]]->definition});
    };

    populate(easy,   easy_list,   999);
    populate(medium, medium_list, 999);
    populate(hard,   hard_list,   999);

    return 0;
}

void ncurses_setup() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    set_escdelay(25);

    /** // TODO give color to ., -, =, 0 
    init_pair(1, COLOR_YELLOW,  COLOR_BLACK);  // @ player
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);  // . short grass
    init_pair(3, COLOR_GREEN,   COLOR_BLACK);  // : tall grass
    init_pair(4, COLOR_WHITE,   COLOR_BLACK);  // # path
    init_pair(5, COLOR_YELLOW,   COLOR_BLACK);  // % boulder
    init_pair(6, COLOR_CYAN,    COLOR_BLACK);  // ~ water
    init_pair(7, COLOR_MAGENTA,   COLOR_BLACK);  // default
    init_pair(8, COLOR_MAGENTA,   COLOR_BLACK);  // default
    init_pair(9, COLOR_WHITE,   COLOR_BLACK);  // default
    */
}

void game_render(const string& target, const string& definition,const string& input,  const string& morse, const string& error, const bool show_tree) {
    clear();

    mvprintw(ROW_TARGET_WORD, 0, "%s", target.c_str());
    mvprintw(ROW_DEFINITION,  0, "Definition: %.60s", definition.c_str());
    mvprintw(ROW_WORD_INPUT,  0, "Input      -> %s", input.c_str());
    mvprintw(ROW_MORSE_CODE,  0, "Morse Code -> %s", morse.c_str());

    if (show_tree) {
        mvprintw(7,  0, "H-S-I-E-|-T-M-O");
        mvprintw(8,  0, "  | | |   | |  ");
        mvprintw(9,  0, "  V U |   | G-Q");
        mvprintw(10, 0, "    | |   | |  ");
        mvprintw(11, 0, "    F |   | Z  ");
        mvprintw(12, 0, "  L-R-A   N-K-Y");
        mvprintw(13, 0, "      |   | |  ");
        mvprintw(14, 0, "      |   | C  ");
        mvprintw(15, 0, "    P-W   D-X  ");
        mvprintw(16, 0, "      |   |    ");
        mvprintw(17, 0, "      J   B    ");
    }

    if (!error.empty())
        mvprintw(ROW_ERROR, 0, "%s", error.c_str());

    refresh();
}

vector<pair<string,string>> start_menu() {
    mvprintw(0, 0, "SELECT Difficulty (1, 2, 3)");
    mvprintw(2, 0, "1. Easy");
    mvprintw(3, 0, "2. Medium");
    mvprintw(4, 0, "3. Hard");
    refresh();

    int ch;
    while (true) {
        ch = getch();
        switch (ch) {
            case '1':
                clear();
                return easy_list;
            case '2':
                clear();
                return medium_list;
            case '3':
                clear();
                return hard_list;
        }
    }
}

int main(int argc, char* argv[]) {
    srand((unsigned)time(NULL) * 2654435761u);

    parse_display_dist(argc, argv);

    cout << "\nPress ENTER to start..." << std::endl;
    std::cin.get();

    ncurses_setup();

    MorseNode* root = buildMorseTree();
    MorseNode* cur = root;

    vector<pair<string,string>> word_list = start_menu();
    int word_index = 0;

    string target_word = word_list.at(word_index).first;
    string definition = word_list.at(word_index).second;
    string input = "";
    string morse = "";
    string error = "";

    bool show_tree = true;
    player.words_completed = 0;
    player.errors = 0;
    player.cur_streak = 0;
    player.longest_streak = 0;
    
    game_render(target_word, definition, input, morse, error, show_tree);

    // add timing + GAME OVER screen(display streak and errors), add color to graph DONE
    // Implement controls help (Z,X, SPACE, H, Q(end game, and restart))
    int ch;
    while ((ch = getch()) != 'q') { 
        error = "";

        if (ch == 'h') {
            show_tree = !show_tree;  
        }

        if (ch == 'z') {
            if (cur->dot) {
                cur = cur->dot;
                morse += '.';
            }
        }
        else if (ch == 'x') {
            if (cur->dash) {
                cur = cur->dash;
                morse += '-';
            }
        }
        else if (ch == ' ') {
            cur = root;
            morse = "";
            player.errors++;
            if (player.cur_streak > player.longest_streak) {
                player.longest_streak = player.cur_streak;
            }
            player.cur_streak = 0;
        }

        if (ch == 'z' || ch == 'x') {
            size_t next = input.size();
            if (cur->letter != '\0' && next < target_word.size() && cur->letter == target_word[next]) {
                input += cur->letter;
                cur = root;
                morse = "";

                if (input == target_word) {
                    player.cur_streak++;
                    //string msg = "SUCCESS! Press any key to CONTINUE... (x" + std::to_string(player.cur_streak) + ")";
                    string msg = "SUCCESS! Press any key to CONTINUE... ";

                    game_render(target_word, definition, input, morse, msg, show_tree);
                    getch();
                    word_index++;
                    if (word_index >= (int)word_list.size())
                        word_index = 0;
                    target_word = word_list.at(word_index).first;
                    definition  = word_list.at(word_index).second;
                    input = "";
                }
            }
        }

        game_render(target_word, definition, input, morse, error, show_tree);
    }

    freeMorseTree(root);
    endwin();
    return 0;
}