// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "dataset.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

namespace dbwsim {


void RawDataSet::InitFromFile(const string& filename,
                              int max_length)
{
  int total_length = 0;
  ifstream infile;
  infile.open(filename.c_str());
  max_length_ = max_length;
  char *line = new char[max_length_];
  infile.getline(line, max_length_);
  num_documents_ = 0;
  documents_.clear();
  while (infile.good()){
    string linestr = line;
    documents_.push_back(linestr);
    num_documents_ ++;
    total_length += linestr.length();
    infile.getline(line, max_length_);    
  }
  average_length_ = num_documents_ > 0 ? total_length/num_documents_ : 0;
  sort(documents_.begin(), documents_.end());
  delete line;
}

void RawDataSet::InitFromStrings(vector<string>& input_strings,
                                 int max_length)
{
  int total_length = 0;
  max_length_ = max_length;
  num_documents_ = 0;
  vector<string>::iterator it;  
  for (it = input_strings.begin(); it < input_strings.end(); it ++)
  {
    documents_.push_back(*it);
    num_documents_ ++;
    total_length = (*it).length();
  }
  average_length_ = num_documents_ > 0 ? total_length/num_documents_ : 0;
  sort(documents_.begin(), documents_.end()); 
}

RawDataSet::RawDataSet(const string& filename, int max_length)
{
  InitFromFile(filename, max_length);
}

RawDataSet::RawDataSet(vector<string>& input_strings, int max_length)
{
  InitFromStrings(input_strings, max_length);  
}

void RawDataSet::DumpDataset(ostream& out)
{
  int id = 0;
  for (vector<string>::iterator it = documents_.begin();
       it != documents_.end(); it ++)
  {
    out << id++ << ": " << *it << endl;
  }
}


const string& RawDataSet::GetDocumentByID(int id){
  return documents_[id];
}

}
