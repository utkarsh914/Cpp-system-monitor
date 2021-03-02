#include <system.h>

/*
  Getting initial info about system
  Initial data for individual cores is set
  System data is set
*/
System :: System() {
  this->numberOfCores = ProcessParser::getNumberOfCores();
  this->osName = ProcessParser::getOsName();
  this->kernelVer = ProcessParser::getSysKernelVersion();
  //when number of cores is detected, vectors are modified to fit incoming data
  int _size = this->numberOfCores;
  this->coresPercent = vector<float> (_size);
  this->lastCpuCoresStats = vector<vector<string>> (_size, vector<string> ());
  this->currentCpuCoresStats = vector<vector<string>> (_size, vector<string> ());

  this->refreshStats();
}

void System :: refreshStats() {
  // getting parsed data
  this->memPercent = ProcessParser::getSysRamPercent();
  this->upTime = ProcessParser::getSysUpTime();
  this->totalProc = ProcessParser::getTotalNumberOfProcesses();
  this->runningProc = ProcessParser::getNumberOfRunningProcesses();
  this->threads = ProcessParser::getTotalThreads();
  // refresh cpu and cores usage
  this->calculateCpuStats();
  this->calculateCpuCoresStats();
  // refresh process lists
  this->refresh_Processes_List();
}

void System::calculateCpuStats() {
  // set last stats
  this->lastCpuStats = ProcessParser::getSysCpuPercent(-1);
  // set current stats
  this->currentCpuStats = ProcessParser::getSysCpuPercent(-1);
  // calculate cpu percent
  this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats, this->currentCpuStats);
  // set last stats equal to current
  // this->lastCpuStats = this->currentCpuStats;
}

/*
  This method updates and creates new datasets for CPU calculation.
  Every core is updated and previous data becomes the current data of calculated measures.
*/
void System::calculateCpuCoresStats() {
  int size = this->numberOfCores;
  // get last cpu cores stats
  for (int i = 0; i < size; i++) {
    this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(i);
  }
  // Getting new stats from files (previous data is required)
  for (int i = 0; i < size; i++) {
    this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(i);
  }
  // after acquirement of data we are calculating every core percentage of usage
  for(int i = 0; i < size; i++) {
    this->coresPercent[i]
      = ProcessParser::printCpuStats(this->lastCpuCoresStats[i], this->currentCpuCoresStats[i]);
  }
  // set last stats equal to current
  // this->lastCpuCoresStats = this->currentCpuCoresStats;
}

void System :: refresh_Processes_List() {
  this->processes.clear();
  vector<string> pids = ProcessParser :: getPidList();
  for (auto& pid : pids) {
    this->processes.push_back(Process(pid));
  }
  // sort process list by RAM usage
  sort(this->processes.begin(), this->processes.end(), [](auto& p1, auto& p2) {
    return p1.getRam() > p2.getRam();
    // return p1.getCpuUtilization() > p2.getCpuUtilization();
  });
}



// getters
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
vector<Process>& System :: getProcesses() {
  return this->processes;
}


/*
  This method creates a string that represents a progress bar.
  The bar shows the current status of aggregate CPU utilization,
  or the utilization of a selected core.
*/
// vector<string> System::getCoresStats() const {
//   vector<string> result;
//   // Constructing string for every core data display
//   for (int i = 0; i < this->coresPercent.size(); i++) {
//     string temp = "cpu" + to_string(i) +": ";
//     float check = stof(this->coresPercent[i]);
//     if (!check || this->coresPercent[i] == "nan") {
//       return vector<string>();
//     }
//     temp += Util::getProgressBar(this->coresPercent[i]);
//     result.push_back(temp);
//   }
//   return result;
// }
