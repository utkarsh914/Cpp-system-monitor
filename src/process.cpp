#include <process.h>

Process :: Process(string pid) {
  this->Pid_ = pid;
  this->User_ = ProcessParser::getProcUser(pid);
  this->Command_ = ProcessParser::getCmd(pid);
  this->CpuUtilization_ = ProcessParser::getCpuPercent(pid);
  this->Ram_ = ProcessParser::getVmSize(pid);
  this->UpTime_ = ProcessParser::getProcUpTime(pid);
}

// Return this process's ID
string Process::getPid() {
  return this->Pid_;
}

// Return the user (name) that generated this process
string Process::getUser() {
  return this->User_;
}

// Return this process's CPU utilization
float Process::getCpuUtilization() {
  return this->CpuUtilization_;
}

// Return this process's memory utilization
float Process::getRam() {
  return this->Ram_;
}

// Return the age of this process (in seconds)
long Process::getUpTime() {
  return this->UpTime_;
}

// Return the command that generated this process
string Process::getCommand() {
  return this->Command_;
}