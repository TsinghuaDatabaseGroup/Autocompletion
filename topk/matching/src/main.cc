#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/time.h>

#include "trie.h"
#include "match.h"

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

std::multiset<std::pair<int, std::pair<Match, int>>> out_range; // <i, match, pos>
Trie* trie;
std::multiset<Match> M1;
std::multiset<std::pair<int, TrieNode*>> M2;
std::unordered_set<std::pair<int, int>> S;
std::unordered_set<int> result;
int hit, miss;

void expand(const std::string &str, int i, int tau, Match &match, int pos) {
    int x, y;
    if (pos <= match.md) {
        x = match.i + match.md + 1;
        y = match.node->depth + pos + 1;
    } else {
        x = match.i + match.md - (pos - match.md) + 1;
        y = match.node->depth + match.md + 1;
    }
    if (x > i) {
        out_range.insert(std::make_pair(x, std::make_pair(match, pos)));
        return;
    }

    if (trie->index.find(str[x]) != trie->index.end()
            && trie->index[str[x]].find(y) != trie->index[str[x]].end()) {
        std::vector<TrieNode*> &vec = trie->index[str[x]][y];
        bs_num[i + 1]++;
        auto iter = std::lower_bound(vec.begin(), vec.end(), match.node->id,
                [] (const TrieNode* node, const int id) {
                return node->id < id;
                });
        while (iter != vec.end() && (*iter)->id <= match.node->last) {
            Match new_match((*iter), x, match.ped + match.md, 0);
            auto id = std::make_pair(new_match.node->id, new_match.i);
            if (S.find(id) == S.end()) {
                S.insert(id);
                M1.insert(new_match);
                M2.insert(std::make_pair(new_match.ped - new_match.i, (*iter)));
                hit++;
            } else miss++;
            if (new_match.ped - new_match.i + i <= tau) {
                (*iter)->get_records(result, K, trie);
            }
            ++iter;
        }
    }
}

void work(const std::string &str, std::multiset<std::pair<int, TrieNode*>>::iterator &iter,
        int i, int tau) {
    // process out range expansion
    while (!out_range.empty()) {
        auto match = *out_range.begin();
        if (match.first > i) break;
        out_range.erase(out_range.begin());
        expand(str, i, tau, match.second.first, match.second.second);
    }

    // fetch result
    for (; iter != M2.end() && iter->first + i <= tau; ++iter) {
        iter->second->get_records(result, K, trie);
        if (result.size() == K) return;
    }

    // need more
    while (!M1.empty()) {
        auto match = *M1.begin();
        if (match.ped + match.md > tau) break;
        M1.erase(M1.begin());
        
        for (int pos = 0; pos < match.md + match.md + 1; ++pos) {
            expand(str, i, tau, match, pos);
        }
        ++match.md;
        M1.insert(match);
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
        std::string str = queries[idx];
        out_range.clear();
        M1.clear();
        M2.clear();
        S.clear();
        M1.insert(Match(trie->root, -1, 0, 0));
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

    int idx = 1;
    while (true) {
        if (query_num[idx] == 0) break;
        int num = query_num[idx];
        double total_time = search_time[idx] / num;
        printf("%d %d %3f %d\n", idx, num, total_time, bs_num[idx] / num);
        ++idx;
    }
    /*int idx = 0;
    if (tau_num[idx] == 0) ++ idx;
    for (;;) {
        if (tau_num[idx] == 0) break;
        printf("tau=%d count=%d\n", idx, tau_num[idx]);
        idx++;
    }
    extern int dup;
    printf("dup=%d, hit=%d, miss=%d, accuracy=%.3f\n", dup, hit, miss, hit / (hit + miss + .0));
*/
    return 0;
}

