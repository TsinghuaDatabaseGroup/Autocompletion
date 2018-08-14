// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

#ifndef __SEARCHER_BASE_H__
#define __SEARCHER_BASE_H__

#include "fastssindex.h"
#include "active_node.h"
#include "variants.h"
#include "dataset.h"
#include "resultset.h"

#include <set>

#define MAX_QUERY_LENGTH 256
namespace dbwsim {

class SearcherBase {
 public:
  FastssIndex* index_;
  ActiveNodePool* pool_;
  char current_query_[MAX_QUERY_LENGTH];
  int  current_length_;
  
  // Metrics
  int  max_tau_;
  bool fixed_tau_;
  int  tau_rate_;
  
  // Active node set;
  int  current_position_;
  vector<ActiveNode*> current_active_;
  ResultSet result_set_;
  
  int last_expanded_;
  
  SearcherBase();

  virtual ~SearcherBase();
  
  // Initialize the searcher with index and
  // activenodepool Also need the threshold. 
  virtual void Initialize(FastssIndex* index,
                          ActiveNodePool* pool,
                          int max_tau = 3,
                          bool fixed_tau = false,
                          int tau_rate = 5) = 0;

  // This is the function to reset the current searcher
  // and prepare for the next comming query. 
  void ResetSearcher();
  
  // To append additional query characters to current
  // query for further process. This will not process
  // the newly appended query. Please use processone
  // or processall.   
  void ExtendQuery(char *new_str, int new_length);
  
  // This will only process one chacater of the left
  // over query. If the current query is at the end,
  // It will return false, Otherwise return true.   
  virtual bool ProcessOne() = 0;


  virtual uint64_t FectchResultsStupid() = 0;

  // This will fectch the results ids for current nodes.
  virtual uint64_t FectchResults(bool stupidfetch = false) = 0;

  // This will process all the left over characters.
  // until the end of the query. The query result is
  // not materilized. 
  void ProcessAll(bool fetch = true);
    
  // This will process the left over query one character
  // by one character to the end of current query.
  // At the end of each query. We will calculate the
  // result set at each setp. 
  void ProcessOneByOne();

  // Print out the result statistic for analysis.
  void ResultsStatistic(ostream& out, bool show_node = false,
                        bool show_results = false);
  
};

} // namespace dbwsim

#endif  // __SEARCHER_BASE_H__
