// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __RESULTSET_H__
#define __RESULTSET_H__

//#include <unordered_set>
#include <unordered_set>

using namespace std;

namespace dbwsim{

class ResultSet {
 public:
  unordered_set<int> result_ids_;
  //set<int> result_ids_;
  
  void add (int document_id) {
    result_ids_.insert(document_id);
  }
  
  void Clear() {
    result_ids_.clear();
  }

  int size() {
    return result_ids_.size();
  } 
};

}

#endif //__RESULTSET_H__
