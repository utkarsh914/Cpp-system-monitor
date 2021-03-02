#ifndef SYSTEM_H
#define SYSTEM_H

#include <constants.h>
// #include <util.h>
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



float System::getCpuPercent() const {
  return this->cpuPercent;
}

float System::getMemPercent() const {
  return this->memPercent;
}

long System::getUpTime() const {
  return this->upTime;
}

string System::getKernelVersion() const {
  return this->kernelVer;
}

int System::getTotalProc() const {
  return this->totalProc;
}

int System::getRunningProc() const {
  return this->runningProc;
}

int System::getThreads() const {
  return this->threads;
}

string System::getOsName() const {
  return this->osName;
}


void System::setLastCpuMeasures() {
  this->lastCpuStats = ProcessParser::getSysCpuPercent(-1);
}

/*
  This method initializes attributes of the System class.
  It takes a size, or number of cores, and initializes the object.
  Besides that, this method sets previous data for a specific CPU core.
*/
void System::getOtherCores(int _size) {
  //when number of cores is detected, vectors are modified to fit incoming data
  this->coresStats = vector<float> (_size);
  this->lastCpuCoresStats = vector<vector<string>> (_size, vector<string> ());
  this->currentCpuCoresStats = vector<vector<string>> (_size, vector<string> ());

  for (int i = 0; i < _size; i++) {
    this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(i);
  }
}

/*
  This method updates and creates new datasets for CPU calculation.
  Every core is updated and previous data becomes the current data of calculated measures.
*/
void System::setCpuCoresStats() {
  // Getting data from files (previous data is required)
  int size = this->currentCpuCoresStats.size();
  for (int i = 0; i < size; i++) {
    this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(i);
  }
  // after acquirement of data we are calculating every core percentage of usage
  for(int i = 0; i < size; i++) {
    this->coresStats[i]
      = ProcessParser::printCpuStats(this->lastCpuCoresStats[i], this->currentCpuCoresStats[i]);
  }
  this->lastCpuCoresStats = this->currentCpuCoresStats;
}

/*
  This function initializes or refreshes an object.
*/
void System::setAttributes() {
  // getting parsed data
  this->memPercent = ProcessParser::getSysRamPercent();
  this->upTime = ProcessParser::getSysUpTime();
  this->totalProc = ProcessParser::getTotalNumberOfProcesses();
  this->runningProc = ProcessParser::getNumberOfRunningProcesses();
  this->threads = ProcessParser::getTotalThreads();
  this->currentCpuStats = ProcessParser::getSysCpuPercent(-1);
  this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats, this->currentCpuStats);
  this->lastCpuStats = this->currentCpuStats;
  this->setCpuCoresStats();
}


vector<Process>& System :: getProcesses() {
  this->refresh_Processes_List();
  return this->processes;
}

void System :: refresh_Processes_List() {
  this->processes.clear();
  vector<string> pids = ProcessParser :: getPidList();
  for (auto& pid : pids) {
    this->processes.push_back(Process(pid));
  }
}

/*
  This method creates a string that represents a progress bar.
  The bar shows the current status of aggregate CPU utilization,
  or the utilization of a selected core.
*/
// vector<string> System::getCoresStats() const {
//   vector<string> result;
//   // Constructing string for every core data display
//   for (int i = 0; i < this->coresStats.size(); i++) {
//     string temp = "cpu" + to_string(i) +": ";
//     float check = stof(this->coresStats[i]);
//     if (!check || this->coresStats[i] == "nan") {
//       return vector<string>();
//     }
//     temp += Util::getProgressBar(this->coresStats[i]);
//     result.push_back(temp);
//   }
//   return result;
// }

#endif