#ifndef MATCH_H_
#define MATCH_H_

#include <set>
#include "trie.h"

struct Match {
    TrieNode* node; // match node
    int i;          // input index
    int ped;        // prefix edit distance for this match
    int md;         // manhattan distance to current processing line

    Match(TrieNode* _node, int _i, int _ped, int _md) :
        node(_node), i(_i), ped(_ped), md(_md) {}
    inline bool operator < (const Match &other) const {
        return this->ped + this->md < other.ped + other.md;
    }
};

#endif  // MATCH_H_
