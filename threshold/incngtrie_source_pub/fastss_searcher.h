// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __FASTSS_SEARCHER_H__
#define __FASTSS_SEARCHER_H__

#include "fastssindex.h"
#include "active_node.h"
#include "variants.h"
#include "dataset.h"
#include "resultset.h"
#include "searcher_base.h"

#include <vector>
#include <unordered_set>

#define MAX_QUERY_LENGTH 256

namespace dbwsim {
  
// FastSS Searcher is a class to process query with fastss
class FastSSSearcher : public SearcherBase {
 public:  

  void Initialize(FastssIndex* index,
                  ActiveNodePool* pool,
                  int max_tau = 3,
                  bool fixed_tau = false,
                  int tau_rate = 5)
  {
    index_ = index;
    pool_ = pool;
    current_query_[0]='\0';
    current_length_ = 0;
    current_position_ = 0;
    fixed_tau_ = fixed_tau;
    max_tau_ = max_tau;
    tau_rate_ = tau_rate;
  }

  void ProcessNode(ActiveNode* node);
  
  // This will progress one step ahead
  bool ProcessOne();

  // Fetch naive
  uint64_t FectchResultsStupid();

  // Retrieve the results of current active nodes.
  uint64_t FectchResults(bool stupidfetch);
  
};

}



#endif //__FASTSS_SEARCHER_H__
