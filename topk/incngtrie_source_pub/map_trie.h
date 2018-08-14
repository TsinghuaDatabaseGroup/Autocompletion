// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 


#ifndef __MAP_TRIE_H__
#define __MAP_TRIE_H__

#include "trie_base.h"

using namespace std;

namespace dbwsim {

class MapTrie : public TrieBase {
 public:

  // Define how the children structure.
  map<char, MapTrie*> *children;

  MapTrie() {
    Init(0, ROOT_NOTE, -1);
  }
  
  MapTrie(int depth, char c, int position = -1)
  {
    Init(depth, c, position);
  }
  
  void Init(int depth, char c, int position = -1)
  {
    this->key = c;
    this->leaf_count = false;
    //this->docids = NULL;
    this->depth = depth;
    this->num_children = 0;
    this->children = NULL;
    this->min_position = position;
    this->max_position = position;
    this->num_results = 0;
  }

  ~MapTrie() {}
  
  BaseIter* FirstChild();

  BaseIter* EndChild();

  TrieBase* SearchChar(const char chr);

  TrieBase* SearchString(const char *str);
  
  int AddString(const char *str, int id = -1, int did = -1);
  
  void BuildTrie(int num_str, const char **strs, int *ids);
};


class MapIter: public BaseIter{
 public:
  map<char, MapTrie*>::iterator current;
  map<char, MapTrie*>::iterator end;
  //MapTrie* trienode;
  MapIter(MapTrie* node){
    current = node->children->begin();
    end = node->children->end();    
  }
  
  ~MapIter() {}

  const void restart(TrieBase* node) {
    MapTrie* map_node = (MapTrie*) node;
    current = map_node->children->begin();
    end = map_node->children->end();    
  }
  
  const void next() {current ++;}
  TrieBase* value() {return current->second;}
  //bool IsEnd(){return current == trienode->children->end();}
  bool IsEnd(){return current == end;}  
};  
      

} // namespace dbwsim


#endif //__MAP_TRIE_H__
