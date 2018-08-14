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

unsigned K;
int tau_num[1000];
int f[50][200000][50];

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "missing argument" << std::endl;
        std::cerr << "should be called with these three parameters: input_data query_data K" << std::endl;
        exit(1);
    }

    std::vector<std::string> records;
    std::vector<std::string> queries;
    read_file(argv[1], records);
    read_file(argv[2], queries);
    K = atoi(argv[3]);

    for (int id = 0; id < (int)queries.size(); ++id) {
        std::string& str = queries[id];
        if (id % 10 == 0)
            std::cout << id << std::endl;
        memset(f, 60, sizeof(f));
        for (int j = 0; j < (int)records.size(); ++j) {
            f[0][j][0] = 0;
            for (int i = 0; i <= (int)str.length(); ++i)
                f[i][j][0] = i;
            for (int k = 0; k <= (int)records[j].length(); ++k)
                f[0][j][k] = k;
        }
        for (int i = 0; i < (int)str.length(); ++i) {
            std::multiset<std::pair<int, int>> tau;
            for (int j = 0; j < (int)records.size(); ++j) {
                std::string& rec = records[j];
                int ped = 1000;
                for (int k = 0; k < (int)rec.length(); ++k) {
                    f[i + 1][j][k + 1] = std::min(f[i][j][k] + (str[i] != rec[k]),
                        std::min(f[i][j][k + 1], f[i + 1][j][k]) + 1);
                    ped = std::min(f[i + 1][j][k + 1], ped);
                }
                if (tau.empty() || tau.size() < K || ped < tau.rbegin()->first) {
                    tau.insert(std::make_pair(ped, j));
                    if (tau.size() > K) tau.erase(--tau.end());
                }
                //printf("index %d ped %d\n", j, ped);
            }
            for (int k = 1; k < K && !tau.empty(); ++k) {
    //            printf("%s ", records[tau.begin()->second].c_str());
                tau.erase(tau.begin());
            }
//            printf("%s ", records[tau.begin()->second].c_str());
  //          printf("tau=%d\n", tau.begin()->first);
            tau_num[tau.begin()->first]++;
        }
    }
    int idx = 0;
    if (tau_num[idx] == 0) ++idx;
    for (;;) {
        if (tau_num[idx] == 0) break;
        printf("tau=%d count=%d\n", idx, tau_num[idx]);
        idx++;
    }
}

