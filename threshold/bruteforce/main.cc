/*

  Copyright (C) 2011 by The Department of Computer Science and
  Technology, Tsinghua University

  Redistribution of this file is permitted under the terms of
  the BSD license.

  Author   : Dong Deng
  Created  : 2014-09-05 11:42:44 
  Modified : 2014-09-12 14:09:21
  Contact  : dd11@mails.tsinghua.edu.cn

*/

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

// const string delims = " ,.\t.";
vector<string> recs;
vector<string> queries;

int min(int a, int b, int c) {
  if (a <= b && a <= c) return a;
  if (b <= c && b <= a) return b;
  return c;
}

void readData(string& filename, vector<string>& recs) {
  string str;
  ifstream input(filename, ios::in);
  while (getline(input, str)) {
    for (auto i = 0; i < str.length(); i++)
      str[i] = tolower(str[i]);
    recs.push_back(str);
  }
}

int main(int argc, char ** argv) {
  int maxPrefix = 1000;
  int result_num[maxPrefix];
  int query_num[maxPrefix];

  string filename = string(argv[1]);
  string queryfile = string(argv[2]);
  int tau = atoi(argv[3]);

  readData(filename, recs);
  readData(queryfile, queries);

  vector<int*> mat[2];
  bool flap = false;
  for (auto i = 0; i < recs.size(); i++) {
    int* row = new int[recs[i].length() + 1];
    int* row_flap = new int[recs[i].length() + 1];
    mat[flap].push_back(row);
    mat[!flap].push_back(row_flap);
  }

  for (auto i = 0; i < queries.size(); i++) {
    /*printf("============================\n");
    printf("%s\n", queries[i].c_str());
    printf("============================\n");*/
    bool isRes[recs.size()];
    memset(isRes, 1, recs.size() * sizeof(bool));
    flap = false;
    for (auto i = 0; i < recs.size(); i++) {
      for (auto j = 0; j < recs[i].length() + 1; j++) 
        mat[flap][i][j] = j;
    }
    for (auto j = 1; j <= queries[i].length(); j++) {
      //printf("%15s  ", queries[i].substr(0, j).c_str());
      int res_count = 0;
      timeval start, term;
      gettimeofday(&start, NULL);
      char ch = queries[i][j - 1];
      for (auto k = 0; k < recs.size(); k++) {
        if (isRes[k] == false) continue;
        mat[!flap][k][0] = mat[flap][k][0] + 1;
        int mined = mat[!flap][k][0];
        int pos = 0;
        for (auto m = 1; m <= recs[k].length(); m++) {
          mat[!flap][k][m] = min(mat[flap][k][m] + 1, mat[!flap][k][m - 1] + 1,
                                 mat[flap][k][m - 1] + (ch == recs[k][m - 1] ? 0 : 1));
          if (mined > mat[!flap][k][m]) {
            mined = mat[!flap][k][m];
            pos = m;
          }
        }
        if (mined <= tau) {
          ++res_count;
          /*
          printf("\x1b[31m%s\x1b[0m", recs[k].substr(0, pos).c_str());
          printf("%s\n", recs[k].substr(pos).c_str());
          */
        } else {
          isRes[k] = false;
        }
      }
      flap = !flap;
      gettimeofday(&term, NULL);
	  query_num[j]++;
	  result_num[j] += res_count;
      //printf("Time: %ld  Result Num: %d \n", (term.tv_sec - start.tv_sec) * 1000000 +
      //    (term.tv_usec - start.tv_usec), res_count);
    }
  }

  int idx = 1;
  while (result_num[idx] > 0) {
	int num = query_num[idx];
	printf("%d %d %d %d\n", idx, query_num[idx], result_num[idx] / num, result_num[idx]);
	++idx;
  }
  
  return 0;
}
