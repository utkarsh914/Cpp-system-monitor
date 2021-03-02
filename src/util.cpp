#include <util.h>

string Util::convertToTime(long input_seconds) {
  long minutes = input_seconds / 60;
  long hours = minutes / 60;
  long seconds = input_seconds % 60;
  minutes = minutes % 60;
  return to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
}

string Util::getProgressBar(float percent) {
  string result = "";
  int size_ = 50;
  int boundaries = (percent / 100) * size_;

  for (int i = 0; i <= size_; i++) {
    if (i <= boundaries)
      result += "|";
    else
      result += " ";
  }

  result += " " + to_string(percent).substr(0, 5) + " /100%";
  return result;
}

ifstream Util::getStream(string path) {
  ifstream stream(path);
  if (!stream) throw runtime_error("Non - existing PID");
  return stream;
}
