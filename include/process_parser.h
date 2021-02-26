#ifndef PROCESS_PARSER_H
#define PROCESS_PARSER_H

#include <constants.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <util.h>

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

class ProcessParser {
 public:
  static string getCmd(string pid);
  static vector<string> getPidList();
  static string getVmSize(string pid);
  static string getCpuPercent(string pid);
  static long int getSysUpTime();
  static string getProcUpTime(string pid);
  static string getProcUser(string pid);
  static vector<string> getSysCpuPercent(string coreNumber = "");
  static float getSysRamPercent();
  static string getSysKernelVersion();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static string getOsName();
  static string printCpuStats(vector<string> values1, vector<string> values2);
};

string ProcessParser::getVmSize(string pid) {
  string line;
  string name = "VmData";
  string value;
  float result;

  // open stream for specific file
  ifstream stream =
      Util::getStream(Path::basePath() + pid + Path::statusPath());

  while (getline(stream, line)) {
    // searching line by line
    // comparing starting of the line with the name (i.e. "VmData")
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      // convert from kB -> GB
      result = stof(values[1]) / float(1024 * 1024);
      break;
    }
  }

  return to_string(result);
}

string ProcessParser::getCpuPercent(string pid) {
  string line;
  ifstream stream =
      Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  float utime = stof(ProcessParser::getProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float totalTime = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  float result = 100.0 * ((totalTime / freq) / seconds);

  return to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
  string line;
  ifstream stream =
      Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  return to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}

long int ProcessParser::getSysUpTime() {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  return stol(values[0]);
}

string ProcessParser::getProcUser(string pid) {
  string line;
  string name = "Uid:";
  string uid;
  string username;

  // open stream for specific file
  ifstream stream =
      Util::getStream(Path::basePath() + pid + Path::statusPath());

  // extract user id first
  while (getline(stream, line)) {
    // searching line by line
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      uid = values[1];
      break;
    }
  }

  // get username corresponding to the userid (uid)
  stream = Util::getStream("/etc/passwd");
  name = "x:" + uid;
  while (getline(stream, line)) {
    // searching line by line
    if (line.find(name) != string::npos) {
      istringstream buf(line);
      username = line.substr(0, line.find(":"));
      // getline(buf, username, ':');
      break;
    }
  }

  return username;
}

vector<string> ProcessParser::getPidList() {
  DIR* dir;
  vector<string> pidList;
  // open "/proc" directory
  if (!(dir = opendir("/proc"))) throw runtime_error(strerror(errno));

  // one by one read the contents of "/proc"
  while (dirent* dirp = readdir(dir)) {
    // if it is not a directory, skip it
    if (!dirp->d_type != DT_DIR) continue;
    // check if every char of name is a digit
    if (all_of(dirp->d_name, dirp->d_name + strlen(dirp->d_name),
               [](char c) { return isdigit(c); })) {
      pidList.push_back(dirp->d_name);
    }
  }
  // Validating process of directory closing
  if (closedir(dir)) throw runtime_error(strerror(errno));
  return pidList;
}

#endif