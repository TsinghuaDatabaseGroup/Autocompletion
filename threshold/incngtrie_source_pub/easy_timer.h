#ifndef __usage_h__
#define __usage_h__

#include <string>
#include <sys/time.h>

using namespace std;

namespace dbwsim {

class EasyTimer{
public:
  struct timeval start_t;
  struct timeval end_t;

  double elapse_time;
  long elapse_time_sec;
  long elapse_time_usec;

  EasyTimer(){}
  ~EasyTimer(){}
  
  void Start();

  void Reset();

  void Restart();
  
  void Finish();

  double ToValue();
  
  const string ToString();
};

}

#endif
