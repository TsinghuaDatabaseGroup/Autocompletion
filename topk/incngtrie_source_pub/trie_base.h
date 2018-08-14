// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __TRIE_BASE_H__
#define __TRIE_BASE_H__

#include <map>
#include <queue>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#define ROOT_NOTE '*'
#define DELETION_NOTE '~'

namespace dbwsim {

class BaseIter;

class TrieBase{
 public:
  // The key fo this node.
  char key;
  
  // The depth of this node. 
  int depth;
  
  // Weather this is a leaf node. 
  bool leaf_count;
  //vector<int> *docids;

  // Number of children.
  int num_children;

  // Data area;
  int min_position;
  int max_position;
  int num_results;  

  //TrieBase() {}
  
  virtual ~TrieBase() {}

  // virtual TrieBase* Sibling();

  virtual BaseIter* FirstChild() = 0;
  
  virtual BaseIter* EndChild() = 0;

  virtual TrieBase* SearchChar(const char chr) = 0;
  
  virtual TrieBase* SearchString(const char *str) = 0;

  virtual int AddString(const char *str, int id = -1, int did = -1) = 0;

  virtual void BuildTrie(int num_str, const char **strs, int *ids) {}
  
  
  void PrintTrie(bool first=true, ostream& out = cout);
  void UpdatePositionRange(int position);
  void UpdateNumResults(int last_del);
};

class BaseIter{  
  // TrieBase* current;
 public:
  BaseIter(){}
  BaseIter(TrieBase* current){}
  virtual const void restart(TrieBase* current) = 0;
  virtual const void next() = 0;
  virtual TrieBase* value() = 0;
  virtual bool IsEnd() = 0;
};


class TrieStatistic{
 public:
  int max_funout;
  int funout[1000];
  map<int,int> num_results;
  int num_node;
  int num_leaf;
    
  TrieStatistic(){
    Clear();
  }
    
  void Clear(){
    for(int i= 0; i < 1000;i++){
      funout[0] = 0;
    }
    num_node = 0;
    num_leaf = 0;
    max_funout = 0;
    num_results.clear();
  }
    
  void GatherStatistic(TrieBase* root);
  
  const string ToString();
};
}

#endif // __TRIE_BASE_H__
