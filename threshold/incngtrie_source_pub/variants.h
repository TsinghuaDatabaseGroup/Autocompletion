// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __VARIANTS_H__
#define __VARIANTS_H__

#include<string>
#include<vector>

#define DELETION_NOTE '~'

// class Variant{
// public:
//   string variant_str;
//   int last_del;
  
//   // Using a variants to initialize a variants. 
//   Variant(const string& var)
//   {
//     variant_str.copy(var);
//     size_t found = variant_str.rfind(DELETION_NOTE);
//     if (found == string::npos){
//       last_del = -1;
//     } else {
//       last_del = found;
//     }
//   }  

//   Variant(const string& var, int ndel, vector<int>& dels ) {
//     variant_str.copy(var);
//     vector<int>::iterator it;    
//     for (it = dels.begin(); it < dels.end(); it++)
//       if (*it < variant_str.length())
//         variant_str[*it] = DELETION_NOTE;
//     size_t found = variant_str.rfind(DELETION_NOTE);
//     if (found == string::npos){
//       last_del = -1;
//     } else {
//       last_del = found;
//     }
//   }  
//   ~Variant(){}
// }

using namespace std;

namespace dbwsim {

class VariantGenerator {
 private:
  int *comb_;
  int num_comb_;
  int tau_;
  int num_comb;
  int comb[5000][5];  
  
 public:

  VariantGenerator (int tau = 3){
    tau_ = tau;
    num_comb_ = 256;
    comb_ = new int[num_comb_];

    
    
  }

  ~VariantGenerator(){
    delete comb_;
  }

  int tau(){return tau_;}

  void set_tau(int tau){tau_ = tau;};
  
  // Given a string and an vector. return all possible variants.
  int GenerateVariants (const string& str, vector<string>& variants);

  // Given a string and an vector. return all possible variants.
  int GenerateVariantsPerGap (const string& str, vector<string>& variants, int gap = 4);

  // Given a string and an vector. return all possible variants.
  int GenerateVariantsFixTau (const string& str, vector<string>& variants, int tau = 3);

  
};

}

#endif // __VARIANTS_H__
