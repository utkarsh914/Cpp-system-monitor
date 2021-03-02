#ifndef SYSTEM_H
#define SYSTEM_H

#include <constants.h>
// #include <util.h>
#include <process.h>
#include <process_parser.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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

class System {
private:
  string osName;
  string kernelVer;

  int numberOfCores;
  float cpuPercent;
  vector<string> lastCpuStats;
  vector<string> currentCpuStats;
  vector<float> coresPercent;
  vector<vector<string>> lastCpuCoresStats;
  vector<vector<string>> currentCpuCoresStats;

  float memPercent;
  float memTotal;
  long upTime;
  int totalProc;
  int runningProc;
  int threads;
  vector<Process> processes;

  // private methods
  void calculateCpuStats();
  void calculateCpuCoresStats();
  void refresh_Processes_List();

public:
  System();
  int getNumberOfCores() const;
  void setAttributes();
  void setLastCpuMeasures();
  string getKernelVersion() const;
  string getOsName() const;
  float getMemPercent() const;
  float getMemTotal() const;
  long getUpTime() const;
  int getThreads() const;
  int getTotalProc() const;
  int getRunningProc() const;
  float getCpuPercent() const;
  void getOtherCores(int _size);
  void setCpuCoresStats();
  vector<float>& getCoresPercent();
  vector<Process>& getProcesses();
  void refreshStats();
};

#endif