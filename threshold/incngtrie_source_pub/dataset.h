// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __DATASET_H__
#define __DATASET_H__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace dbwsim {

//const int kMaxDataLength = 256;

class RawDataSet {
 public:
  // Empty initialize.
  RawDataSet () {}
  
  // Initialize Dataset from file reading.
  RawDataSet (const string& filename, int max_length = 256);
  
  // Create a RawDataSet from a list of strings.
  RawDataSet (vector<string>& input_strings, int max_length = 256);

  // Init with file.
  void InitFromFile(const string& filename, int max_length = 256);

  // Init with strings.
  void InitFromStrings(vector<string>& input_strings, int max_length = 256);
  
  // By default, the ID is the ranking of string in.
  // the data list. 
  const string& GetDocumentByID(int id);

  // Dump all strings with its id;
  void DumpDataset(ostream& out = cout);
  
  ~RawDataSet (){};
  
  //private:  
  vector<string> documents_;
  int num_documents_;
  int max_length_;
  double average_length_;
};

}

#endif // __DATASET_H__
