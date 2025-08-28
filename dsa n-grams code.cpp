// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include <algorithm>
// #include <cctype>
// #include <memory>

// class Node {
// public:
//     Node() : mContent(' '), mMarker(false) {}
//     explicit Node(char c) : mContent(c), mMarker(false) {}

//     char content() const { return mContent; }
//     void setContent(char c) { mContent = c; }

//     bool wordMarker() const { return mMarker; }
//     void setWordMarker() { mMarker = true; }

//     Node* findChild(char c) const {
//         for (auto* child : mChildren) {
//             if (child->content() == c) return child;
//         }
//         return nullptr;
//     }

//     void appendChild(Node* child) { mChildren.push_back(child); }
//     const std::vector<Node*>& children() const { return mChildren; }

// private:
//     char mContent;
//     bool mMarker;
//     std::vector<Node*> mChildren;
//     friend class Trie;
// };

// class Trie {
// public:
//     Trie() : root(new Node()) {}
//     ~Trie() { deleteSubtree(root); }

//     void addWord(const std::string& s) {
//         Node* current = root;
//         if (s.empty()) {
//             current->setWordMarker();
//             return;
//         }
//         for (size_t i = 0; i < s.size(); ++i) {
//             Node* child = current->findChild(s[i]);
//             if (child) {
//                 current = child;
//             } else {
//                 Node* tmp = new Node(s[i]);
//                 current->appendChild(tmp);
//                 current = tmp;
//             }
//             if (i + 1 == s.size()) current->setWordMarker();
//         }
//     }

//     bool searchWord(const std::string& s) const {
//         const Node* current = root;
//         for (char ch : s) {
//             const Node* next = current->findChild(ch);
//             if (!next) return false;
//             current = next;
//         }
//         return current->wordMarker();
//     }

//     // Fill `res` with autocomplete suggestions (up to limit).
//     // Returns false if the prefix doesn't exist.
//     bool autoComplete(const std::string& prefix, std::vector<std::string>& res, size_t limit = 15) const {
//         const Node* current = root;
//         for (char ch : prefix) {
//             const Node* next = current->findChild(ch);
//             if (!next) return false;
//             current = next;
//         }
//         bool keepGoing = true;
//         dfsCollect(current, prefix, res, keepGoing, limit);
//         return true;
//     }

// private:
//     Node* root;

//     static void deleteSubtree(Node* n) {
//         if (!n) return;
//         for (auto* c : n->mChildren) deleteSubtree(c);
//         delete n;
//     }

//     static void dfsCollect(const Node* node, const std::string& built,
//                           std::vector<std::string>& out, bool& keepGoing, size_t limit) {
//         if (!keepGoing || !node) return;

//         if (node->wordMarker()) {
//             out.push_back(built);
//             if (out.size() >= limit) { keepGoing = false; return; }
//         }
//         for (auto* child : node->children()) {
//             if (!keepGoing) break;
//             dfsCollect(child, built + child->content(), out, keepGoing, limit);
//         }
//     }
// };

// static bool loadDictionary(Trie& trie, const std::string& filename) {
//     std::ifstream words(filename);
//     if (!words) {
//         std::cout << "Dictionary file not open: " << filename << "\n";
//         return false;
//     }
//     std::string w;
//     while (words >> w) {
//         // normalize to lowercase
//         std::transform(w.begin(), w.end(), w.begin(),
//                       [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
//         trie.addWord(w);
//     }
//     return true;
// }

// int main(int argc, char** argv) {
//     std::string dictPath = "wordlist.txt";
//     if (argc >= 2) dictPath = argv[1];

//     Trie trie;
//     std::cout << "Loading dictionary: " << dictPath << "\n";
//     if (!loadDictionary(trie, dictPath)) {
//         std::cout << "Exiting (could not load dictionary).\n";
//         return 1;
//     }

//     while (true) {
//         std::cout << "\nInteractive mode, press\n"
//                   << "1: Auto Complete Feature\n"
//                   << "2: Search Word\n"
//                   << "3: Quit\n\n";

//         int mode = 0;
//         if (!(std::cin >> mode)) break;

//         if (mode == 1) {
//             std::cout << "Enter prefix: ";
//             std::string s;
//             if (!(std::cin >> s)) break;
//             std::transform(s.begin(), s.end(), s.begin(),
//                           [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

//             std::vector<std::string> suggestions;
//             if (!trie.autoComplete(s, suggestions)) {
//                 std::cout << "No suggestions (prefix not found)\n";
//             } else if (suggestions.empty()) {
//                 std::cout << "No suggestions\n";
//             } else {
//                 std::cout << "Autocomplete reply:\n";
//                 for (const auto& t : suggestions) {
//                     std::cout << "    " << t << "\n";
//                 }
//             }
//         } else if (mode == 2) {
//             std::cout << "Enter word to search: ";
//             std::string s;
//             if (!(std::cin >> s)) break;
//             std::transform(s.begin(), s.end(), s.begin(),
//                           [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
//             std::cout << (trie.searchWord(s) ? "Found\n" : "Not found\n");
//         } else if (mode == 3) {
//             break;
//         } else {
//             std::cout << "Unknown option.\n";
//         }
//     }

//     return 0;
// }

// ngram_autocomplete.cpp  (C++11-compatible)
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;

static vector<string> tokenize(istream& in) {
    vector<string> tokens;
    string buf, line;
    const auto flush = [&](){
        if (!buf.empty()) { tokens.push_back(buf); buf.clear(); }
    };
    while (getline(in, line)) {
        for (char c : line) {
            if (isalpha(static_cast<unsigned char>(c))) {
                buf.push_back(static_cast<char>(tolower(static_cast<unsigned char>(c))));
            } else if (c == '\'') {
                flush();
            } else {
                flush();
                if (c == '.' || c == '!' || c == '?') tokens.push_back("<s>");
            }
        }
        flush();
        tokens.push_back("<s>");
    }
    while (!tokens.empty() && tokens.back() == "<s>") tokens.pop_back();
    return tokens;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    const string corpusPath = "corpus.txt";
    ifstream fin(corpusPath.c_str());
    if (!fin) {
        cerr << "Could not open " << corpusPath << ". Place it next to the program.\n";
        return 1;
    }

    vector<string> toks = tokenize(fin);
    if (toks.empty()) {
        cerr << "Corpus is empty after tokenization.\n";
        return 1;
    }

    const string BOS = "<s>";
    unordered_map<string, int> unigram;
    unordered_map<string, unordered_map<string, int> > bigram;

    if (toks.front() != BOS) toks.insert(toks.begin(), BOS);
    for (size_t i = 0; i < toks.size(); ++i) unigram[toks[i]]++;
    for (size_t i = 1; i < toks.size(); ++i) bigram[toks[i-1]][toks[i]]++;

    // C++11-compatible topK (explicit comparator types)
    const size_t Kdef = 5;
    const string* prevPtr = NULL; // just to emphasize capture by reference below not needed

    auto topK = [&](const string& prev, const string& prefix, size_t K) {
        vector<pair<string,int> > cand;

        unordered_map<string, unordered_map<string,int> >::const_iterator it = bigram.find(prev);
        if (it != bigram.end()) {
            const unordered_map<string,int>& nexts = it->second;
            for (unordered_map<string,int>::const_iterator kv = nexts.begin(); kv != nexts.end(); ++kv) {
                const string& w = kv->first;
                if (w == BOS) continue;
                if (prefix.empty() || w.compare(0, prefix.size(), prefix) == 0) {
                    cand.push_back(make_pair(w, kv->second));
                }
            }
        }
        if (cand.empty()) {
            for (unordered_map<string,int>::const_iterator kv = unigram.begin(); kv != unigram.end(); ++kv) {
                const string& w = kv->first;
                if (w == BOS) continue;
                if (prefix.empty() || w.compare(0, prefix.size(), prefix) == 0) {
                    cand.push_back(make_pair(w, kv->second));
                }
            }
        }

        sort(cand.begin(), cand.end(),
             [](const pair<string,int>& a, const pair<string,int>& b) {
                 if (a.second != b.second) return a.second > b.second; // freq desc
                 return a.first < b.first; // lexicographic tie-break
             });

        if (cand.size() > K) cand.resize(K);
        return cand;
    };

    auto show = [&](const string& prev, const string& prefix){
        vector<pair<string,int> > s = topK(prev, prefix, Kdef);
        if (s.empty()) {
            cout << "No suggestions.\n";
            return;
        }
        cout << "Suggestions";
        if (!prefix.empty()) cout << " for \"" << prefix << "\"";
        cout << " after \"" << (prev == BOS ? "<start>" : prev) << "\":\n";
        for (size_t i = 0; i < s.size(); ++i) {
            cout << "  " << (i+1) << ") " << s[i].first << "  (freq=" << s[i].second << ")\n";
        }
        cout << "Press 1 to accept top suggestion.\n";
    };

    cout << "N-gram (bigram) autocomplete loaded from " << corpusPath << "\n";
    cout << "Controls:\n"
         << "  • Type letters [a-z] to build the next word\n"
         << "  • '<' = backspace one letter\n"
         << "  • '!' = start a new sentence (context reset)\n"
         << "  • '1' = accept TOP suggestion\n"
         << "  • '2' = accept exactly what you typed\n"
         << "  • '0' = finish\n\n";

    string sentence, current, prev = BOS;
    bool done = false;

    show(prev, current);

    while (!done) {
        char ch;
        if (!(cin >> ch)) break;

        if (ch == '0') {
            done = true;
        } else if (ch == '1') {
            vector<pair<string,int> > s = topK(prev, current, 1);
            if (!s.empty()) {
                const string& w = s[0].first;
                if (!sentence.empty()) sentence.push_back(' ');
                sentence += w;
                prev = w;
                current.clear();
                cout << sentence << "\n";
                show(prev, current);
            } else {
                cout << "No suggestion to accept.\n";
            }
        } else if (ch == '2') {
            if (!current.empty()) {
                if (!sentence.empty()) sentence.push_back(' ');
                sentence += current;
                prev = current;
                current.clear();
                cout << sentence << "\n";
                show(prev, current);
            } else {
                cout << "(Nothing typed yet.)\n";
            }
        } else if (ch == '<') {
            if (!current.empty()) current.pop_back();
            show(prev, current);
        } else if (ch == '!') {
            prev = BOS;
            current.clear();
            cout << "[New sentence]\n";
            show(prev, current);
        } else if (isalpha(static_cast<unsigned char>(ch))) {
            current.push_back(static_cast<char>(tolower(static_cast<unsigned char>(ch))));
            show(prev, current);
        }
    }

    if (!current.empty()) {
        if (!sentence.empty()) sentence.push_back(' ');
        sentence += current;
    }
    cout << "\nFinal: " << sentence << "\n";
    return 0;
}
