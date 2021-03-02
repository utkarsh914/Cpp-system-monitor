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
  Process(string pid);
  // getters
  string getPid();                            // See src/process.cpp
  string getUser();                           // See src/process.cpp
  string getCommand();                        // See src/process.cpp
  float getCpuUtilization();                  // See src/process.cpp
  float getRam();                             // See src/process.cpp
  long getUpTime();                           // See src/process.cpp
};

#endif