// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "variants.h"
#include "util.h"

#include <string>
#include <algorithm>

using namespace std;

namespace dbwsim {


int VariantGenerator::GenerateVariantsFixTau(const string& str, vector<string>& variants, int tau)
{
  if (num_comb_ < (int)str.length()) {
    num_comb_ = str.length() * 2; // double the size. 
    delete comb_;
    comb_ = new int [num_comb_];
  }   
  for (int i = 0; i < (int)str.length(); i ++){
    comb_[i] = i;
  }
  if (tau > (int)str.length())
    tau = str.length();
  
  variants.push_back(str);
  int count = 1;
  for (int t = 1; t <= tau; t++)
  {
    sort (comb_, comb_ + str.length());

    do {
      string tmpstr = str;
      for (int i = 0; i < t; i ++) {
        tmpstr[comb_[i]] = DELETION_NOTE;
      }
      count ++;
      variants.push_back(tmpstr);
    } while (next_combination(comb_, comb_ + t, comb_ + str.length()));        
  }
  return count;
}

int VariantGenerator::GenerateVariants(const string& str, vector<string>& variants)
{
  return GenerateVariantsFixTau(str, variants, tau_);
}

int VariantGenerator::GenerateVariantsPerGap(const string& str, vector<string>& variants, int gap)
{  
  int tau = str.length() / gap;
  if (tau > tau_)
    tau = tau_;
  return GenerateVariantsFixTau(str, variants, tau); 
}

}
