#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/time.h>

#include "trie.h"
#include "match.h"
#include "mtrie.h"

void read_file(const char *file, std::vector<std::string>& vec) {
    std::ifstream fin(file);
    if (!fin.is_open()) {
        std::cerr << "error opening file " << file << std::endl;
        exit(1);
    }
    std::string str;
    while (getline(fin, str)) {
        for (auto& ch : str)
            ch = tolower(ch);
        vec.push_back(str);
    }
}

bool verbose = false;
unsigned K;

namespace std {
    template<>
        struct hash<std::pair<int, int>> {
            inline size_t operator () (std::pair<int, int> const& p) const {
                return ((std::hash<int>()(p.first) << 16) + std::hash<int>()(p.second));
            }
        };
}

const int maxPrefix = 1000;
double search_time[maxPrefix];
int query_num[maxPrefix];
int tau_num[maxPrefix];
int bs_num[maxPrefix];

std::multiset<std::pair<int, std::pair<Match, std::pair<MTrieNode*, int>>>> out_range; // <i, match, mnode, pos>
Trie* trie;
MTrie* mtrie;
auto set_cmp = [] (const MTrieNode* a, const MTrieNode* b) {
    return a->matches[a->min_index] < b->matches[b->min_index];
};
std::multiset<MTrieNode*, decltype(set_cmp)> M1(set_cmp);
std::multiset<std::pair<int, TrieNode*>> M2;
std::unordered_map<TrieNode*, std::multiset<MTrieNode*, decltype(set_cmp)>::iterator> map;
std::unordered_set<std::pair<int, int>> S;
std::unordered_set<int> result;
int hit, miss;

void getpos(const Match &match, int depth, int pos, int &x, int &y) {
    if (pos <= match.md) {
        x = match.i + match.md + 1;
        y = depth + pos + 1;
    } else {
        x = match.i + match.md - (pos - match.md) + 1;
        y = depth + match.md + 1;
    }
}

void expand(const std::string &str, int i, int tau, MTrieNode *mnode, Match &match,
        int pos, int &x, int &y) {
    if (x == -1 && y == -1) {
        getpos(match, mnode->node->depth, pos, x, y);
    }
    if (x > i) {
        out_range.insert(std::make_pair(x, std::make_pair(match, std::make_pair(mnode, pos))));
        return;
    }

    if (trie->index.find(str[x]) != trie->index.end()
            && trie->index[str[x]].find(y) != trie->index[str[x]].end()) {
        std::vector<TrieNode*> &vec = trie->index[str[x]][y];
        bs_num[i + 1]++;
        auto iter = std::lower_bound(vec.begin(), vec.end(), mnode->node->id,
                [] (const TrieNode* node, const int id) {
                return node->id < id;
                });
        while (iter != vec.end() && (*iter)->id <= mnode->node->last) {
            Match new_match(x, match.ped + match.md, 0);
            auto id = std::make_pair((*iter)->id, new_match.i);
            if (S.find(id) == S.end()) {
                S.insert(id);
                MTrieNode* node;
                if (map.find(*iter) == map.end()) {
                    node = new MTrieNode();
                    node->key_index = node->min_index = 0;
                    node->matches.push_back(new_match);
                    node->node = *iter;
                    node->child = node->next = NULL;
                    node->parent = mnode;
                    map[*iter] = M1.insert(node);
                    M2.insert(std::make_pair(new_match.ped - new_match.i, (*iter)));
                } else {
                    auto mit = map[*iter];
                    node = *mit;
                    M1.erase(mit);
                    auto key_match = node->matches[node->key_index];
                    auto min_match = node->matches[node->min_index];
                    M2.erase(std::make_pair(key_match.ped - key_match.i, *iter));

                    node->matches.push_back(new_match);
                    if (new_match.ped - new_match.i < key_match.ped - key_match.i)
                        node->key_index = node->matches.size() - 1;
                    if (new_match.ped + new_match.md < min_match.ped + min_match.md)
                        node->min_index = node->matches.size() - 1;

                    map[*iter] = M1.insert(node);
                    auto &new_key_match = node->matches[node->key_index];
                    M2.insert(std::make_pair(new_key_match.ped - new_key_match.i, (*iter)));
                }
                hit++;
            } else miss++;
            if (new_match.ped - new_match.i + i <= tau) {
                (*iter)->get_records(result, K, trie);
            }
            ++iter;
        }
    }
    return;
}

void work(const std::string &str, std::multiset<std::pair<int, TrieNode*>>::iterator &iter,
        int i, int tau) {
    // process out of range expansion first
    while (!out_range.empty()) {
        auto match = *out_range.begin();
        if (match.first > i) break;
        out_range.erase(out_range.begin());
        int x = -1, y = -1;
        expand(str, i, tau, match.second.second.first, match.second.first, match.second.second.second, x, y);
    }

    // fetch result
    for (; iter != M2.end() && iter->first + i <= tau; ++iter) {
        iter->second->get_records(result, K, trie);
        if (result.size() == K) return;
    }

    // need more
    while (!M1.empty()) {
        MTrieNode* mnode = *M1.begin();
        auto &min_match = mnode->matches[mnode->min_index];
        int min_ped = min_match.ped + min_match.md;
        if (min_ped > tau) break;
        M1.erase(M1.begin());

        int cx, cy, x = -1, y = -1, lx = -1, ly = -1;
        std::vector<Match*> M;
        for (auto &match : mnode->matches) {
            if (match.ped + match.md != min_ped) continue;
            M.push_back(&match);
        }
        for (int k = M.size() - 1; k >= 0; --k) {
            auto match = M[k];
            if (k == 0) cx = cy = -1;
            else {
                auto pm = M[k - 1];
                cx = pm->i + pm->md + 1;
                cy = mnode->node->depth + pm->md + 1;
            }
            int pos = 0;
            getpos(*match, mnode->node->depth, pos, x, y);
            if (x > lx && y < ly) {
                if (y + match->md > ly) pos += ly - y - 1;
                else pos += match->md + x - lx - 1;
            }
            for (; pos < match->md + match->md + 1; ++pos) {
                getpos(*match, mnode->node->depth, pos, x, y);
                if (x < cx && y < cy) break;
                expand(str, i, tau, mnode, *match, pos, x, y);
            }
            lx = match->i + 1;
            ly = mnode->node->depth + match->md + 1;
            ++match->md;
        }
        map[mnode->node] = M1.insert(mnode);
        if (result.size() == K) break;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "missing argument" << std::endl;
        std::cerr << "should be called with these three parameters: input_data query_data K" << std::endl;
        exit(1);
    }

    //std::clog << "reading data...";
    std::vector<std::string> records;
    std::vector<std::string> queries;
    read_file(argv[1], records);
    read_file(argv[2], queries);
    K = atoi(argv[3]);
    if (argc >= 5 && strcmp(argv[4], "-v") == 0) verbose = true;
    //std::clog << "done" << std::endl;

    //std::clog << "building index...";
    trie = new Trie();
    for (auto i = 0; i < (int)records.size(); ++i) {
        trie->insert(records[i], i);
    }
    trie->build_index();
    //std::clog << "done" << std::endl;

    //std::clog << "processing queries..." << std::endl;
    timeval start, term;
    //std::clog << std::endl;
    for (int idx = 0; idx < (int)queries.size(); ++idx) {
        if (idx % 100 == 0) std::clog << idx << std::endl;

        std::string str = queries[idx];
        out_range.clear();
        M1.clear();
        map.clear();
        M2.clear();
        S.clear();
        MTrieNode *root = new MTrieNode();
        root->node = trie->root;
        root->key_index = 0;
        root->matches.push_back(Match(-1, 0, 0));
        root->child = root->next = root->parent = NULL;
        MTrie* mtrie = new MTrie();
        mtrie->root = root;
        map[trie->root] = M1.insert(root);
        M2.insert(std::make_pair(1, trie->root));
        S.insert(std::make_pair(trie->root->id, -1));

        int tau = 0;
        for (auto i = 0; i < (int)str.length(); ++i) {
            result.clear();
            gettimeofday(&start, NULL);

            auto iter = M2.begin();
            work(str, iter, i, tau);
            if (result.size() < K) {
                ++tau;
                work(str, iter, i, tau);
            }
            gettimeofday(&term, NULL);

            if (verbose) {
                std::cout << str.substr(0, i + 1) << " tau=" << tau << ':' << std::endl;
                for (auto &i : result) {
                    std::cout << '\t' << i << ':' << records[i] << std::endl;
                }
                std::cout << std::endl;
            } else {
                //std::cout << str.substr(0, i + 1) << " tau=" << tau << " result=" << result.size() << std::endl;
            }
            query_num[i + 1]++;
            tau_num[tau]++;
            search_time[i + 1] += ((term.tv_sec - start.tv_sec) * 1000 + (term.tv_usec - start.tv_usec) * 1.0 / 1000);
        }
        //std::cout << str << std::endl;
    }
    //std::clog << "done" << std::endl;

    /*int idx = 1;
    while (true) {
        if (query_num[idx] == 0) break;
        int num = query_num[idx];
        double total_time = search_time[idx] / num;
        printf("%d %d %3f %d\n", idx, num, total_time, bs_num[idx]);
        ++idx;
    }*/
    int idx = 0;
    if (tau_num[idx] == 0) ++ idx;
    for (;;) {
        if (tau_num[idx] == 0) break;
        printf("tau=%d count=%d\n", idx, tau_num[idx]);
        idx++;
    }
    printf("hit=%d, miss=%d, accuracy=%.3f\n", hit, miss, hit / (hit + miss + .0));

    return 0;
}

