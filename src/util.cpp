#include <util.h>

string Util::convertToTime(long seconds) {
  long hours = seconds / 3600;
  seconds -= hours * 3600;
  long minutes = seconds / 60;
  seconds -= minutes * 60;
  string hh = to_string(hours);
  string mm = to_string(minutes);
  string ss = to_string(seconds);
  if (hours < 10) hh = "0" + hh;
  if (minutes < 10) mm = "0" + mm;
  if (seconds < 10) ss = "0" + ss;
  return hh + ":" + mm + ":" + ss;
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
