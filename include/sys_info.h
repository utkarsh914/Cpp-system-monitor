#ifndef SYS_INFO_H
#define SYS_INFO_H

#include <constants.h>
#include <util.h>
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


class SysInfo {
private:
  vector<string> lastCpuStats;
  vector<string> currentCpuStats;
  vector<string> coresStats;
  vector<vector<string>> lastCpuCoresStats;
  vector<vector<string>> currentCpuCoresStats;
  string cpuPercent;
  float memPercent;
  string osName;
  string kernelVer;
  long upTime;
  int totalProc;
  int runningProc;
  int threads;

public:
  SysInfo() {
    /*
      Getting initial info about system
      Initial data for individual cores is set
      System data is set
    */
    this->setLastCpuMeasures();
    this->getOtherCores(ProcessParser::getNumberOfCores());
    this->setAttributes();
    this->osName = ProcessParser::getOsName();
    this->kernelVer = ProcessParser::getSysKernelVersion();
  }
  void setAttributes();
  void setLastCpuMeasures();
  string getMemPercent() const;
  long getUpTime() const;
  string getThreads() const;
  string getTotalProc() const;
  string getRunningProc() const;
  string getKernelVersion() const;
  string getOsName() const;
  string getCpuPercent() const;
  void getOtherCores(int _size);
  void setCpuCoresStats();
  vector<string> getCoresStats() const;
};



string SysInfo::getCpuPercent() const {
  return this->cpuPercent;
}

string SysInfo::getMemPercent() const {
  return to_string(this->memPercent);
}

long SysInfo::getUpTime() const {
  return this->upTime;
}

string SysInfo::getKernelVersion() const {
  return this->kernelVer;
}

string SysInfo::getTotalProc() const {
  return to_string(this->totalProc);
}

string SysInfo::getRunningProc() const {
  return to_string(this->runningProc);
}

string SysInfo::getThreads() const {
  return to_string(this->threads);
}

string SysInfo::getOsName() const {
  return this->osName;
}


void SysInfo::setLastCpuMeasures() {
  this->lastCpuStats = ProcessParser::getSysCpuPercent();
}

/*
  This method initializes attributes of the SysInfo class.
  It takes a size, or number of cores, and initializes the object.
  Besides that, this method sets previous data for a specific CPU core.
*/
void SysInfo::getOtherCores(int _size) {
  //when number of cores is detected, vectors are modified to fit incoming data
  this->coresStats = vector<string> (_size);
  this->lastCpuCoresStats = vector<vector<string>> (_size, vector<string> ());
  this->currentCpuCoresStats = vector<vector<string>> (_size, vector<string> ());

  for (int i = 0; i < _size; i++) {
    this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));
  }
}

/*
  This method updates and creates new datasets for CPU calculation.
  Every core is updated and previous data becomes the current data of calculated measures.
*/
void SysInfo::setCpuCoresStats() {
  // Getting data from files (previous data is required)
  for(int i = 0; i < this->currentCpuCoresStats.size(); i++) {
    this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(to_string(i));
  }
  // after acquirement of data we are calculating every core percentage of usage
  for(int i = 0; i < this->currentCpuCoresStats.size(); i++) {
    this->coresStats[i]
      = ProcessParser::printCpuStats(this->lastCpuCoresStats[i], this->currentCpuCoresStats[i]);
  }
  this->lastCpuCoresStats = this->currentCpuCoresStats;
}

/*
  This function initializes or refreshes an object.
*/
void SysInfo::setAttributes() {
  // getting parsed data
  this->memPercent = ProcessParser::getSysRamPercent();
  this->upTime = ProcessParser::getSysUpTime();
  this->totalProc = ProcessParser::getTotalNumberOfProcesses();
  this->runningProc = ProcessParser::getNumberOfRunningProcesses();
  this->threads = ProcessParser::getTotalThreads();
  this->currentCpuStats = ProcessParser::getSysCpuPercent();
  this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats, this->currentCpuStats);
  this->lastCpuStats = this->currentCpuStats;
  this->setCpuCoresStats();
}

/*
  This method creates a string that represents a progress bar.
  The bar shows the current status of aggregate CPU utilization,
  or the utilization of a selected core.
*/
vector<string> SysInfo::getCoresStats() const {
  vector<string> result;
  // Constructing string for every core data display
  for (int i = 0; i < this->coresStats.size(); i++) {
    string temp = "cpu" + to_string(i) +": ";
    float check = stof(this->coresStats[i]);
    if (!check || this->coresStats[i] == "nan") {
      return vector<string>();
    }
    temp += Util::getProgressBar(this->coresStats[i]);
    result.push_back(temp);
  }
  return result;
}

#endif