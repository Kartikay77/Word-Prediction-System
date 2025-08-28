#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <memory>

class Node {
public:
    Node() : mContent(' '), mMarker(false) {}
    explicit Node(char c) : mContent(c), mMarker(false) {}

    char content() const { return mContent; }
    void setContent(char c) { mContent = c; }

    bool wordMarker() const { return mMarker; }
    void setWordMarker() { mMarker = true; }

    Node* findChild(char c) const {
        for (auto* child : mChildren) {
            if (child->content() == c) return child;
        }
        return nullptr;
    }

    void appendChild(Node* child) { mChildren.push_back(child); }
    const std::vector<Node*>& children() const { return mChildren; }

private:
    char mContent;
    bool mMarker;
    std::vector<Node*> mChildren;
    friend class Trie;
};

class Trie {
public:
    Trie() : root(new Node()) {}
    ~Trie() { deleteSubtree(root); }

    void addWord(const std::string& s) {
        Node* current = root;
        if (s.empty()) {
            current->setWordMarker();
            return;
        }
        for (size_t i = 0; i < s.size(); ++i) {
            Node* child = current->findChild(s[i]);
            if (child) {
                current = child;
            } else {
                Node* tmp = new Node(s[i]);
                current->appendChild(tmp);
                current = tmp;
            }
            if (i + 1 == s.size()) current->setWordMarker();
        }
    }

    bool searchWord(const std::string& s) const {
        const Node* current = root;
        for (char ch : s) {
            const Node* next = current->findChild(ch);
            if (!next) return false;
            current = next;
        }
        return current->wordMarker();
    }

    // Fill `res` with autocomplete suggestions (up to limit).
    // Returns false if the prefix doesn't exist.
    bool autoComplete(const std::string& prefix, std::vector<std::string>& res, size_t limit = 15) const {
        const Node* current = root;
        for (char ch : prefix) {
            const Node* next = current->findChild(ch);
            if (!next) return false;
            current = next;
        }
        bool keepGoing = true;
        dfsCollect(current, prefix, res, keepGoing, limit);
        return true;
    }

private:
    Node* root;

    static void deleteSubtree(Node* n) {
        if (!n) return;
        for (auto* c : n->mChildren) deleteSubtree(c);
        delete n;
    }

    static void dfsCollect(const Node* node, const std::string& built,
                           std::vector<std::string>& out, bool& keepGoing, size_t limit) {
        if (!keepGoing || !node) return;

        if (node->wordMarker()) {
            out.push_back(built);
            if (out.size() >= limit) { keepGoing = false; return; }
        }
        for (auto* child : node->children()) {
            if (!keepGoing) break;
            dfsCollect(child, built + child->content(), out, keepGoing, limit);
        }
    }
};

static bool loadDictionary(Trie& trie, const std::string& filename) {
    std::ifstream words(filename);
    if (!words) {
        std::cout << "Dictionary file not open: " << filename << "\n";
        return false;
    }
    std::string w;
    while (words >> w) {
        // normalize to lowercase
        std::transform(w.begin(), w.end(), w.begin(),
                       [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
        trie.addWord(w);
    }
    return true;
}

int main(int argc, char** argv) {
    std::string dictPath = "wordlist.txt";
    if (argc >= 2) dictPath = argv[1];

    Trie trie;
    std::cout << "Loading dictionary: " << dictPath << "\n";
    if (!loadDictionary(trie, dictPath)) {
        std::cout << "Exiting (could not load dictionary).\n";
        return 1;
    }

    while (true) {
        std::cout << "\nInteractive mode, press\n"
                  << "1: Auto Complete Feature\n"
                  << "2: Search Word\n"
                  << "3: Quit\n\n";

        int mode = 0;
        if (!(std::cin >> mode)) break;

        if (mode == 1) {
            std::cout << "Enter prefix: ";
            std::string s;
            if (!(std::cin >> s)) break;
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

            std::vector<std::string> suggestions;
            if (!trie.autoComplete(s, suggestions)) {
                std::cout << "No suggestions (prefix not found)\n";
            } else if (suggestions.empty()) {
                std::cout << "No suggestions\n";
            } else {
                std::cout << "Autocomplete reply:\n";
                for (const auto& t : suggestions) {
                    std::cout << "    " << t << "\n";
                }
            }
        } else if (mode == 2) {
            std::cout << "Enter word to search: ";
            std::string s;
            if (!(std::cin >> s)) break;
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            std::cout << (trie.searchWord(s) ? "Found\n" : "Not found\n");
        } else if (mode == 3) {
            break;
        } else {
            std::cout << "Unknown option.\n";
        }
    }

    return 0;
}
