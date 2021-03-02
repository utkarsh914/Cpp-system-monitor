#include <process_parser.h>

/*
  @parameter [string] : a unique process ID (PID)

  retrieves data for a specific running process.

  reads the file "proc/[PID]/status".
  The line “VmData” states the process's memory usage, in kB.
    VmSize: 8936 kB
*/
float ProcessParser::getVmSize(string pid) {
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

  return result;
}

/*
  @parameter [string] : a unique process ID (PID)

  retrieves percentage of CPU being used by the process.

  reads the file "proc/[PID]/stat".

  Note: Because the timing is measured in CPU clock ticks, not seconds,
  we need to convert the data from the file
*/
float ProcessParser::getCpuPercent(string pid) {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  float utime = ProcessParser::getProcUpTime(pid);
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float totalTime = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime / freq);
  float result = 100.0 * ((totalTime / freq) / seconds);

  return result;
}

/*
  @parameter [string] : a unique process ID (PID)

  retrieves uptime of the process.

  reads the file "proc/[PID]/stat".

  Note: We retrieve the system frequency for calculating the process up time.
*/
long ProcessParser::getProcUpTime(string pid) {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  return long(stof(values[13]) / sysconf(_SC_CLK_TCK));
}

/*
  @parameter [void] :

  retrieves system uptime.

  reads the file "proc/uptime".
  The line is of format -
    4239.42 31304.13
  
  first value is uptime in seconds
*/
long ProcessParser::getSysUpTime() {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);

  return stol(values[0]);
}

/*
  @parameter [string] : a unique process ID (PID)
  retrieves the username associated with the process.

  reads the file "proc/[PID]/status" to extract the owner's userID.
  The line is of format -
    Uid:    0       0       0       0
  second value is required userID.

  Now need to get the username associated with this userID
  reads "/etc/passwd". Format is -
    myDemoUserName:x:1000:1000:,,,:/home/myDemoUserName:/bin/bash
  
  need to search the line containing "x:[UID]"
*/
string ProcessParser::getProcUser(string pid) {
  string line;
  string name = "Uid:";
  string uid;
  string username;

  // open stream for specific file
  ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

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

/*
  @parameter [void] :

  retrieves a list of all the running process.

  reads the contents of "/proc/",
  and if it is a valid processID, add it to list.

  Note: valid processID directory meets 2 conditions -
    1. It is a directory (not a file)
    2. name of the directory contains only digits
*/
vector<string> ProcessParser::getPidList() {
  DIR* dir;
  vector<string> pidList;
  // open "/proc" directory
  if (!(dir = opendir("/proc"))) throw runtime_error(strerror(errno));

  // one by one read the contents of "/proc"
  while (dirent* dirp = readdir(dir)) {
    // if it is not a directory, skip it
    if (dirp->d_type != DT_DIR) continue;
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

/*
  @parameter [string] : a unique process ID (PID)

  retrieves the command that executed the process.

  reads the file "proc/[PID]/cmdline".
  It has a single line containing the command.
  The format is -
    /init/blablabla
*/
string ProcessParser::getCmd(string pid) {
  string line;
  ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
  getline(stream, line);
  return line;
}

/*
  @parameter [void] :

  retrieves the number of CPU cores on the system.

  reads the file "proc/cpuinfo".
  This file contains detailed info of all CPU cores

  so look for the line starting with "cpu cores". format is
    cpu cores       : 4
*/
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

/*
  @parameter [string] (optional) : a core number.

  retrieves the raw data of the CPU core.

  reads the file "proc/stat".

  This file contains information on overall cpu usage, as well stats for individual cores.
  Other functions ultimately use this data to calculate total, active, and idle CPU time.
  Every token from the raw data is stored as an individual value in a string vector.
  The format is -
    cpu  186663 0 110557 4171190 0 9176 0 0 0 0
    cpu0 44137 0 35898 478517 0 5664 0 0 0 0
    cpu2 42618 0 20646 495287 0 367 0 0 0 0

  Note:
    It is possible to use this method for selection of data for overall cpu or
    every core. When nothing is passed "cpu" line is read.
    When, for example "0" is passed  -> "cpu0" -> data for first core is read
*/
vector<string> ProcessParser::getSysCpuPercent(int coreNumber) {
  string line;
  string name = "cpu" + (coreNumber == -1) ? "" : to_string(coreNumber);
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

  return vector<string> ();
}

/*
  @parameter [vector<string>] :

  Following two functions are for calculating active and idle time
  and are a direct extension of the system CPU percentage.
  They sort and categorize a newly created string vector, which contains parsed raw data from file.
  Because most of the data is recorded as time, we are select and sum all active and idle time.
*/
float getSysActiveCpuTime(vector<string> values) {
  return (stof(values[S_USER])
        + stof(values[S_NICE])
        + stof(values[S_SYSTEM])
        + stof(values[S_IRQ])
        + stof(values[S_SOFTIRQ])
        + stof(values[S_STEAL])
        + stof(values[S_GUEST])
        + stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string> values) {
  return (stof(values[S_IDLE])
        + stof(values[S_IOWAIT]));
}

/*
  @parameter [vector<string>] :
  @parameter [vector<string>] :

  Calculates CPU usage, either overall or for a selected core.

  Because CPU stats can be calculated only if you take measures in two different time,
  this function has two parameters: two vectors of relevant values.
  We use a formula to calculate overall activity of processor.
*/
float ProcessParser::printCpuStats(vector<string>& values1,
                                    vector<string>& values2) {
  float activeTime =
      getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
  float totalTime = activeTime + idleTime;
  float result = 100.0 * (activeTime / totalTime);
  return result;
}

/*
  @parameter [void] :

  Reads the file "proc/memInfo",
  where we can read three characteristic values:
  MemAvailable,buffers and memFree.
  
  From these values we are calculating RAM usage in percentage.
  Line Format is -
    MemTotal:        7288000 kB
    MemFree:         3963352 kB
    Buffers:           34032 kB
*/
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

/*
  @parameter [void] :

  Reads the file "proc/version"

  Need to extract from the line starting with "Linux version "
  Line Format is -
    Linux version 4.4.0-19041-Microsoft (Microsoft@Microsoft.com) ......
  
  Third word is the kernel version.
*/
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

/*
  @parameter [void] :

  reads the file "/etc/os-release".
  
  Extract from the line starting with "PRETTY_NAME="
  Line Format is -
    PRETTY_NAME="Ubuntu 18.04.4 LTS"
  
  Note: the line doesn't have spaces, so we can't extract using stream.
*/
string ProcessParser::getOsName() {
  string line;
  string name = "PRETTY_NAME=";

  ifstream stream = Util::getStream(("/etc/os-release"));

  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      int found = line.find("=");
      found++;
      string result = line.substr(found);
      result.erase(remove(result.begin(), result.end(), '"'), result.end());
      return result;
    }
  }
  return "";
}

/*
  @parameter [void] :

  The total thread count is calculated, rather than read from a specific file.
  We open every PID folder and read its thread count. After that,
  we sum the thread counts to calculate the total number of threads on the host machine.

  for every processID:
    read the file "proc/[PID]/status"
    Extract from the line starting with "Threads:"
    Line Format is -
      Threads:        2
*/
int ProcessParser::getTotalThreads() {
  string line;
  int result = 0;
  string name = "Threads:";
  vector<string> pidList = ProcessParser::getPidList();

  for (string pid : pidList) {
    // getting every process and reading their number of their threads
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while (std::getline(stream, line)) {
      if (line.compare(0, name.size(), name) == 0) {
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> values(beg, end);
        result += stoi(values[1]);
        break;
      }
    }
  }

  return result;
}

/*
  @parameter [void] :

  Retrieves the total number of processes on the machine.

  read the file "/proc/stat"
  Extract from the line starting with "processes"
  Line Format is -
    processes 97
*/
int ProcessParser::getTotalNumberOfProcesses() {
  string line;
  int result = 0;
  string name = "processes";
  ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }

  return result;
}

/*
  @parameter [void] :

  Retrieves the total number of RUNNING PROCESSES on the machine.

  read the file "/proc/stat"
  Extract from the line starting with "procs_running"
  Line Format is -
    procs_running 11
*/
int ProcessParser::getNumberOfRunningProcesses() {
  string line;
  int result = 0;
  string name = "procs_running";
  ifstream stream = Util::getStream(Path::basePath() + Path::statPath());

  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }

  return result;
}