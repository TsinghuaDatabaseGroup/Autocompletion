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
};

#endif  // MTRIE_H_
