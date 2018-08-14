// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __UTIL_H__
#define __UTIL_H__

#include <algorithm>
#include <string>

using namespace std;
namespace dbwsim { 

template <typename Iterator>
inline bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
  if ((first == last) || (first == k) || (last == k))
    return false;
  Iterator itr1 = first;
  Iterator itr2 = last;
  ++itr1;
  if (last == itr1)
    return false;
  itr1 = last;
  --itr1;
  itr1 = k;
  --itr2;
  while (first != itr1)
  {
    if (*--itr1 < *itr2)
    {
      Iterator j = k;
      while (!(*itr1 < *j)) ++j;
      std::iter_swap(itr1,j);
      ++itr1;
      ++j;
      itr2 = k;
      std::rotate(itr1,j,last);
      while (last != j)
      {
        ++j;
        ++itr2;
      }
      std::rotate(k,itr2,last);
      return true;
    }
  }
  std::rotate(first,k,last);
  return false;
}


#define MAX2(a, b) (((a) > (b)) ? (a) : (b))
#define MAX3(a, b, c) (MAX2(MAX2((a),(b)),(c)))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MIN3(a, b, c) (MIN2(MIN2((a),(b)),(c)))
#define ABS(a)   (((a) < 0) ? -(a) : (a))


// Edit distance calculation for one character. 
int edit_one_char(const char *data, int data_len, char query_char,
                  int query_pos, int* mat_in, int* mat_out, int tau);

// Edit distance varification. 
int edit_verify(const char *data, int data_len, const char *query, int query_len, int tau);

int edit_verify_debug(const char *data, int data_len, const char *query, int query_len, int tau);


#define hash32(x) (((x)*2654435761))
#define hash64(x) (((x)*265512412435761))

uint64_t create_sign_md5(const string& str);

}

#endif // __UTIL_H__
