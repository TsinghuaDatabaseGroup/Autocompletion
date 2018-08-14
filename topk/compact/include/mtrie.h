#ifndef MTRIE_H_
#define MTRIE_H_

#include <vector>
#include "match.h"
#include "trie.h"

struct MTrieNode {
    std::vector<Match> matches;
    TrieNode* node;
    int key_index;
    int min_index;

    MTrieNode* parent;
    MTrieNode* child;
    MTrieNode* next;
};

struct MTrie {
    MTrieNode* root;
};

#endif  // MTRIE_H_
