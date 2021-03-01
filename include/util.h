#include <fstream>
#include <string>

using namespace std;

class Util {
 public:
  static string convertToTime(string input);
  static string getProgressBar(string percent);
  static ifstream getStream(string path);
};

string Util::convertToTime(string input) {
  long int input_seconds = stol(input);
  long minutes = input_seconds / 60;
  long hours = minutes / 60;
  long seconds = int(input_seconds % 60);
  minutes = minutes % 60;
  return to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
}

string Util::getProgressBar(string percent) {
  string result = "0% ";
  int size_ = 50;
  int boundaries = (stof(percent) / 100) * size_;

  for (int i = 0; i <= size_; i++) {
    if (i <= boundaries)
      result += "|";
    else
      result += " ";
  }

  result += " " + percent.substr(0, 5) + " /100%";
  return result;
}

ifstream Util::getStream(string path) {
  ifstream stream(path);
  if (!stream) throw runtime_error("Non - existing PID");
  return stream;
}