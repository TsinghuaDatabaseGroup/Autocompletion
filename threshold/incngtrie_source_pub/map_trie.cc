// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "map_trie.h"

using namespace std;

namespace dbwsim{

BaseIter* MapTrie::FirstChild() {
  return new MapIter(this);
}

BaseIter* MapTrie::EndChild() {
  return new MapIter(this);
}

TrieBase* MapTrie::SearchChar(const char chr)
{
  if(num_children > 0){
    map<char, MapTrie*>::iterator it = children->find(chr);
    if (it != children->end())
      return it->second;
  }
  return NULL;
}

TrieBase* MapTrie::SearchString(const char *str)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return this;
  // Empty tree with no nodes in here. 
  if (children == NULL) return NULL;

  // Initialize.
  map<char, MapTrie*>* subtrees = this->children;
  MapTrie* node = this;
  int pos = 0;

  // Repeat to find answer.
  while (str[pos] != '\0')
  {
    // Search for current code.
    map<char, MapTrie*>::iterator it = subtrees->find(str[pos]);
    if (it == subtrees->end())
    {
      // Not found. Return NULL;
      return NULL;
    }
    else
    {
      // Find one matching Node.
      node = it->second;
      subtrees = node->children;
          
      // Check if children is NULL and query not ended.
      if (subtrees == NULL && str[pos+1] != '\0')
        return NULL;
    }
    // Increase pos. 
    pos++;
  }
  // Find a result.
  return node;
}
  
int MapTrie::AddString(const char *str, int pid, int did)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return 0;
  // Empty tree with no nodes in here.
  if (num_children == 0)
  {
    // Create a empty subtrie map.
    children = new map<char, MapTrie*>();
  }
  // number of new nodes
  int num_new_nodes = 0;

  // Initialize.
  map<char, MapTrie*> *subtrees = this->children;
  MapTrie *node = this;
  int pos = 0;
  int depth = this->depth;

  int last_del = -1;
  const char *last_del_char = strrchr(str, DELETION_NOTE);
  if (last_del_char != NULL){
    last_del = last_del_char - str;
  }
    
  node->UpdatePositionRange(pid);
  node->UpdateNumResults(last_del);
     
  // Repeat to find answer.
  while (str[pos] != '\0')
  {                
    // Search for current code. 
    map<char, MapTrie*>::iterator it = subtrees->find(str[pos]);
    if (it == subtrees->end())
    {
      // No node find. Add a new node. 
      MapTrie *new_node = 
          new MapTrie(depth + 1, str[pos]);          
      (*subtrees)[str[pos]] = new_node;
      node->num_children ++;
      num_new_nodes ++;
      node = new_node; 
    }
    else
    {
      // Find a node.
      node = it->second;
    }
        
    subtrees = node->children;
        
    // Check if next key is over.
    if (subtrees == NULL && str[pos+1] != '\0')
    {
      // Create a new subtree.
      subtrees = new map<char, MapTrie*>();
      node->children = subtrees;
    }

    node->UpdatePositionRange(pid);
    node->UpdateNumResults(last_del);

    depth ++;
    pos ++;
  }
  node->leaf_count = true;
  if ( did != -1 ){
    
  }
  
  return num_new_nodes;        
}

void MapTrie::BuildTrie(int num_str, const char **strs, int *ids) {
  for (int i = 0; i < num_str; i++){
    if (ids == NULL)
      AddString(strs[i], i, -1);
    else
      AddString(strs[i], i, ids[i]);
  }
}

}

