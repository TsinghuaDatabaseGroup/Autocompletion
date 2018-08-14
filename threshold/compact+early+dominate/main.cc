/*

   Copyright (C) 2011 by The Department of Computer Science and
   Technology, Tsinghua University

   Redistribution of this file is permitted under the terms of
   the BSD license.

Author   : Dong Deng
Created  : 2014-09-05 11:42:44 
Modified : 2014-10-21 22:37:46
Contact  : dd11@mails.tsinghua.edu.cn

*/

#include "util.h"
#include "trie.h"
#include "mtrie.h"
#include <sys/time.h>
#include <stdlib.h>

#define MAXIMUM 0x7fffffff

vector<string> recs;
vector<string> queries;
int tau;

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

void quickFind(vector<TrieNode*>::iterator &vit, const vector<TrieNode*>::const_iterator end, MTrieNode* node, int themin, const int row) {
	if (node->opt.first < 0) node->opt = node->parent->opt;
	MTrieNode* cnode = node->child;
	MTrieNode* pnode = NULL;

	int next_start = MAXIMUM;
	if (cnode != NULL) next_start = cnode->tnode->id;

	while (vit != end && (*vit)->id <= node->tnode->last) {
		if ((*vit)->id < next_start && (*vit)->last < next_start) {
			// left
			if (themin <= tau &&
					node->opt.second + (*vit)->depth - node->opt.first > themin) {
				MTrieNode* new_node = new MTrieNode();
				new_node->parent = node;
				new_node->tnode = *vit;
				new_node->eds.push_back(make_pair(row, themin));
				new_node->opt = make_pair((*vit)->depth, themin);
				if (pnode == NULL) {
					node->child = new_node;
					new_node->next = cnode;
				} else {
					pnode->next = new_node;
					new_node->next = cnode;
				}
				pnode = new_node;
			}
		} else if ((*vit)->id < next_start && (*vit)->last >= next_start) {
			// ancestor
			if (themin <= tau &&
					node->opt.second + (*vit)->depth - node->opt.first > themin) {
				MTrieNode* new_node = new MTrieNode();
				new_node->parent = node;
				new_node->child = cnode;
				new_node->tnode = *vit;
				new_node->eds.push_back(make_pair(row, themin));
				new_node->opt = make_pair((*vit)->depth, themin);
				while (true) {
					cnode->parent = new_node;
					if (cnode->next == NULL) {
						cnode = cnode->next;
						break;
					}
					if (cnode->next->tnode->id > (*vit)->last) {
						MTrieNode* temp = cnode->next;
						cnode->next = NULL;
						cnode = temp;
						break;
					}
					cnode = cnode->next;
				}
				if (pnode == NULL) {
					node->child = new_node;
				} else {
					pnode->next = new_node;
				}
				pnode = new_node;
				new_node->next = cnode;
			} else {
				while (cnode) {
					if (cnode->tnode->id > (*vit)->last) break;
					pnode = cnode;
					cnode = cnode->next;
				}
			}
			if (cnode != NULL) next_start = cnode->tnode->id;
			else next_start = MAXIMUM;
		} else if ((*vit)->id == next_start) {
			// self
			if (themin <= tau &&
					node->opt.second + (*vit)->depth - node->opt.first > themin) {
				cnode->eds.push_back(make_pair(row, themin));
			}
		} else if ((*vit)->id > next_start && (*vit)->id <= cnode->tnode->last) {
			// child
			int thenewmin = themin;
			for (auto qit = cnode->eds.begin(); qit != cnode->eds.end(); ++qit) {  // <row, ed>
				if (row == qit->first) continue;
				if (thenewmin > qit->second + max(row - 1 - qit->first, (*vit)->depth - 1 - cnode->tnode->depth))
				  thenewmin = qit->second + max(row - 1 - qit->first, (*vit)->depth - 1 - cnode->tnode->depth);
			}
			if (thenewmin > tau)
			  cout << "BIGGER BIGGER" << endl;
			quickFind(vit, end, cnode, thenewmin, row);
			pnode = cnode;
			cnode = cnode->next;
			if (cnode != NULL) next_start = cnode->tnode->id;
			else next_start = MAXIMUM;
			continue;
		} else if ((*vit)->id > next_start && (*vit)->id > cnode->tnode->last) {
			// right
			pnode = cnode;
			cnode = cnode->next;
			if (cnode != NULL) next_start = cnode->tnode->id;
			else next_start = MAXIMUM;
			continue;
		}
		++vit;
	}
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
	tau = atoi(argv[3]);

	readData(filename, recs);
	readData(queryfile, queries);

	// construct trie index
	cerr << "#  Construct Index :" << endl;
	Trie* trie = new Trie();
	for (auto i = 0; i < recs.size(); i++)
	  trie->append(recs[i].c_str(), i);
	trie->buildIdx();
	trie->bfsIndex();

	for (auto i = 0; i < queries.size(); i++) {
		MTrie* mtrie = new MTrie(tau);
		mtrie->root->insertChild(trie->root, make_pair(0,0));

		/*printf("============================\n");
		printf("\n%s\n", queries[i].c_str());
		printf("============================\n");*/

        vector<MTrieNode*> trash;
		for (auto j = 1; j <= queries[i].length(); j++) {
			timeval start, middle, term;
			gettimeofday(&start, NULL);
			queue<MTrieNode*> q;  // mnode
			char ch = queries[i][j - 1];     // current key stroke

			// Get the list of Mnodes;
			vector<pair<MTrieNode*, pair<int, int>>> v; // all the mnodes <first_level, last_level>, BFS;
			queue<pair<MTrieNode*, int>> qp;  // <mnode, last_level>
			qp.push(make_pair(mtrie->root, -1));
			while (!qp.empty()) {
				qp.front().first->opt = make_pair(-32767, 0);
				int parent_last_level = qp.front().second;
				MTrieNode* cnode = qp.front().first->child;
				qp.pop();
				while (cnode) {
					int first_level = max(cnode->tnode->depth + 1, parent_last_level + 1);
					int last_level = cnode->tnode->depth + 1;
					for (; last_level <= cnode->tnode->depth + tau + 1; ++last_level) {
						int themin = tau + 1;
						for (auto qit = cnode->eds.begin(); qit != cnode->eds.end(); ++qit) {  // <row, ed>
							if (themin > qit->second + max(j - 1 - qit->first, last_level - 1 - cnode->tnode->depth))
							  themin = qit->second + max(j - 1 - qit->first, last_level - 1 - cnode->tnode->depth);
						}
						if (themin > tau) break;
					}
					v.push_back(make_pair(cnode, make_pair(first_level, last_level - 1)));
					qp.push(make_pair(cnode, last_level - 1));
					cnode = cnode->next;
				}
			}

			// printf("%d\n", (int)v.size());

			for (auto pmnode = v.begin(); pmnode != v.end(); ++pmnode) {
				MTrieNode* node = pmnode->first;
				for (int depth = pmnode->second.first; depth <= pmnode->second.second; ++depth) {
					vector<TrieNode*> &vec = trie->index[ch][depth];
					auto vit = lower_bound(vec.begin(), vec.end(), node->tnode->id, comp_id);
                    bs_num[j]++;
					int themin = tau + 1;
					for (auto qit = node->eds.begin(); qit != node->eds.end(); ++qit) {  // <row, ed>
						if (j == qit->first) continue;
						if (themin > qit->second + max(j - 1 - qit->first, depth - 1 - node->tnode->depth))
						  themin = qit->second + max(j - 1 - qit->first, depth - 1 - node->tnode->depth);
					}
					quickFind(vit, vec.end(), node, themin, j);
				}
			}

			q.push(mtrie->root);
			while (!q.empty()) {
				MTrieNode* node = q.front();
				MTrieNode* cnode = node->child;
				q.pop();
				MTrieNode* pnode = NULL;
				while (cnode) {
					if (cnode->eds.back().first == j) {
						pair<int, int> match = cnode->eds.back();
						cnode->eds.pop_back();
						while (!cnode->eds.empty() && match.second <= cnode->eds.back().second)
						  cnode->eds.pop_back();
						cnode->eds.push_back(match);
					}
					int pop_num = 0;
					for (auto qit = cnode->eds.begin(); qit != cnode->eds.end(); ++qit) {
						if (j - qit->first + qit->second > tau ||
							node->opt.second + cnode->tnode->depth - node->opt.first <= qit->second)
						  ++pop_num;
						else break;
					}
					if (pop_num == cnode->eds.size()) {
						// remove the cnode
                        trash.push_back(cnode);
						MTrieNode* ccnode = cnode->child;
						if (ccnode == NULL) {
							if (pnode == NULL) {
								node->child = cnode->next;
							} else {
								pnode->next = cnode->next;
								// TODO delete the pointer
							}
							cnode = cnode->next;
						} else {
							while (true) {
								ccnode->parent = node;
								if (ccnode->next == NULL) break;
								ccnode = ccnode->next;
							}
							ccnode->next = cnode->next;
							if (pnode == NULL) {
								node->child = cnode->child;
							} else {
								pnode->next = cnode->child;
							}
							cnode = cnode->child;
						}
					} else {
						while (--pop_num >= 0) cnode->eds.pop_front();
						q.push(cnode);
						pnode = cnode;
						cnode = cnode->next;
					}
				}
			}

			gettimeofday(&middle, NULL);

			vector<TrieNode*> m_nodes;
			MTrieNode* cnode = mtrie->root->child;
			while (cnode) {
				m_nodes.push_back(cnode->tnode);
				cnode = cnode->next;
			}
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

			/*printf("Time: %ld  Result Num: %d  Nodes Num: %d  Match Num: %d\n", (term.tv_sec - start.tv_sec) * 1000000 +
						(term.tv_usec - start.tv_usec), (int)results.size(), (int)v.size(), (int)m_nodes.size());
			
			   for (auto ait = agg.begin(); ait != agg.end(); ++ait) {
			   printf("Depth: %d", ait->first->depth);
			   for (auto qit = ait->second.begin(); qit != ait->second.end(); ++qit)
			   printf(" <%d %d> ", qit->first, qit->second);
			   cout << endl;
			   }
			   */
		}
        delete mtrie;
        for (auto& t : trash) {
            t->next = t->child = NULL;
            delete t;
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


/*
   int beg_depth = j - tau >= 1 ? j - tau : 1;
   int end_depth = j + tau;
   for (auto depth = beg_depth; depth <= end_depth; ++depth) {
   vector<TrieNode*> &vec = trie->index[ch][depth];
   for (auto vit = vec.begin(); vit != vec.end(); ++vit) {
   }
   }


*/
