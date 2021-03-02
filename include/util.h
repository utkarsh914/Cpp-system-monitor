#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <string>

using namespace std;

class Util {
 public:
  static string convertToTime(long input_seconds);
  static string getProgressBar(float percent);
  static ifstream getStream(string path);
};

#endif