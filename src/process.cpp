#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

// using std::string;
// using std::to_string;
// using std::vector;
using namespace std;

string Process::getProcess() {
  string pid = this->Pid_;
  this->CpuUtilization_ = ProcessParser::getCpuPercent(pid);
  this->Ram_ = ProcessParser::getVmSize(pid);
  this->UpTime_ = ProcessParser::getProcUpTime(pid);

  return (this->Pid_ + " "
        + this->User_ + " "
        + this->Ram_.substr(0, 5) + " "
        + this->CpuUtilization_.substr(0, 5) + " "
        + this->UpTime_.substr(0, 5) + " "
        + this->Command_.substr(0, 30) + "... " );
}

// Return this process's ID
string Process::getPid() {
  return this->Pid_;
}

// Return this process's CPU utilization
string Process::getCpuUtilization() {
  return this->CpuUtilization_;
}

// Return the command that generated this process
string Process::getCommand() {
  return this->Command_;
}

// Return this process's memory utilization
string Process::getRam() {
  return this->Ram_;
}

// Return the user (name) that generated this process
string Process::getUser() {
  return this->User_;
}

// Return the age of this process (in seconds)
string Process::getUpTime() {
  return this->UpTime_;
}


// ********* setters *********
void Process::setPid(string pid) {
  this->Pid_ = pid;
}


// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }