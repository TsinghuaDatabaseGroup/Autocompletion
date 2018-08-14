// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#include "util.h"

#include <iostream>
#include <openssl/md5.h>

using namespace std;
namespace dbwsim { 

int edit_one_char(const char *data, int data_len, char query_char,
                  int query_pos, int* mat_in, int* mat_out, int tau)
{
  int data_pos, mat_pos;
  int min_tau = tau + 1;
  for (int i = -tau; i <= tau; i ++) {
    data_pos = query_pos + i;
    mat_pos = i + tau;
    if (data_pos < -1){
      mat_out[mat_pos] = tau + 1;
    } else if (data_pos >= data_len){
      while ( i <= tau) {
        mat_pos = i + tau;
        mat_out[mat_pos] = tau + 1;
        i ++;
      }
      break;
    } else {
      if (query_char == data[data_pos]){
        mat_out[mat_pos] = mat_in[mat_pos];        
      } else {
        mat_out[mat_pos] = MIN3(mat_pos > 0? mat_out[mat_pos-1]: tau,
                                mat_in[mat_pos],
                                mat_pos < 2 * tau ? mat_in[mat_pos + 1] : tau) + 1;
      }
    }
    if (mat_out[mat_pos] < min_tau)
      min_tau = mat_out[mat_pos];    
  }
  return min_tau;
}

int edit_verify(const char *data, int data_len, const char *query, int query_len, int tau)
{
  int  row = 0;
  int  mat[2][10];
  int  min_tau = tau + 1;
  
  for (int i = -tau; i <= tau + 1; i++){
    int mat_pos = i + tau;
    if (i < 0){
      mat[row][mat_pos] = tau + 1;
    }else {
      mat[row][mat_pos] = i;
    }
  }
  
  for (int i = 0; i < query_len; i++, row = !row) {
    min_tau = edit_one_char(data, data_len, query[i], i, mat[row], mat[!row], tau);
    if (min_tau > tau) break;    
  }
  return min_tau;
}

int edit_verify_debug(const char *data, int data_len, const char *query, int query_len, int tau)
{
  int  row = 0;
  int  mat[2][10];
  int  min_tau = tau + 1;
  
  for (int i = -tau; i <= tau + 1; i++){
    int mat_pos = i + tau;
    if (i < 0){
      mat[row][mat_pos] = tau + 1;
    }else {
      mat[row][mat_pos] = i;
    }
  }

  for ( int m = 0; m <= tau + 1; m ++) {
    cout << "   ";
  }
  for ( int m = 0; m < data_len; m ++) {
    cout << data[m] << "  ";
  }
  cout <<endl;
  cout << "   ";
  int i;
  for (i = 0; i < query_len; i++, row = !row) {
    min_tau = edit_one_char(data, data_len, query[i], i, mat[row], mat[!row], tau);
    for ( int m = 0; m < i; m ++) {
      cout << "   ";
    }
    for (int j = 0; j < 2 * tau + 1; j ++){
      cout << mat[row][j] << "  ";
    }
    cout << endl << query[i] << "  ";
    if (min_tau > tau) break;    
  }
  for ( int m = 0; m < (i >= query_len?query_len: i + 1) ; m ++) {
    cout << "   ";
  }      
  for (int j = 0; j < 2 * tau + 1; j ++){
    if (i>=query_len)
      cout << mat[row][j] << "  ";
    else
      cout << mat[!row][j] << "  ";
  }
  cout <<endl <<endl;
  return min_tau;
}

uint64_t create_sign_md5(const string& str)
{
  unsigned int md5res[4];
  uint64_t sign;
  MD5((unsigned char*)str.c_str(),(unsigned int)str.length(),(unsigned char*)md5res);
  sign = md5res[0] + md5res[1];
  sign = (sign << 32) + (md5res[2] + md5res[3]);
  return sign;
}



} // dbwsim
