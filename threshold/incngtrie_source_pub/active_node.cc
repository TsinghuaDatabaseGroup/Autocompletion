// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

#include "active_node.h"
//#include "naive_trie.h"

using namespace std;

namespace dbwsim {

const char* StateName[] = {
  "Active", "InActive", "Extended", "Filtered", "Covered"
};

const string ActiveNode::ToString(){
  stringstream str_stream;
  str_stream << "#Rsts: "  << trie_node->num_results;
  str_stream << "\tStat: " << StateName[node_state];
  str_stream << "\tFech: " << is_fetched;
  str_stream << "\tEdit: " << (int) node_ed;
  str_stream << "\tiEdit: " << (int) inode_ed;
  str_stream << "\tExp: "  << (int) key_match;
  str_stream << "\tExt: "  << (int) document_id;
  str_stream << "\tPfx: \"" << node_str << "\"";
  return str_stream.str();    
}

ActiveNodePool::ActiveNodePool(){
  CleanUpPool();
  IncreasePoolSize();  
}

void ActiveNodePool::IncreasePoolSize() {
  for (int i = 0; i < kACTIVENODE_POOL_SIZE; i++) {
    node_pool_.push(new ActiveNode);
  }
}

void ActiveNodePool::CleanUpPool() {
  while (!node_pool_.empty()) {
    delete node_pool_.front();
    node_pool_.pop();    
  }    
}

ActiveNode* ActiveNodePool::GetActiveNode(){
  if (node_pool_.empty())
    IncreasePoolSize();
  ActiveNode *nodep = node_pool_.front();
  node_pool_.pop();
  nodep->Clear();
  return nodep;
}

void ActiveNodePool::ReturnActiveNode(ActiveNode* nodep) {
  if (nodep!=NULL)
    node_pool_.push(nodep);
}

ActiveNodePool::~ActiveNodePool(){
  CleanUpPool();
}

} // dbwsim





