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
  vector<string> lastCpuStats;
  vector<string> currentCpuStats;
  vector<float> coresStats;
  vector<vector<string>> lastCpuCoresStats;
  vector<vector<string>> currentCpuCoresStats;
  string osName;
  string kernelVer;
  float cpuPercent;
  float memPercent;
  long upTime;
  int totalProc;
  int runningProc;
  int threads;
  vector<Process> processes;

public:
  /*
    Getting initial info about system
    Initial data for individual cores is set
    System data is set
  */
  System() {
    this->setLastCpuMeasures();
    this->getOtherCores(ProcessParser::getNumberOfCores());
    this->setAttributes();
    this->osName = ProcessParser::getOsName();
    this->kernelVer = ProcessParser::getSysKernelVersion();
  }
  void setAttributes();
  void setLastCpuMeasures();
  string getKernelVersion() const;
  string getOsName() const;
  float getMemPercent() const;
  long getUpTime() const;
  int getThreads() const;
  int getTotalProc() const;
  int getRunningProc() const;
  float getCpuPercent() const;
  void getOtherCores(int _size);
  void setCpuCoresStats();
  // vector<float> getCoresStats() const;
  vector<Process>& getProcesses();
  void refresh_Processes_List();
};

#endif