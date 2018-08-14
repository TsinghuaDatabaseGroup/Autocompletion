// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

#include "fastssindex.h"
#include "variants.h"
#include <iostream>
#include <utility>
#include <string>
#include <vector>

#include "easy_timer.h"
#include "brother_trie.h"

using namespace std;

namespace dbwsim{

bool _compare_variants_pair( const pair<string, int>& i, const pair<string, int>& j ) {
  if( i.first < j.first ) return true;
  if( i.first > j.first ) return false;
  return i.second < j.second;
}

void FastssIndex::Initilization(const string &filename, int tau,
                                bool normalized, int norm_gap, TrieBase *trie)
{
  dataset_.InitFromFile(filename, 200);
  variant_generator_.set_tau(tau);
  vector< pair<string, int> > variants_did_list;  

#ifdef DEBUG
  EasyTimer indexing_timer;
  indexing_timer.Start();
  cerr << "Indexing Start NG Generation" << endl;
#endif

  for (int i = 0; i < dataset_.num_documents_; i++) {
    vector<string> variants;
    const string & doc_str = dataset_.GetDocumentByID(i);
    if (normalized) {
      variant_generator_.GenerateVariantsPerGap(doc_str, variants, norm_gap);
    } else {
      variant_generator_.GenerateVariants(doc_str, variants);
    }

    for (vector<string>::iterator it = variants.begin();
         it < variants.end(); it ++ ) {
      variants_did_list.push_back (make_pair(*it, i));
    }
  }

#ifdef DEBUG
  indexing_timer.Finish();
  cerr << "Indexing Start NG Generation takes " << indexing_timer.ToString() << endl;
  cerr << "Indexing Start NG Sorting" << endl;
  indexing_timer.Start();
#endif  

  // Sort vector
  sort (variants_did_list.begin(), variants_did_list.end(), _compare_variants_pair);

#ifdef DEBUG
  indexing_timer.Finish();
  cerr << "Indexing Start NG Sorting takes " << indexing_timer.ToString() << endl;  
  cerr << "Indexing Start NG Connecting" << endl;
  indexing_timer.Start();
#endif  

  
  variant_tuples = new VariantTuple[variants_did_list.size()];
  int prev_last_del_location[256];
  for (int i = 0; i < 256; i ++ ) prev_last_del_location[i] = -1;
  int count  = 0;
  for (vector<pair<string, int> >::iterator it = variants_did_list.begin();
       it < variants_did_list.end(); it ++, count ++) {
    variant_tuples[count].variant_string = it->first;
    variant_tuples[count].document_id = it->second;
    variant_tuples[count].next = count;
    
    size_t last_del = it->first.rfind(DELETION_NOTE);
    if (last_del==string::npos) {
      //variant_tuples[count].last_del_pos = - 1;
      if (prev_last_del_location[255] >= 0) {
        variant_tuples[prev_last_del_location[255]].next = count;        
      }
      prev_last_del_location[255] = count;      
    } else {
      //variant_tuples[count].last_del_pos = last_del;
      if (prev_last_del_location[last_del] >= 0) {
        variant_tuples[prev_last_del_location[last_del]].next = count;
      }
      prev_last_del_location[last_del] = count;
    }
  }
  num_variant_tuples = count;

#ifdef DEBUG
  indexing_timer.Finish();
  cerr << "Indexing Start NG Connecting takes " << indexing_timer.ToString() << endl;  
  cerr << "Indexing Start NG Trie BUILDING" << endl;
  indexing_timer.Start();
#endif  
   
  // Build Trie index.
  if (trie == NULL){
    trie = new MapTrie();
  }  
  trie_ = trie;
  for (int i = 0; i < num_variant_tuples; i ++) {
    trie_->AddString(variant_tuples[i].variant_string.c_str(), i);
  }

#ifdef DEBUG
  indexing_timer.Finish();
  cerr << "Indexing Start NG Trie BUILDING takes " << indexing_timer.ToString() << endl;  
  cerr << "Indexing Finished NG" << endl;
#endif  

}


void FastssIndex::FastInit(const string &filename, int tau,
                           bool normalized, int norm_gap, TrieBase *trie)
{
  dataset_.InitFromFile(filename, 200);
  variant_generator_.set_tau(tau);
  vector< pair<string, int> > variants_did_list;

  temptrie = new BrotherTrie();
  
#ifdef DEBUG
  cerr << "Indexing Start NG Generation" << endl;
#endif

  for (int i = 0; i < dataset_.num_documents_; i++) {
    vector<string> variants;
    const string & doc_str = dataset_.GetDocumentByID(i);
    if (normalized) {
      variant_generator_.GenerateVariantsPerGap(doc_str, variants, norm_gap);
    } else {
      variant_generator_.GenerateVariants(doc_str, variants);
    }

    for (vector<string>::iterator it = variants.begin();
         it < variants.end(); it ++ ) {     
      //variants_did_list.push_back (make_pair(*it, i));
      // Insert the current string into the 
      temptrie->AddString((*it).c_str(), -1, i);
    }
  }

#ifdef DEBUG
  cerr << "Statistic Start" << endl;
#endif

  TrieStatistic triestate;
  triestate.Clear();
  triestate.GatherStatistic(temptrie);
  
  // Index Reduction. 
  int string_merge_node_saving = 0;
  int string_merge_vars_saving = 0;
  int subtree_merge_node_saving = 0;
  int subtree_merge_vars_saving = 0;
  
  // printf("\nHash tree structure:\n");

#ifdef DEBUG
  cerr << "StringMerge Start" << endl;
#endif

  
  temptrie->StringMerge(&string_merge_vars_saving,
                      &string_merge_node_saving, true);
#ifdef DEBUG
  cerr << "SubtreeMerge Start" << endl;
#endif
  
  temptrie->SubtreeMerge(&subtree_merge_vars_saving,
                       &subtree_merge_node_saving);  
  
  cout << filename << " tau: " << tau << " ndocs: " << dataset_.num_documents_;
  cout << " TotalVars: " << triestate.num_leaf;
  cout << " TotalNodes: " << triestate.num_node;  
  cout << " StringMergeSaveVars: " << string_merge_vars_saving
       << " StringMergeSaveNode: " << string_merge_node_saving
       << " SubtreeMergSaveeVars: " << subtree_merge_vars_saving
       << " SubtreeMergeSaveNode: " << subtree_merge_node_saving
       << endl;

#ifdef DEBUG
  cerr << "All Finished" << endl;
#endif  
  
}



}
