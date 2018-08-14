// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "vector_trie.h"
#include "util.h"

using namespace std;

namespace dbwsim{

BaseIter* VectorTrie::FirstChild() {
  return new VectorIter(this);
}

BaseIter* VectorTrie::EndChild() {
  return new VectorIter(this);
}

TrieBase* VectorTrie::SearchChar(const char chr)
{
  int pos = SearchCharLB(chr);
  if (pos >= 0 && pos < (int)children->size() && children->at(pos)->key == chr){
    return children->at(pos);
  }
  return NULL;
}

int VectorTrie::SearchCharLB(const char chr)
{
  if (children == NULL) return -1; // Empty
  int left = 0, right = children->size(), mid;
  while (left < right) {    
    mid = (left + right) >> 1;
    if (children->at(mid)->key < chr){
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return left;
}

TrieBase* VectorTrie::SearchString(const char *str)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return this;
  // Empty tree with no nodes in here. 
  if (num_children == 0) return NULL;

  // Initialize.
  VectorTrie* node = this;
  int pos = 0;

  // Repeat to find answer.
  while (str[pos] != '\0')
  {
    // Search for current code.
    VectorTrie* searchnode = (VectorTrie*)node->SearchChar(str[pos]);
    if (searchnode == NULL)
      return NULL;
    node = searchnode;
    pos ++;
  }
  return node;
}

int VectorTrie::AddString(const char *str, int pid, int did)
{
  // Empty String or length zero string.
  if (str == NULL || *str == '\0') return 0;

  // number of new nodes
  int num_new_nodes = 0;

  // Initialize.
  VectorTrie *node = this;
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
    int searchpos = node->SearchCharLB(str[pos]);

    if (searchpos >= 0 && searchpos < (int)node->children->size() &&
        node->children->at(searchpos)->key == str[pos]){
      node = node->children->at(searchpos);
    } else {
      VectorTrie* newnode = new VectorTrie(depth + 1, str[pos]);
      node->num_children ++;
      num_new_nodes ++;

      if (searchpos < 0){
        node->children = new vector<VectorTrie* >();
      }
      
      if (searchpos < 0 || searchpos >= (int)node->children->size()){
        node->children->push_back(newnode);
      }else{
        //node->children->push_back(newnode);
        node->children->insert(node->children->begin() + searchpos, newnode);
      }
      node = newnode;
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

void VectorTrie::AddDocId(int did)
{
  if (did == -1)
    return;
  if (docids == NULL){
    docids = new vector<int>();
  }
  docids->push_back(did);
}

void VectorTrie::BuildTrie(int num_str, const char **strs, int *ids) {
  for (int i = 0; i < num_str; i++){
    if (ids == NULL)
      AddString(strs[i], -1);
    else
      AddString(strs[i], ids[i]);
  }
}

uint64_t VectorTrie::HashFunction(vector<uint64_t>* subhash, vector<int>* docids)
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

uint64_t VectorTrie::HashSubtree(bool print, int *label, int *vars){
  vector<uint64_t> subhash;
  int oldlabel = 0, oldvars = 0;
  if (label != NULL)
    oldlabel = *label;
  if (vars != NULL)
    oldvars = *vars;

  VectorTrie* child;
  uint64_t sign;

  if (children != NULL) {
    for(vector<VectorTrie* >::iterator it = children->begin();
        it != children->end(); it++ ){
      child = *it;
      sign = child->HashSubtree(print, label, vars);
      subhash.push_back(sign);    
    }
  }

  sign = HashFunction(&subhash, docids);

  if (docids!=NULL) {
    //cout << *vars << endl;
    (*vars) = (*vars) + docids->size();
  }

  if(print) {
    //fprintf(stdout, "Sign "
    cout << "Sign: " << sign << dec << " ";
    cout << "Depth: " << depth << " ";
    //cout << "Result: " << num_results << " ";
    if (vars != NULL) {
      cout << "Vars: " << (*vars) - oldvars << " " ;
    }
    if (label != NULL){
      (*label) ++;
      cout << "Node_From: " << oldlabel << " To: " << *label
           << " Contain: " << (*label) - oldlabel << " ";      
    }
    cout << endl;
  }  
  return sign;
}

}
