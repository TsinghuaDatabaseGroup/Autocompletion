#include <stdio.h>
#include "easy_timer.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

namespace dbwsim {

void EasyTimer::Start()
{
  elapse_time = elapse_time_sec = elapse_time_usec = 0;
  gettimeofday(&start_t, NULL);
}

void EasyTimer::Restart()
{ 
  gettimeofday(&start_t, NULL);
}


void EasyTimer::Reset()
{
  elapse_time = elapse_time_sec = elapse_time_usec = 0;
}


void EasyTimer::Finish()
{
  timeval res;  
  gettimeofday(&end_t, NULL);
  timersub(&end_t,&start_t,&res);
  elapse_time += (res.tv_sec + res.tv_usec/1000000.0);
  // elapse_time_sec += (long) (end_t.tv_sec - start_t.tv_sec);
  // elapse_time_usec += (long) (end_t.tv_usec - start_t.tv_usec);  
}

const string EasyTimer::ToString(){ 
  stringstream str_stream;
  char message[256];  
  // sprintf(message, "%ld.%06ld",
  //         elapse_time_sec + elapse_time_usec / 100000,
  //         elapse_time_usec % 100000);

  sprintf(message, "%.6f", elapse_time);
  
  str_stream << message;
  return str_stream.str();
}

double EasyTimer::ToValue(){
  return elapse_time;
  
  // double value = 0;
  // value = elapse_time_sec + elapse_time_usec / 100000 +
  //         (elapse_time_usec % 100000)/1000000.0;
  // return value;
}

}
