#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "MackeKTest.h"


int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Please give a .ktest file as a parameter" << std::endl;
    return -1;
  } else {
    if (!std::ifstream(argv[1]).good()) {
      std::cerr << "Invalid filename" << std::endl;
      return 1;
    } else {
      std::cout << MackeKTest(argv[1]);
      return 0;
    }
  }
}
