
#include "trie.h"

TrieNode::~TrieNode() {
  if (child) { delete child; child = NULL; }
  if (next) { delete next; next = NULL; }
}

void TrieNode::getRecords(vector<int> &recs) {
  queue<TrieNode*> q;
  q.push(this);
  while (!q.empty()) {
    TrieNode* node = q.front();
    q.pop();
    if (!node->rids.empty()) 
      recs.insert(recs.end(), node->rids.begin(), node->rids.end());
    TrieNode* cnode = node->child;
    while (cnode) {
      q.push(cnode);
      cnode = cnode->next;
    }
  }
}

int TrieNode::preorder(int id, Trie* trie) {
  this->id = id;
  if (!rids.empty()) {
    for (auto it = rids.begin(); it != rids.end(); it++)
      trie->ids.push_back(make_pair(id, *it));
  }
  TrieNode* node = this->child;
  while (node) {
    id = id + 1;
    id = node->preorder(id, trie);
    node = node->next;
  }
  this->last = id;
  return id;
}

int Trie::buildIdx() {
  return root->preorder(1, this) + 1;
}

void Trie::bfsIndex() {
  queue<TrieNode*> q;
  q.push(root);
  while (!q.empty()) {
    TrieNode* node = q.front();
    q.pop();
    index[node->key][node->depth].push_back(node);
    TrieNode* cnode = node->child;
    while (cnode) {
      q.push(cnode);
      cnode = cnode->next;
    }
  }
}

  /* build length index*/
  /*
  for (auto it = index.begin(); it != index.end(); it++) {
    auto first = it->second.begin();
    int current = (*first)->depth;
    auto low = lower_bound(first, it->second.end(), current + 1, comp);
    int prev = 0;
    while (true) {
      index_depth[it->first].push_back(Triple(current, prev, distance(first, low)));
      if (low == it->second.end()) break;
      prev = distance(first, low);
      current = (*low)->depth;
      low = lower_bound(low, it->second.end(), current + 1, comp);
    }
  }
  */

TrieNode* TrieNode::insertChild(char ch) {
  TrieNode *node = child;
  while (node) {
    if (ch == node->key) break;
    node = node->next;
  }
  if (node == NULL) {
    node = new TrieNode();
    node->parent = this;
    node->key = ch;
    node->depth = depth + 1;
    node->next = child;
    child = node;
  }
  return node;
}

TrieNode* Trie::append(const char *str, const int rid) {
  TrieNode *node = root;
  while (*str) {
    node = node->insertChild(*str);
    ++str;
  }
  node->rids.push_back(rid);
  return node;
}
