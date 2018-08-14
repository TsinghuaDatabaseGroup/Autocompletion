
#ifndef _TRIE_H_
#define _TRIE_H_

#include <vector>
#include <string>
#include <queue>

using namespace std;

template <class Type> class Trie;
template <class Type>
class TrieNode {
public:
  TrieNode<Type>* parent;
  unsigned depth;
  int id;
  int last;
  char character;
  vector<TrieNode<Type>*> children;
  vector<int> ids;

  int preorder(int id, Trie<Type>* trie) {
	  this->id = id;
	  if (!ids.empty()) {
		for (auto rid : ids)
		  trie->ids.push_back(make_pair(id, rid));
	  }
	  for (auto child : children) {
		  ++id;
		  id = child->preorder(id, trie);
	  }
	  this->last = id;
	  return id;
  }
  unsigned int getChildrenCount() {
    return children.size();
  }
  TrieNode* getChild(unsigned int idx) {
    return children[idx];
  }
  unsigned getDepth() {
    return depth;
  }
  TrieNode<Type>* getParent() {
    return parent;
  }
  char getCharacter() {
    return character;
  }
  TrieNode<Type>* insertChild(char ch) {
    auto vit = children.begin();
		for (; vit != children.end(); vit++)
			if ((*vit)->getCharacter() == ch) break;

    if (vit == children.end()) {
      TrieNode<Type>* node = new TrieNode<Type>();
      node->character = ch;
      node->depth = depth + 1;
      children.push_back(node);
      node->parent = this;
      return node;
    }
    return *vit;
  }

  void getRecords(vector<int> &recs) {
    queue<TrieNode<Type>*> q;
    q.push(this);
    while (!q.empty()) {
      TrieNode<Type>* node = q.front();
      q.pop();
      if (!node->ids.empty()) 
        recs.insert(recs.end(), node->ids.begin(), node->ids.end());
      for (auto vit = node->children.begin(); vit != node->children.end(); ++vit) {
        q.push(*vit);
      }
    }
  }
};

template <class Type>
class Trie {
private:
  TrieNode<Type>* root;
public:
  vector<pair<int, int>> ids;
  Trie() {
    root = new TrieNode<Type>();
  }
  TrieNode<Type>* getRoot() {
    return root;
  }
  TrieNode<Type>* append(const char *str, const int rid) {
    TrieNode<Type> *node = root;
    while (*str) {
      node = node->insertChild(*str);
      ++str;
    }
    node->ids.push_back(rid);
    return node;
  }
  void buildIdx() {
	  this->root->preorder(1, this);
  }
};

#endif
