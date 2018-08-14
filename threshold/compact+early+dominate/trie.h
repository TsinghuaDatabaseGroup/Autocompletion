
#ifndef _TRIE_H_
#define _TRIE_H_

#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <iostream>
#include <tr1/unordered_map>
#include "memory.h"

using namespace std;
using namespace tr1;

class Trie;
class TrieNode {
  public:
    char key;
    short depth;
    int id;
    int last;
    TrieNode* parent;
    TrieNode* child;
    TrieNode* next;
    vector<int> rids;
  public:
    TrieNode() : key(0), depth(0), parent(NULL), child(NULL), next(NULL) { }
    TrieNode* insertChild(const char);
    int preorder(int id, Trie* trie);
    void getRecords(vector<int>&);
    ~TrieNode();
};

class Trie {
  public:
    TrieNode *root;
    vector<pair<int, int>> ids; // node_id, rec_id
    unordered_map<char, unordered_map<int, vector<TrieNode*>>> index;
  public:
    Trie() : root(new TrieNode()) { }
    TrieNode* append(const char*, const int);
    int buildIdx();
    void bfsIndex();
    ~Trie() { if (root) { delete root; root = NULL; } }
};

#endif
