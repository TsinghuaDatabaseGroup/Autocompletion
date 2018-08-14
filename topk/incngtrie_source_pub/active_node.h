// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __ACTIVE_NODE_H__
#define __ACTIVE_NODE_H__

#include "trie_base.h"
#include <queue>
#include <iostream>
#include <sstream>

using namespace std;
namespace dbwsim {

#define kACTIVENODE_POOL_SIZE 1024*1024
#define kPREFIX_LEN 20

enum ActiveNodeState {
  eActive   = 0,
  eInActive = 1,
  eExtended = 2,
  eFiltered = 3,
  eCovered  = 4,
};

class ActiveNode{

 public:
  // Associated trienode.
  TrieBase *trie_node;

  int document_id;
  
  // The string of the node.
  char node_str[kPREFIX_LEN];

  // The length of the string. aka the depth.
  char node_len;

  // Match location. In fastss. It means
  // Expected. In pivotal, it is the last match.
  char key_match;

  // The edit distance of current node.
  char node_ed;

  // The edit distance of preview match. 
  char inode_ed;

  // The state of curetn node. 
  ActiveNodeState node_state;
  
  // If this node is used to fetch the result.
  bool is_fetched;

  ActiveNode(){}

  void Clear(){
    trie_node = NULL;
    node_str[0] = '\0';
    node_len = 0;
    key_match = -1;
    node_state = eActive;
    is_fetched = false;
    key_match = -1;
    node_ed = 0;
    inode_ed = 0;
    document_id = 0;
  }

  // Check Cover is somthing very interesting.
  // First if A cover B means A have more ~ than B
  // And A's other characters Align with B's
  // There are three state in this contditon.
  // return 1 means this cover node.
  // return 0 means this inrelevant node.
  // return -1 means this un-cover node. 
  int CheckCover(ActiveNode* node)
  {
    if (node_len != node->node_len)
      return -1;
    int cover = 1;
    for (int i = 0; i < node_len; i++){
      if (node_str[i] != DELETION_NOTE) {
        if (node_str[i] != node->node_str[i]){
          if (node->node_str[i] == DELETION_NOTE)
            cover = 0;
          else
            return -1;
        }
      }
    }
    return cover;
  }

  // Print out the node.
  const string ToString();  
};

class ActiveNodePool{
  queue<ActiveNode*> node_pool_;
  
  void CleanUpPool();

  void IncreasePoolSize();
  
 public:
  
  // Initializer to create Factory.
  ActiveNodePool();
  
  // Deconstructor
  ~ActiveNodePool();
  
  // Method create ActiveNode;
  ActiveNode* GetActiveNode();
  
  // Destory ActiveNode;
  void ReturnActiveNode(ActiveNode* node);

  void PrintStatistic(){
    cout << node_pool_.size() << endl;    
  }
};

}

#endif // __ACTIVE_NODE_H__
