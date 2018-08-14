
#ifndef _M_TRIE_H_
#define _M_TRIE_H_

#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <iostream>
#include <tr1/unordered_map>
#include "memory.h"
#include "trie.h"

using namespace std;
using namespace tr1;

class MTrieNode {
  public:
    MTrieNode* next;
    MTrieNode* child;
    MTrieNode* parent;
    TrieNode* tnode;
    deque<pair<int, int>> eds;
	pair<int, int> opt;
  public:
    MTrieNode() : parent(NULL), child(NULL), next(NULL) { }
    MTrieNode* insertChild(TrieNode* node, pair<int, int> ed);
    ~MTrieNode();
};

class MTrie {
  public:
    MTrieNode *root;
  public:
    MTrie(int tau) {
      root = new MTrieNode();
      root->tnode = new TrieNode();
      root->tnode->depth = 0 - tau - 3;
      root->tnode->id = 0;
      root->tnode->last = 0x7fffffff;
    }
    ~MTrie() { if (root) { delete root; root = NULL; } }
};

#endif
