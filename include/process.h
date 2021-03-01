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
  string CpuUtilization_;
  string Ram_;
  string UpTime_;

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
  string getProcess();
  string getPid();                            // See src/process.cpp
  string getUser();                           // See src/process.cpp
  string getCommand();                        // See src/process.cpp
  string getCpuUtilization();                 // See src/process.cpp
  string getRam();                            // See src/process.cpp
  string getUpTime();                         // See src/process.cpp
  // setters
  void setPid(string pid);
  bool operator<(Process const& a) const;  // See src/process.cpp
};

#endif