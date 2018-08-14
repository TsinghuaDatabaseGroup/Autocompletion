// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 


#ifndef __VECTOR_TRIE_H__
#define __VECTOR_TRIE_H__

#include "trie_base.h"

using namespace std;

namespace dbwsim {

class VectorTrie : public TrieBase {
 public:

  // Define how the children structure.
  vector<VectorTrie* >* children;

  vector<int>* docids;
  
  VectorTrie() {
    Init(0, ROOT_NOTE, -1);
  }
  
  VectorTrie(int depth, char c, int position = -1)
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
    this->children = NULL;
    this->min_position = position;
    this->max_position = position;
    this->num_results = 0;
  }  
  
  ~VectorTrie() {
    if (children!=NULL){
      for(vector<VectorTrie* >::iterator it = children->begin();
          it != children->end(); it++ ){
        delete (*it);
      }
      delete children;
    }
    if(docids != NULL)
      delete docids;   
  }
  
  BaseIter* FirstChild();

  BaseIter* EndChild();

  TrieBase* SearchChar(const char chr);

  int SearchCharLB(const char chr);

  TrieBase* SearchString(const char *str);
  
  int AddString(const char *str, int pid = -1, int did = -1);

  void AddDocId(int did);
  
  void BuildTrie(int num_str, const char **strs, int *ids);

  uint64_t HashFunction(vector<uint64_t>* subhash, vector<int>* docids);
  uint64_t HashSubtree(bool print = true, int *label = NULL, int *vars = NULL);
};

class VectorIter: public BaseIter{
 public:
  vector<VectorTrie* >::iterator current;
  vector<VectorTrie* >::iterator end;
  VectorIter(VectorTrie* node){
    current = node->children->begin();
    end = node->children->end();
  }  
  ~VectorIter() {}
  const void restart(TrieBase* node) {
    VectorTrie* vector_node = (VectorTrie*) node;
    current = vector_node->children->begin();
    end = vector_node->children->end();   
  }  
  const void next() {current ++;}
  TrieBase* value() {return *current;}
  bool IsEnd(){return current == end;}
};  
      

} // namespace dbwsim


#endif //__VECTOR_TRIE_H__
