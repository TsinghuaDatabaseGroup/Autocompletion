// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)


#include "fastss_searcher.h"
#include "fastssindex.h"
#include "active_node.h"
#include "variants.h"
#include "dataset.h"
#include "resultset.h"

#include <unordered_set>
#include <vector>

using namespace std;

namespace dbwsim {

bool _active_node_sort_by_expect(const ActiveNode* i, const ActiveNode* j ){
  if (i->key_match < j->key_match) return true;
  if (i->key_match > j->key_match) return false;
  if (i->node_len < j->node_len) return true;
  if (i->node_len > j->node_len) return false;
  int t = strcmp(i->node_str , j->node_str);
  if (t < 0) return true;
  if (t > 0) return false;
  return i->node_ed < j->node_ed;
}

void FastSSSearcher::ProcessNode(ActiveNode* node){
  char pattern[MAX_QUERY_LENGTH];
  int  pattern_len = 0;
  if (node == NULL){
    for (int d = 0; d <= max_tau_; d++){
      pattern[d] = '\0';
      for (int t = 0; t <= max_tau_; t++){
        ActiveNode *active_node = pool_->GetActiveNode();
        TrieBase *trie_node = index_->trie_->SearchString(pattern);
        active_node->trie_node = trie_node;
        strcpy(active_node->node_str, pattern);
        active_node->node_len = d;
        active_node->key_match = t;
        active_node->node_ed = d > t ? d: t;
        current_active_.push_back(active_node);
      }
      pattern[d] = DELETION_NOTE;
    }
    return;
  }

  char current_char = current_query_[current_position_];
  // Check if current node exist.
  TrieBase *next_trie_node = node->trie_node->SearchChar(current_char);
  if (next_trie_node == NULL){
    node->node_state = eInActive;
    return;
  }    
  strcpy(pattern, node->node_str);
  pattern[(int)node->node_len] = current_char;
  pattern[node->node_len + 1] = '\0';
  pattern_len = node->node_len + 1;
  for (int d = 0; d <= max_tau_ - node->node_ed; d++)
  {
    pattern[pattern_len + d] = '\0';
    for (int t = 0; t <= max_tau_ - node->node_ed; t++){
      TrieBase *trie_node = NULL;
      if (d == 0)
      {
        trie_node = next_trie_node;
      } else {
        trie_node = next_trie_node->SearchString(pattern + pattern_len);
      }
      if (trie_node != NULL) {
        ActiveNode *active_node = pool_->GetActiveNode();
        strcpy(active_node->node_str, pattern);
        active_node->trie_node = trie_node;
        active_node->node_len = pattern_len + d;
        active_node->key_match = current_position_ + 1 + t;
        active_node->node_ed = node->node_ed + (d > t ? d: t);
        current_active_.push_back(active_node);
      }
    }
    pattern[pattern_len + d] = DELETION_NOTE;
  }  
}

bool FastSSSearcher::ProcessOne()
{
  if (current_position_ == -1){
    ProcessNode(NULL);
    current_position_ = 0;
    last_expanded_ = 0;
    // Sort Active node set based on expected char.
    sort (current_active_.begin() + last_expanded_, current_active_.end(),
          _active_node_sort_by_expect);        
  }

  if (current_position_ >= current_length_)
    return false;
  
  while (last_expanded_ < (int)current_active_.size() &&
         current_position_ == current_active_[last_expanded_]->key_match)
  {
    bool filtered = false;
    int checkpos = last_expanded_ - 1;
    while (checkpos >= 0 && !filtered)
    {
      int cover = current_active_[last_expanded_]->CheckCover(current_active_[checkpos]);
      if (cover == 1){
        if (current_active_[last_expanded_]->trie_node->num_results <=
            current_active_[checkpos]->trie_node->num_results){
          filtered = true;
          break;
        }
      } else if (cover == -1 ){
        break;
      }
      checkpos --;
    }

    if (filtered == true) {
      current_active_[last_expanded_]->node_state = eFiltered;
    } else {      
      ProcessNode(current_active_[last_expanded_]);
      current_active_[last_expanded_]->node_state = eExtended;
    }
    last_expanded_ ++;
  }
  // Sort Active node set based on expected char.
  sort (current_active_.begin() + last_expanded_,
        current_active_.end(),
        _active_node_sort_by_expect); 
  current_position_++;

  return current_position_ < current_length_? true:false;
}


uint64_t FastSSSearcher::FectchResultsStupid(){  
  uint64_t fetched = 0;
  set<int> result;
  
  set<string> ancester;

  for (int i = last_expanded_; i < (int)current_active_.size(); i++) {
    if (current_active_[i]->node_str[current_active_[i]->node_len-1]
        == DELETION_NOTE)
      // Current Node end with ~/ We don't need to fatch result.
      continue;
    if (current_active_[i]->key_match > current_position_)
      // If we reach next level of nodex. We finished. 
      break;

    {
      bool covered = false;
      ancester.insert(current_active_[i]->node_str);
      char sub;
      int sub_pos  = current_active_[i]->node_len - 1;
      while (sub_pos >= 0){
        //if (sub_pos){
        sub = current_active_[i]->node_str[sub_pos];        
        current_active_[i]->node_str[sub_pos] = '\0';
        set<string>::iterator it =
            ancester.find(current_active_[i]->node_str);
        current_active_[i]->node_str[sub_pos] = sub;
        if (it != ancester.end()){
          covered = true;
        }
        sub_pos --;
      }
      //}
    }
      
    int minpos = current_active_[i]->trie_node->min_position;
    int maxpos = current_active_[i]->trie_node->max_position;
    
    while (minpos <= maxpos){
      fetched ++;
      result.insert(index_->variant_tuples[minpos].document_id);
      minpos ++;
    }
  }
  return fetched;  
}




uint64_t FastSSSearcher::FectchResults(bool stupidfetch)
{
  if (stupidfetch) return FectchResultsStupid();
  
  uint64_t fetched = 0;
  unordered_set<string> ancester;
  result_set_.Clear();
  for (int i = last_expanded_; i < (int)current_active_.size(); i++) {
    if (current_active_[i]->node_str[current_active_[i]->node_len-1]
        == DELETION_NOTE)
      // Current Node end with ~/ We don't need to fatch result.
      continue;
    if (current_active_[i]->key_match > current_position_)
      // If we reach next level of nodex. We finished. 
      break;
    int checkpos = i + 1;
    bool covered = false;   
    while (checkpos < (int)current_active_.size()){
      int cover;
      cover  = current_active_[checkpos]->CheckCover(current_active_[i]);
      if (cover != 0) {        
        if (cover == 1 ) {
          if (current_active_[checkpos]->
              node_str[current_active_[checkpos]->node_len-1] !=
              DELETION_NOTE){
            covered = true;
            break;
          }
        } else {
          break;
        }        
      }
      checkpos ++;
    }
    
    //Check Ancester
    if (covered != true){
      ancester.insert(current_active_[i]->node_str);
      char sub;
      int sub_pos  = current_active_[i]->node_len - 1;
      while (sub_pos >= 0 && sub_pos >= current_position_ - max_tau_){
        sub = current_active_[i]->node_str[sub_pos];
        current_active_[i]->node_str[sub_pos] = '\0';
        unordered_set<string>::iterator it =
            ancester.find(current_active_[i]->node_str);
        current_active_[i]->node_str[sub_pos] = sub;
        if (it != ancester.end()){
          covered = true;
        }
        sub_pos --;
      }
    }
    
    // Not covered.
    if (covered != true){
      current_active_[i]->is_fetched = true;
      int minpos = current_active_[i]->trie_node->min_position;
      int maxpos = current_active_[i]->trie_node->max_position;
      
      while (minpos <= maxpos){
        fetched ++;
        result_set_.add(index_->variant_tuples[minpos].document_id);
        if (minpos == index_->variant_tuples[minpos].next)
          break;
        minpos = index_->variant_tuples[minpos].next;      
      }
    }
  }
  return fetched;
}

}
