// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

#include "searcher_base.h"

using namespace std;

namespace dbwsim {

SearcherBase::SearcherBase()
{
}

SearcherBase::~SearcherBase()
{
}

// This is the function to reset the current searcher
// and prepare for the next comming query. 
void SearcherBase::ResetSearcher() {
  for (vector<ActiveNode*>::iterator it
           = current_active_.begin();
       it < current_active_.end(); it ++ ) {
    pool_->ReturnActiveNode(*it);
  }
  current_active_.clear();
  current_query_[0] = '\0';
  current_length_ = 0;
  result_set_.Clear();
  current_position_ = -1;
  last_expanded_ = 0;
}

// To append additional query characters to current
// query for further process. This will not process
// the newly appended query. Please use processone
// or processall.   
void SearcherBase::ExtendQuery(char *new_str, int new_length)
{
  if (current_length_ + new_length > MAX_QUERY_LENGTH){
    new_length = MAX_QUERY_LENGTH - current_length_;
  }          
  strncpy(current_query_ + current_length_, new_str, new_length);
  current_length_ += new_length;
  current_query_[current_length_] = '\0';    
}


// until the end of the query. The query result is
// not materilized. 
void SearcherBase::ProcessAll(bool fetch)
{
  while (ProcessOne()){
  }
  if (fetch)
    FectchResults();
}
  
// This will process the left over query one character
// by one character to the end of current query.
// At the end of each query. We will calculate the
// result set at each setp. 
void SearcherBase::ProcessOneByOne()
{
  while (ProcessOne()){
    FectchResults();
  }
}
  
void SearcherBase::ResultsStatistic(ostream& out, bool show_node, bool show_results)
{
  out << "Query: " << current_query_;
  out << "\t Result#: " << result_set_.size();
  out << "\t Active#: " << current_active_.size() << endl;
  if (show_node) {
    for (int i = 0; i < (int)current_active_.size(); i ++){
      out << "  >> ANode: " << i << "\t" << current_active_[i]->ToString() << endl;
    }
  }
  if (show_results){
    int i = 0; 
    for (unordered_set<int>::iterator it = result_set_.result_ids_.begin();
         it != result_set_.result_ids_.end(); it ++)
    {
      int did = *it;
      const string data = index_->dataset_.GetDocumentByID(did);
      out << "  << Result: " << i << "\t" << current_query_ << "\t DID: " << did <<  "\t\"" << data << "\"" << endl;
    }
  }
}

} // namespace dbwsvn

