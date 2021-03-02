#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "ncurses_display.h"
#include "system.h"

using std::string;
using std::to_string;

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {

  int row{0};

  mvwprintw(window, ++row, 2, ("OS: " + system.getOsName()) .c_str());

  mvwprintw(window, ++row, 2, ("Kernel: " + system.getKernelVersion()) .c_str());

  // mvwprintw(window, ++row, 2, "CPU: ");
  // wattron(window, COLOR_PAIR(1));
  // mvwprintw(window, row, 10, "");
  // wprintw(window, Util::getProgressBar(system.getCpuPercent()) .c_str());
  // wattroff(window, COLOR_PAIR(1));
  auto& coresPercent = system.getCoresPercent();
  for (int i = 0; i < system.getNumberOfCores(); i++) {
    mvwprintw(window, ++row, 2, ("CPU_" + to_string(i) + ": ") .c_str());
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, row, 10, "");
    wprintw(window, (Util::getProgressBar(coresPercent[i])
                    + " " + to_string(coresPercent[i]).substr(0,4) + "%") .c_str());
    wattroff(window, COLOR_PAIR(1));
  }

  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");

  string ramInfo = to_string(system.getMemPercent() * system.getMemTotal() / 100.0).substr(0, 4)
                  + "/" + to_string(system.getMemTotal()).substr(0, 4) + " [GB]";
  wprintw(window, (Util::getProgressBar(system.getMemPercent()) + " " + ramInfo) .c_str());
  wattroff(window, COLOR_PAIR(1));

  mvwprintw(window, ++row, 2, ("Total Processes: " + to_string(system.getTotalProc())) .c_str());

  mvwprintw(window, ++row, 2, ("Running Processes: " + to_string(system.getRunningProc())) .c_str());

  mvwprintw(window, ++row, 2, ("Up Time: " + Util::convertToTime(system.getUpTime())) .c_str());

  wrefresh(window);

}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n) {
  if ((int)processes.size() < n) n = (int)processes.size();
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));

  // print table header
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));

  // print every process
  for (int i = 0; i < n; ++i) {
    auto& p = processes[i];
    mvwprintw(window, ++row, pid_column, p.getPid() .c_str());
    mvwprintw(window, row, user_column, p.getUser() .c_str());
    mvwprintw(window, row, cpu_column, to_string(p.getCpuUtilization()).substr(0, 5) .c_str());
    mvwprintw(window, row, ram_column, to_string(p.getRam()).substr(0, 5) .c_str());
    mvwprintw(window, row, time_column, Util::convertToTime(p.getUpTime()) .c_str());
    mvwprintw(window, row, command_column, p.getCommand().substr(0, window->_maxx - 46) .c_str());
  }

}

void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int x_max{getmaxx(stdscr)};
  WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

  while (1) {
    // refresh system stats (including process list)
    system.refreshStats();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    DisplaySystem(system, system_window);
    DisplayProcesses(system.getProcesses(), process_window, n);
    wrefresh(system_window);
    wrefresh(process_window);
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}