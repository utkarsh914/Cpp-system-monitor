#ifndef PROCESS_H
#define PROCESS_H

#include <process_parser.h>
#include <string>

// using std::string;
using namespace std;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
  // Declare any necessary private members
private:
  string Pid_;
  string User_;
  string Command_;
  float CpuUtilization_;
  float Ram_;
  long UpTime_;

public:
  Process(string pid) {
    this->Pid_ = pid;
    this->User_ = ProcessParser::getProcUser(pid);
    this->Command_ = ProcessParser::getCmd(pid);
    this->CpuUtilization_ = ProcessParser::getCpuPercent(pid);
    this->Ram_ = ProcessParser::getVmSize(pid);
    this->UpTime_ = ProcessParser::getProcUpTime(pid);
  }
  // getters
  // string getProcess();
  string getPid();                            // See src/process.cpp
  string getUser();                           // See src/process.cpp
  string getCommand();                        // See src/process.cpp
  float getCpuUtilization();                  // See src/process.cpp
  float getRam();                             // See src/process.cpp
  long getUpTime();                           // See src/process.cpp
};



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

#endif