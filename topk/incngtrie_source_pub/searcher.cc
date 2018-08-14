// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

#include <getopt.h>
#include "fastssindex.h"
#include "active_node.h"
#include "variants.h"
#include "dataset.h"
#include "resultset.h"
#include "searcher_base.h"
#include "fastss_searcher.h"
#include "easy_timer.h"
#include "map_trie.h"
#include "vector_trie.h"
#include "brother_trie.h"

#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>

using namespace std;
using namespace dbwsim;

char g_version[]=VERSION;
void print_version(){
  fprintf(stderr, "Version: %s\n", g_version);
}

long long num_queries = 0;
long long num_results = 0;
long long num_actives = 0;
long long smart_fetch_count = 0;
long long stupid_fetch_count = 0;
int K;

EasyTimer indexing_timer, query_timer, stupid_timer, smarter_timer;

void print_usage(){
  fprintf(stderr, "usage: -t <Max Edit Distance>\n");
  fprintf(stderr, "       -d <Data File>\n");
  fprintf(stderr, "       -c <Trie Type, MapTrie Default>\n");
  fprintf(stderr, "          map    is using MapTrie \n");
  fprintf(stderr, "          vec    is using VectorTrie \n");
  fprintf(stderr, "          bro    is using BrotherTrie \n");
  fprintf(stderr, "       -p print active nodes\n");
  fprintf(stderr, "       -k process query by one.\n");
  fprintf(stderr, "       -s stupid fatching.\n");
  fprintf(stderr, "       -i interactive mode\n");
  fprintf(stderr, "       -f no result fetch at end.\n");  
  fprintf(stderr, "       -s fetch results one character by one\n");
  print_version();
  exit(0);
}

void running_interactive_mode(SearcherBase* searcher,
                              bool onebyone = false,
                              bool print_active = false){  
  char query[256];
  while (true) {
    cout << endl << "Query: ";
    cin.getline(query, 256);
    searcher->ResetSearcher();
    searcher->ExtendQuery(query, strlen(query));
    if (onebyone)
      searcher->ProcessOneByOne();
    else
      searcher->ProcessAll();
    searcher->ResultsStatistic(cout, print_active, true);
  }
}

vector<string> queryset;
char query[256];
int maxlen = 0;

void running_test_mode(SearcherBase* searcher1,
                       SearcherBase* searcher2,
                       bool do_smart_fetch = true,
                       bool onebyone = false,
                       bool do_stupid_fetch = false,
                       int prefixlen = -1){
  fflush(stderr);
  char line[256];
  smart_fetch_count = num_results = num_actives = stupid_fetch_count = 0;
  SearcherBase* searcher = searcher1;
  for (vector<string >::iterator it = queryset.begin();
       it != queryset.end(); it ++) {
    strcpy(line, (*it).c_str());
    if (prefixlen > 0){
      line[prefixlen]='\0';
    }

    // Make it hot from cool
    // searcher->ResetSearcher();
    // searcher->ExtendQuery(line, strlen(line));    
    // searcher->ProcessAll(false);
    
    // Run the real test.
    searcher->ResetSearcher();
    searcher->ExtendQuery(line, strlen(line));
    
    query_timer.Restart();
    searcher->ProcessAll(false);
    query_timer.Finish();

    if (do_stupid_fetch) {
      // Keep result set for final step.
      stupid_timer.Restart();
      stupid_fetch_count += searcher->FectchResults(true);
      stupid_timer.Finish();
    }     
        
    if (do_smart_fetch) {
      // Keep result set for final step.
      smarter_timer.Restart();
      smart_fetch_count += searcher->FectchResults(false);
      smarter_timer.Finish();      
    }
    if (searcher->result_set_.size() < K) {
        searcher2->ResetSearcher();
        searcher2->ExtendQuery(line, strlen(line));

        query_timer.Restart();
        searcher2->ProcessAll(false);
        query_timer.Finish();

        if (do_smart_fetch) {
            // Keep result set for final step.
            smarter_timer.Restart();      
            smart_fetch_count += searcher2->FectchResults(false);
            smarter_timer.Finish();      
        }
        if (searcher2->result_set_.size() < K) {
            fprintf(stderr, "oops\n");
            exit(1);
        }
    }
    
    //searcher->ResultsStatistic(cout, false, false);
    num_results += searcher->result_set_.size();
    num_actives += searcher->current_active_.size();    
  }
}

void loadqueryset(){  
  while (fgets(query, 256, stdin)){
    num_queries ++;
    int len = strlen(query);
    if (len == 0) continue;
    while ( query[len-1] == '\n' ||
            query[len-1] == '\r' )
    { query[--len] = '\0';}
    
    queryset.push_back(query);
    if (maxlen < len){
      maxlen = len;
    }
  }
}

int main(int argc, char* argv[])
{
  char c;
  int max_tau = 3;
  char *data_input = NULL;
  bool print_active = false;
  bool fetch_onbyone = false;
  bool interactive = false;
  bool do_smart_fetch = false;
  char algo[20] = "fastss";
  FastssIndex *fastss_index = NULL;
  FastssIndex *fastss_index2 = NULL;
  SearcherBase *searcher = NULL;
  SearcherBase *searcher2 = NULL;
  ActiveNodePool *active_pool = NULL;
  ActiveNodePool *active_pool2 = NULL;
  char triesel[20] = "map";
  bool do_step = false;
  int stepfrom = 1;
  bool do_stupid_fetch = false;
  TrieBase* indextrie;
  
  while ((c = getopt(argc,argv, "hvt:d:c:pik:fs")) != -1)
    switch (c){
      case 't':
        max_tau = atoi(optarg);
        break;
      case 'd':
        data_input = optarg;
        break;
      case 'c':
        strcpy(triesel, optarg);
        break;        
      case 'p':
        print_active = true;
        break;
      case 'k':
        K = atoi(optarg);
        break;        
      case 's':
        do_stupid_fetch = true;
        break;
      case 'i':
        interactive = true;
        break;        
      case 'f':
        do_smart_fetch = true;
        break;        
      case 'h':
        print_usage();
        break;
      case 'v':
        print_version();
        break;
      case '?':
        if ( optopt == 't' || optopt == 'd' || optopt == 'a' || optopt == 'c' || optopt == 'k' )
          cerr << "Error: Option -" << optopt << "requires an argument." << endl;
        else if ( isprint(optopt))
          cerr << "Error: Unknown Option -" << optopt << endl;
        else
          cerr << "Error: Unknown Option character" <<endl;
        return 1;
      default:
        print_usage();
    }
  
  if (data_input == NULL){
    cerr << "Need input file name" <<endl;
    print_usage();
  }

  // Choose the right trie.
  if(strcmp(triesel, "vec") == 0) {
    indextrie = new VectorTrie();
  }else if(strcmp(triesel, "bro") == 0) {
    indextrie = new BrotherTrie();
  }else {
    indextrie = new MapTrie();
  }
  
  // Init it as fastss algorithm;
  printf("Indexing...\n");
  indexing_timer.Start();
  fastss_index = new FastssIndex();
  fastss_index -> Initilization(data_input, 1, false, 0, indextrie);
  fastss_index2 = new FastssIndex();
  fastss_index2 -> Initilization(data_input, 2, false, 0, indextrie);
  indexing_timer.Finish();
  active_pool = new ActiveNodePool();
  FastSSSearcher *fsssearcher = new FastSSSearcher();
  fsssearcher -> Initialize(fastss_index, active_pool, 1, false);
  searcher = fsssearcher;
  active_pool2 = new ActiveNodePool();
  FastSSSearcher *fsssearcher2 = new FastSSSearcher();
  fsssearcher2 -> Initialize(fastss_index2, active_pool2, 2, false);
  searcher = fsssearcher;
  searcher2 = fsssearcher2;
  printf("Indexing End...\n");
  
  if (interactive){
    running_interactive_mode(searcher, fetch_onbyone, print_active);
  }
  else
  {
    loadqueryset();
    int to_step = maxlen;
    int start = maxlen;
    if (do_step)
      to_step = stepfrom;
    
    while (start >= to_step){
      // Running interactive mode.
      query_timer.Reset();
      stupid_timer.Reset();
      smarter_timer.Reset();     
      running_test_mode(searcher, searcher2, do_smart_fetch, fetch_onbyone,
                        do_stupid_fetch, start);
      
      //stri node_str = "";
      stringstream str_stream;
      str_stream << data_input;
      str_stream << " Algo: " << algo;
      str_stream << " tau: " << max_tau;
      str_stream << " #QRY: " << num_queries;
      str_stream << " #Trie: " << triesel;
      str_stream << " PFXL: " << start;
      str_stream << " FETR: " << (do_smart_fetch? "1":"0");
      str_stream << " STUP: " << (do_stupid_fetch? "1":"0");
      str_stream << " FETD: " << smart_fetch_count;
      str_stream << " STUPD: " << stupid_fetch_count;
      str_stream << " #RST: " << num_results;
      str_stream << " #ACT: " << num_actives;
      str_stream << " Idxtm: " << indexing_timer.ToString();
      str_stream << " FCHTM: " << smarter_timer.ToString();
      str_stream << " STUTM: " << stupid_timer.ToString();
      str_stream << " QRYTM: " << query_timer.ToString();
      //cerr << str_stream.str() << endl;     
	  double init_time = indexing_timer.ToValue() * 1000;
	  double work_time = query_timer.ToValue() * 1000;
	  double fetch_time = smarter_timer.ToValue() * 1000;
	  cerr << start << ' ' << num_queries << ' ' << (work_time + fetch_time) / num_queries << ' ';
	  cerr << work_time / num_queries << ' ' << fetch_time / num_queries << ' ';
	  cerr << num_actives / num_queries << ' ' << num_results / num_queries << ' ';
	  cerr << num_actives << ' ' << num_results << ' ' << init_time << endl;
      start --;
    }    
  }  
  delete indextrie;
  return 0;
}
