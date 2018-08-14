// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "trie_base.h"
#include <iostream>


using namespace std;

namespace dbwsim{

void TrieBase::PrintTrie(bool first, ostream& out)
{
  //stringstream str_stream;
  if (!first) {
    for (int i = 0; i < depth; i++)
      out << "|\t\t";
  }
    
  first = true;
  out << key << (leaf_count ? '\'':' ');
  out << "(" << min_position << "|" << max_position
      << "|" << num_results << ")\t";
  
  if (num_children != 0) {
    BaseIter* it = FirstChild();
    for (; !it->IsEnd(); it->next())
    {
      (it->value())->PrintTrie(first);
      first = false;
    }
    delete it;
  }
  else {
    out << endl;
  }
}   

void TrieBase::UpdatePositionRange(int position)
{
  if (position == -1)
    return;
  if (min_position == -1 || position < min_position)
  {
    min_position = position;
  }
  if (max_position == -1 || position > max_position)
  {
    max_position = position;
  }
}
  
void TrieBase::UpdateNumResults(int last_del)
{
  if (last_del < depth)
    num_results ++;
}

void TrieStatistic::GatherStatistic(TrieBase* root)
{
  num_node ++;
  num_results[root->num_results] ++;
  num_leaf += root->leaf_count;
  if (root->num_children == 0){
    funout[0] ++;
  }else {
    if (root->num_children > max_funout){
      max_funout = root->num_children;
    }
    funout[root->num_children] ++;
    BaseIter* it = root->FirstChild();
    for (;!it->IsEnd(); it->next())
    {
      GatherStatistic(it->value());
    }
    delete it;
  }
}

const string TrieStatistic::ToString(){
  stringstream str_stream;
  str_stream << "#Node: "<< num_node << " #leaf: " << num_leaf
             << " max_funout: " << max_funout << endl;
  for (int i = 0; i < max_funout; i ++ ){
    str_stream << " # " << i << " funout: " << funout[i] <<endl;
  }
  str_stream << endl;
  
  for (map<int,int>::iterator it = num_results.begin();
       it != num_results.end(); it ++ )
  {
    str_stream << " #results " << it->first << " count: " << it -> second  <<endl;
  }      
  return str_stream.str();
}

};












