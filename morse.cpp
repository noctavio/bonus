#include "classes.cpp"

const map<char, double> DEPTH = {
    {'e',1},{'t',1},
    {'a',2},{'i',2},{'n',2},{'m',2},
    {'s',3},{'u',3},{'r',3},{'w',3},{'d',3},{'k',3},{'g',3},{'o',3},
    {'h',4},{'v',4},{'f',4},{'l',4},{'p',4},{'j',4},
    {'b',4},{'x',4},{'c',4},{'y',4},{'z',4},{'q',4}
};

double scoreWord(const std::string& word) {
    double s = 0;
    for (char c : word) {
        auto it = DEPTH.find(c);
        s += (it != DEPTH.end()) ? it->second : 3.0;
    }
    return s + word.size() * 0.75;
}

std::string extractField(const std::string& obj, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t kpos = obj.find(search);
    if (kpos == std::string::npos) return "";
    size_t vstart = obj.find("\"", kpos + search.size());
    if (vstart == std::string::npos) return "";
    vstart++;
    size_t vend = obj.find("\"", vstart);
    if (vend == std::string::npos) return "";
    return obj.substr(vstart, vend - vstart);
}

struct Word {
    std::string word, difficulty, definition;
    double score;
};

int main(int argc, char* argv[]) {
    if (argc < 2) { std::cerr << "Usage: ./morse <words.json>\n"; return 1; }
    std::ifstream file(argv[1]);
    if (!file.is_open()) { std::cerr << "Could not open file\n"; return 1; }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    std::vector<Word> words;
    std::map<std::string, int> counts;
    counts["easy"] = counts["medium"] = counts["hard"] = 0;

    size_t pos = 0;
    while ((pos = content.find('{', pos)) != std::string::npos) {
        size_t end = content.find('}', pos);
        if (end == std::string::npos) break;
        std::string obj = content.substr(pos, end - pos + 1);

        std::string w    = extractField(obj, "word");
        std::string diff = extractField(obj, "difficulty");
        std::string def  = extractField(obj, "definition");

        if (!w.empty() && counts.find(diff) != counts.end()) {
            words.push_back({w, diff, def, scoreWord(w)});
            counts[diff]++;
        }
        pos = end + 1;
    }

    // Sort by score
    std::sort(words.begin(), words.end(), [](const Word& a, const Word& b) {
        return a.score < b.score;
    });

    // Score ranges per tier
    auto byDiff = [&](const std::string& d) {
        std::vector<Word*> v;
        for (auto& w : words) if (w.difficulty == d) v.push_back(&w);
        return v;
    };

    auto easy   = byDiff("easy");
    auto medium = byDiff("medium");
    auto hard   = byDiff("hard");

    auto printRange = [](const std::string& name, std::vector<Word*>& v) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << name << ": " << v.size() << " words, score range "
                  << v.front()->score << " - " << v.back()->score << "\n";
    };

    printRange("Easy  ", easy);
    printRange("Medium", medium);
    printRange("Hard  ", hard);

    auto printExtremes = [](const std::string& label, std::vector<Word*>& v, bool top) {
        std::cout << "\n=== 10 " << label << " ===\n";
        std::cout << std::fixed << std::setprecision(1);
        int n = std::min((int)v.size(), 10);
        for (int i = 0; i < n; i++) {
            Word* w = top ? v[v.size() - n + i] : v[i];
            std::string defn = w->definition.size() > 50
                ? w->definition.substr(0, 50)
                : w->definition;
            std::cout << "  " << std::left << std::setw(20) << w->word
                      << " score: " << std::setw(5) << w->score
                      << " (" << defn << ")\n";
        }
    };

    printExtremes("EASIEST", easy,   false);
    printExtremes("HARDEST", hard,   true);

    return 0;
}