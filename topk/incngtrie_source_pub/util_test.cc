#include "util.h"

// istream getline
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace dbwsim;
using namespace std;

void naive_test()
{
  int myints[] = {0,1,2,3,4,5,6,7,8,9,10};
  cout << "The 3! possible permutations with 3 elements:\n";
  sort (myints,myints+3);
  
  do {
    cout << myints[0] << " " << myints[1] << " " << myints[2] << endl;   
  } while ( next_combination (myints, myints + 3,  myints + 11) );
}

void edit_verify_test()
{
  string data = "simplain";
  string query = "nipple";
  int ed4 = edit_verify_debug(data.c_str(), data.length(), query.c_str(), query.length(), 4);
  int ed3 = edit_verify_debug(data.c_str(), data.length(), query.c_str(), query.length(), 3);
  int ed2 = edit_verify_debug(data.c_str(), data.length(), query.c_str(), query.length(), 2);
  int ed1 = edit_verify_debug(data.c_str(), data.length(), query.c_str(), query.length(), 1);
  cout << ed4 << ed3 << ed2 << ed1 << endl;
}

int main(int argc, char **argv)
{
  edit_verify_test();
  //naive_test();
}
