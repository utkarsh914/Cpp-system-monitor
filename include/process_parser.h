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
  static int getNumberOfCores();
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

string ProcessParser::getCmd(string pid) {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
  getline(stream, line);
  return line;
}

int ProcessParser::getNumberOfCores() {
  // Get the number of host cpu cores
  string line;
  string name = "cpu cores";
  ifstream stream = Util::getStream(Path::basePath() + "cpuinfo");
  int numberOfCores = 0;

  while (getline(stream, line)) {
    // searching line by line
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      numberOfCores = stoi(values[3]);
      break;
    }
  }

  return numberOfCores;
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "") {
  // It is possible to use this method for selection of data for overall cpu or
  // every core. when nothing is passed "cpu" line is read when, for example "0"
  // is passed  -> "cpu0" -> data for first core is read
  string line;
  string name = "cpu" + coreNumber;
  ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

  while (std::getline(stream, line)) {
    // if this is the required line
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      // set of cpu data active and idle times;
      return values;
    }
  }

  return vector<string>();
}

float getSysActiveCpuTime(vector<string> values) {
  return (stof(values[S_USER]) + stof(values[S_NICE]) + stof(values[S_SYSTEM]) +
          stof(values[S_IRQ]) + stof(values[S_SOFTIRQ]) +
          stof(values[S_STEAL]) + stof(values[S_GUEST]) +
          stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string> values) {
  return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

string ProcessParser::printCpuStats(vector<string> values1,
                                    vector<string> values2) {
  /*
  Because CPU stats can be calculated only if you take measures in two different
  time, this function has two parameters: two vectors of relevant values. We use
  a formula to calculate overall activity of processor.
  */
  float activeTime =
      getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
  float totalTime = activeTime + idleTime;
  float result = 100.0 * (activeTime / totalTime);
  return to_string(result);
}

float ProcessParser::getSysRamPercent() {
  string line;
  string name_1a = "MemTotal:";
  string name_1b = "MemAvailable:";
  string name_2 = "MemFree:";
  string name_3 = "Buffers:";
  float total_mem = 0;
  float free_mem = 0;
  float buffers = 0;

  ifstream stream = Util::getStream(Path::basePath() + Path::memInfoPath());

  while (getline(stream, line)) {
    if (total_mem != 0 && free_mem != 0) break;
    // find total memory
    if ((line.compare(0, name_1a.size(), name_1a) == 0) or
        (line.compare(0, name_1b.size(), name_1b) == 0)) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      total_mem = stof(values[1]);
    }
    // find free memory
    else if (line.compare(0, name_2.size(), name_2) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      free_mem = stof(values[1]);
    }
    // find buffer memory
    else if (line.compare(0, name_3.size(), name_3) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      buffers = stof(values[1]);
    }
  }

  return float(100.0 * (1 - (free_mem / (total_mem - buffers))));
}

string ProcessParser::getSysKernelVersion() {
  string line;
  string name = "Linux version ";
  ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());

  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      return values[2];
    }
  }

  return "";
}

string ProcessParser::getOsName() {
  string line;
  string name = "PRETTY_NAME=";

  ifstream stream = Util::getStream(("/etc/os-release"));

  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      // PRETTY_NAME="Ubuntu 18.04.4 LTS"   <-- this format
      // it doesn't have spaces, so we can't extract using stream
      int found = line.find("=");
      found++;
      string result = line.substr(found);
      result.erase(remove(result.begin(), result.end(), '"'), result.end());
      return result;
    }
  }
  return "";
}

#endif