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
  static float getVmSize(string pid);
  static float getCpuPercent(string pid);
  static long getSysUpTime();
  static long getProcUpTime(string pid);
  static string getProcUser(string pid);
  static float getSysRamPercent();
  static string getSysKernelVersion();
  static int getNumberOfCores();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static string getOsName();
  static vector<string> getSysCpuPercent(int coreNumber = -1);
  static float printCpuStats(vector<string> values1, vector<string> values2);
};


#endif