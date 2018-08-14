/*

  Copyright (C) 2011 by The Department of Computer Science and
  Technology, Tsinghua University

  Redistribution of this file is permitted under the terms of
  the BSD license.

  Author   : Dong Deng
  Created  : 2014-09-05 11:42:44 
  Modified : 2014-09-28 15:18:13
  Contact  : dd11@mails.tsinghua.edu.cn

*/

#include "util.h"
#include "trie.h"
#include <sys/time.h>
#include <stdlib.h>

vector<string> recs;
vector<string> queries;


void printResults(string & query, int pos, unordered_map<int, pair<int,int> > &results);

bool comp_node(const TrieNode* t1, const TrieNode* t2) {
  if (t1->id < t2->id) 
    return true;
  else if (t1->id > t2->id)
    return false;
  else
    return t1->last > t2->last;
}

bool comp_lower(const pair<int, int> & p, const int v) {
  return p.first < v;
}

bool comp_id(const TrieNode* n, const int id) {
  return n->id < id;
}

int main(int argc, char ** argv) {

  int maxPrefix = 1000;
  double search_time[maxPrefix];
  double fetch_time[maxPrefix];
  int match_num[maxPrefix];
  int result_num[maxPrefix];
  int query_num[maxPrefix];
  int bs_num[maxPrefix];

  string filename = string(argv[1]);
  string queryfile = string(argv[2]);
  int tau = atoi(argv[3]);

  readData(filename, recs);
  readData(queryfile, queries);

  // construct trie index
  cerr << "#  Construct Index :" << endl;
  Trie* trie = new Trie();
  for (auto i = 0; i < recs.size(); i++)
    trie->append(recs[i].c_str(), i);
  trie->buildIdx();
  trie->bfsIndex();

  timeval start, middle, term;

  for (auto i = 0; i < queries.size(); i++) {
    unordered_map<TrieNode*, deque<pair<int, int>>> agg; // the previous matches  <row, ed>
    unordered_map<TrieNode*, pair<int, int>> matches;    // the current matches  <col, ed>
    agg[trie->root].push_back(make_pair(0, 0));          // initialize agg with  <root, 0, 0>

    for (auto j = 1; j <= queries[i].length(); j++) {

      gettimeofday(&start, NULL);

      matches.clear();
      char ch = queries[i][j - 1];     // current key stroke
      for (auto it = agg.begin(); it != agg.end(); ++it) {   // foreach <node, list<row, ed>>
        for (int count = 0; count <= tau; ++count) { // roll down count levels
          int depth = it->first->depth + count + 1;
          vector<TrieNode*> &vec = trie->index[ch][depth];
          auto vit = lower_bound(vec.begin(), vec.end(), it->first->id, comp_id);
          bs_num[j]++;
          while (vit != vec.end() && (*vit)->id <= it->first->last) {
            // select the minimum one
            int themin = tau + 1;
            for (auto qit = it->second.begin(); qit != it->second.end(); ++qit) {  // <row, ed>
              if (themin > qit->second + max(j - 1 - qit->first, count))
                themin = qit->second + max(j - 1 - qit->first, count);
            }
            if (themin <= tau) {
              auto mit = matches.find(*vit);
              if (mit == matches.end() || mit->second.second > themin)
                matches[*vit] = make_pair(j, themin);
            }
            ++vit;
          }
        }
      }

      // begin update agg
      // add in new matches
      for (auto mit = matches.begin(); mit != matches.end(); mit++)
        agg[mit->first].push_back(mit->second);

      // remove old matches
      for (auto ait = agg.begin(); ait != agg.end(); ) {
        int pop_num = 0;
        for (auto qit = ait->second.begin(); qit != ait->second.end(); ++qit) { 
          if (j - qit->first + qit->second > tau) ++pop_num;
          else break;
        }
        if (pop_num == ait->second.size()) {
          ait = agg.erase(ait);
        } else {
          while (--pop_num >= 0) ait->second.pop_front();
          ait++;
        }
      }
      // end update agg 

      gettimeofday(&middle, NULL);

      // begin output results
      vector<TrieNode*> m_nodes;
      for (auto ait = agg.begin(); ait != agg.end(); ait++)
        m_nodes.push_back(ait->first);
      sort(m_nodes.begin(), m_nodes.end(), comp_node);
      vector<int> results;
      int prev_last = -1;
      auto tit = trie->ids.begin();
      for (auto vit = m_nodes.begin(); vit != m_nodes.end(); vit++) {
        if ((*vit)->last <= prev_last) continue;
        prev_last = (*vit)->last;
        tit = lower_bound(tit, trie->ids.end(), (*vit)->id, comp_lower);
        while (tit != trie->ids.end() && tit->first <= (*vit)->last) {
          results.push_back(tit->second);
          ++tit;
        }
      }
      // printResults(queries[i], j, results);
      // end output results
      
      gettimeofday(&term, NULL);

      query_num[j]++;
      match_num[j] += m_nodes.size();
      result_num[j] += results.size();
      search_time[j] += ((middle.tv_sec - start.tv_sec) * 1000 + (middle.tv_usec - start.tv_usec) * 1.0 / 1000);
      fetch_time[j] += ((term.tv_sec - middle.tv_sec) * 1000 + (term.tv_usec - middle.tv_usec) * 1.0 / 1000);

      // printf("Time: %ld  Result Num: %d  Nodes Num: %d  Match Num: %d\n", (term.tv_sec - start.tv_sec) * 1000000 +
      //   (term.tv_usec - start.tv_usec), (int)results.size(), (int)matches.size(), (int)agg.size());
    }
  }

  int idx = 1;
  while (true) {
    if (query_num[idx] == 0) break;
    int num = query_num[idx];
    double total_time = (search_time[idx] + fetch_time[idx]) / num;
    printf("%d %d %3f %3f %3f %d %d %d %d %d\n", idx, num, total_time, search_time[idx] / num, 
          fetch_time[idx] / num, match_num[idx] / num, result_num[idx] / num, match_num[idx], result_num[idx],
          bs_num[idx] / num);
    ++idx;
  }
  return 0;
}

struct triple {
  int a;
  int b;
  int c;
  triple(int x, int y, int z)
    : a(x), b(y), c(z) { }
};

bool comp(const triple &t1, const triple &t2) {
  return t1.c < t2.c;
}

void printResults(string & query, int pos, unordered_map<int, pair<int,int> > &results) {
  vector<triple> triples;
  printf("============================\n");
  printf("%s\n", query.substr(0, pos).c_str());
  printf("============================\n");
  for (auto it = results.begin(); it != results.end(); ++it)
    triples.push_back(triple(it->first, it->second.first, it->second.second));
  sort(triples.begin(), triples.end(), comp);
  for (auto it = triples.begin(); it != triples.end(); ++it) {
    printf("\x1b[31m%s\x1b[0m", recs[it->a].substr(0, it->b).c_str());
    printf("%s\n", recs[it->a].substr(it->b).c_str());
  }
}
