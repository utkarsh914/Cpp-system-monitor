#ifndef PROCESS_PARSER_H
#define PROCESS_PARSER_H

#include <constants.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <util.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std;

class ProcessParser {
 public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  static string getCpuPercent(string pid);
  static long int getSysUpTime();
  static string getProcUpTime(string pid);
  static string getProcUser(string pid);
  static vector<string> getSysCpuPercent(string coreNumber = "");
  static float getSysRamPercent();
  static string getSysKernelVersion();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static string getOsName();
  static string printCpuStats(vector<string> values1, vector<string> values2);
};

string ProcessParser::getVmSize(string pid) {
  string line;
  string name = "VmData";
  string value;
  float result;

  // open stream for specific file
  ifstream stream =
      Util::getStream(Path::basePath() + pid + Path::statusPath());

  while (getline(stream, line)) {
    // searching line by line
    // comparing starting of the line with the name (i.e. "VmData")
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      // convert from kB -> GB
      result = stof(values[1]) / float(1024 * 1024);
      break;
    }
  }

  return to_string(result);
}

string ProcessParser::getCpuPercent(string pid) {
  string line;
  ifstream stream =
      Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  float utime = stof(ProcessParser::getProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float totalTime = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  float result = 100.0 * ((totalTime / freq) / seconds);

  return to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
  string line;
  ifstream stream =
      Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  return to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime() {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  return stol(values[0]);
}

#endif