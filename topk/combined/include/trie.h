#ifndef TRIE_H_
#define TRIE_H_

#include <vector>
#include <unordered_map>
#include <unordered_set>

class Trie;

struct TrieNode {
    char ch;
    int id;     // preorder
    int last;   // preorder
    int depth;
    TrieNode* parent;
    TrieNode* child;
    TrieNode* next;
    int rid;    // record id

    TrieNode(const char _ch = 0, int _depth = 0, TrieNode* _parent = NULL) : 
        ch(_ch), depth(_depth), rid(-1), parent(_parent), child(NULL), next(NULL) {}
    ~TrieNode();
    TrieNode* insert_child(const char ch);
    int preorder(int id, Trie* trie);
    void get_records(std::unordered_set<int>& records, unsigned K, Trie* trie);
};

class Trie {
public:
    TrieNode* root;
    std::vector<std::pair<int, int>> rids; // <node id, record id>
    std::unordered_map<
        char,
        std::unordered_map<int, std::vector<TrieNode*>>
    > index;

    Trie() : root(new TrieNode()) {}
    ~Trie();
    TrieNode* insert(const std::string& str, const int rid);
    void build_index();
};

#endif  // TRIE_H_
