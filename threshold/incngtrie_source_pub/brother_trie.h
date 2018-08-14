// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 


#ifndef __BROTHER_TRIE_H__
#define __BROTHER_TRIE_H__

#include "trie_base.h"

using namespace std;

namespace dbwsim {

typedef struct node_sign_tuple_t{
  uint64_t sign;
  int from;
  int label;
  int vars;
} node_sign_tuple;

class BrotherTrie : public TrieBase {
 public:

  // Define how the children structure.
  BrotherTrie* brother;
  BrotherTrie* firstchild;
  vector<int>* docids;
  
  BrotherTrie() {
    Init(0, ROOT_NOTE, -1);
  }
  
  BrotherTrie(int depth, char c, int position = -1)
  {
    Init(depth, c, position);
  }
  
  void Init(int depth, char c, int position = -1)
  {
    this->key = c;
    this->leaf_count = false;
    this->docids = NULL;
    this->depth = depth;
    this->num_children = 0;
    this->brother = NULL;
    this->firstchild = NULL;
    this->min_position = position;
    this->max_position = position;
    this->num_results = 0;
  }  
  
  ~BrotherTrie() {
    if (firstchild!=NULL)
      delete firstchild;
    if (brother!=NULL)
      delete brother;
    if(docids != NULL)
      delete docids;   
  }
  
  BaseIter* FirstChild();

  BaseIter* EndChild();

  TrieBase* SearchChar(const char chr);

  BrotherTrie* SearchCharLB(const char chr);

  TrieBase* SearchString(const char *str);
  
  int AddString(const char *str, int pid = -1, int did = -1);

  void AddDocId(int did);
  
  void BuildTrie(int num_str, const char **strs, int *ids);

  uint64_t HashFunction(vector<uint64_t>* subhash, vector<int>* docids);

  uint64_t HashSubtree(bool print = true, int *label = NULL, int *vars = NULL,
                        vector<node_sign_tuple> *sign_tuples = NULL);

  int CountSaving(int *vars_save, int *nodes_save);
  void StringMerge(int *vars_save, int *nodes_save, bool del = false);
  int SubtreeMerge(int *vars_save, int *nodes_save);
};


class BrotherIter: public BaseIter{
 public:
  BrotherTrie* current;
  BrotherIter(BrotherTrie* node){
    current = node->firstchild;
  }  
  ~BrotherIter() {}
  const void restart(TrieBase* node) {
    BrotherTrie* brother_node = (BrotherTrie*) node;
    current = brother_node->firstchild;
  }  
  const void next() {current = current->brother;}
  TrieBase* value() {return current;}
  bool IsEnd(){return current == NULL;}
};  
      

} // namespace dbwsim


#endif //__BROTHER_TRIE_H__
