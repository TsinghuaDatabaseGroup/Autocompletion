// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "brother_trie.h"
#include "util.h"

using namespace std;

namespace dbwsim{

BaseIter* BrotherTrie::FirstChild() {
  return new BrotherIter(this);
}

BaseIter* BrotherTrie::EndChild() {
  return new BrotherIter(this);
}

TrieBase* BrotherTrie::SearchChar(const char chr)
{
  if(num_children > 0){
    BrotherTrie* child = firstchild;
    while (child != NULL){
      if (child->key == chr){
        return child;
      } else if (child->key > chr){
        break;
      }
      child = child->brother;
    }
  }
  return NULL;
}

BrotherTrie* BrotherTrie::SearchCharLB(const char chr)
{
  BrotherTrie* child = firstchild;
  BrotherTrie* previous = NULL;    
  if(num_children > 0){
    while (child != NULL){
      if (child->key == chr){
        return child;
      } else if (child->key < chr){
        previous = child;
        child = child->brother;
      } else {
        break;
      }
    }    
  }
  return previous;
}

TrieBase* BrotherTrie::SearchString(const char *str)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return this;
  // Empty tree with no nodes in here. 
  if (num_children == 0) return NULL;

  // Initialize.
  BrotherTrie* node = this;
  int pos = 0;

  // Repeat to find answer.
  while (str[pos] != '\0')
  {
    // Search for current code.
    BrotherTrie* searchnode = (BrotherTrie*) node->SearchChar(str[pos]);
    if (searchnode == NULL)
      return NULL;
    node = searchnode;
    pos ++;
  }
  return node;
}

int BrotherTrie::AddString(const char *str, int pid, int did)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return 0;

  // number of new nodes
  int num_new_nodes = 0;

  // Initialize.
  BrotherTrie *node = this;
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
    BrotherTrie* searchnode = node->SearchCharLB(str[pos]);
    
    if (searchnode == NULL) {      
      // Need to create a node under this node.
      BrotherTrie* newnode = new BrotherTrie(depth + 1, str[pos]);
      node->num_children ++;
      num_new_nodes ++;
      newnode->brother = node->firstchild;
      node->firstchild = newnode;
      node = newnode;
    } else if (searchnode->key != str[pos]){
      // Need to create a node for new place.
      BrotherTrie* newnode = new BrotherTrie(depth + 1, str[pos]);
      node->num_children ++;
      num_new_nodes ++;
      newnode->brother = searchnode->brother;
      searchnode->brother = newnode;
      node = newnode;
    } else {
      node = searchnode;      
    }
    pos ++;
    depth ++;
    node->UpdatePositionRange(pid);
    node->UpdateNumResults(last_del);    
  }
  node->leaf_count ++;
  node->AddDocId(did);
  return num_new_nodes;
}

void BrotherTrie::AddDocId(int did){
  if (did == -1)
    return;
  if (docids == NULL){
    docids = new vector<int>();
  }
  docids->push_back(did);
}

void BrotherTrie::BuildTrie(int num_str, const char **strs, int *ids) {
  for (int i = 0; i < num_str; i++){
    if (ids == NULL)
      AddString(strs[i], -1);
    else
      AddString(strs[i], ids[i]);
  }
}


uint64_t BrotherTrie::HashFunction(vector<uint64_t>* subhash, vector<int>* docids)
{
  stringstream str_stream;  
  str_stream << hex << hash32(key);  
  if (subhash != NULL) {
    for (vector<uint64_t>::iterator it = subhash->begin();
         it != subhash->end(); it ++) {
      str_stream << hex << (*it);
    }
  }

  if (docids != NULL) {
    for (vector<int>::iterator it = docids->begin();
         it != docids->end(); it ++) {
      str_stream << hex << (*it);
    }
  }
  return create_sign_md5(str_stream.str());
}

int BrotherTrie::CountSaving(int *vars_save, int *nodes_save){
  int did = -1;
  (*nodes_save) ++;
  (*vars_save) += leaf_count;
  BrotherTrie* child = firstchild;
  while (child != NULL){
    int sign = child->CountSaving(vars_save, nodes_save);
    if (sign != -1) did = sign;
    child = child->brother;
  }
  if (did == -1){
    if (leaf_count > 0){
      did = docids->at(0);
    }
  }
  return did;
}

void BrotherTrie::StringMerge(int *vars_save, int *nodes_save, bool del){
  if (num_results == 1){
    // Cut the subtree and count numbers.
    //int loc_vars_save = 0;
    //int loc_nodes_save = 0;
    //int did = CountSaving(&loc_vars_save, &loc_nodes_save);
    int did = CountSaving(vars_save, nodes_save);
    (*vars_save)--;
    (*nodes_save)--;

    if (del) {
      if (firstchild != NULL)
        delete firstchild;
      firstchild = NULL;
      num_children = 0;
      leaf_count = 1;
      if (docids != NULL){
        docids->clear();
      }
      AddDocId(did);      
    }

  } else {
    BrotherTrie* child = firstchild;
    while (child != NULL){
      child->StringMerge(vars_save, nodes_save, del);
      child = child->brother;
    }
  }
}

int BrotherTrie::SubtreeMerge(int *vars_save, int *nodes_save){
  vector<node_sign_tuple> sign_tuples;
  map<uint64_t, int> exist_sign;
  int label= 0;
  int vars = 0;
  HashSubtree(false, &label, &vars, &sign_tuples);
  int covered = label + 100;
  vector<node_sign_tuple>::reverse_iterator rit;
  for(rit = sign_tuples.rbegin();
      rit != sign_tuples.rend(); ++rit)
  {
    if (covered > (*rit).label) {
      map<uint64_t, int>::iterator it =
          exist_sign.find((*rit).sign);
      if (it == exist_sign.end()){
        exist_sign[(*rit).sign] ++;
      } else {
        (*vars_save) += (*rit).vars;
        (*nodes_save) += (*rit).label - (*rit).from;
        covered = (*rit).from;
      }
    }
  }
  return exist_sign.size();
}

uint64_t BrotherTrie::HashSubtree(bool print, int *label, int *vars,
                                  vector<node_sign_tuple>* sign_tuples)
{
  vector<uint64_t> subhash;
  int oldlabel = 0, oldvars = 0;
  if (label != NULL)
    oldlabel = *label;
  if (vars != NULL)
    oldvars = *vars;
  
  BrotherTrie* child = firstchild;
  uint64_t sign;
  while (child != NULL){
    sign = child->HashSubtree(print, label, vars, sign_tuples);
    subhash.push_back(sign);
    child = child->brother;
  }
  sign = HashFunction(&subhash, docids);

  if (docids!=NULL) {
    //cout << *vars << endl;
    (*vars) = (*vars) + leaf_count;
  }

  if (label != NULL)
    (*label) ++;

  if (sign_tuples!=NULL){
    node_sign_tuple tuple;
    tuple.sign = sign;
    tuple.from = oldlabel;
    tuple.label = (*label);
    tuple.vars = (*vars) - oldvars;
    sign_tuples->push_back(tuple);
  }    

  if(print) {
    //fprintf(stdout, "Sign "
    cout << "Sign: " << sign << dec << " ";
    cout << "Depth: " << depth << " ";
    cout << "Result: " << num_results << " ";
    if (vars != NULL) {
      cout << "Vars: " << (*vars) - oldvars << " " ;
    }
    if (label != NULL){
      cout << "Node_From: " << oldlabel << " To: " << *label
           << " Contain: " << (*label) - oldlabel << " ";      
    }
    cout << endl;
  }
  return sign;
}

}
