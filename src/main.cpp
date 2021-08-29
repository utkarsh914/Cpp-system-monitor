// #pragma once
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  NCursesDisplay::Display(system);

  // try {
  //   System system;
  //   NCursesDisplay::Display(system);
  // }
  // catch (...) {
  //   cout << "Exception in main function" << endl;
  // }
}