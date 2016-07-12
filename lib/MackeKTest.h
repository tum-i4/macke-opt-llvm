#ifndef LIB_MACKEKTEST_H_
#define LIB_MACKEKTEST_H_

#include <string>
#include <vector>
#include "klee/Internal/ADT/KTest.h"

struct MackeKTestObject {
  std::string name;
  std::vector<unsigned char> value;

  MackeKTestObject(std::string _name, std::vector<unsigned char> _value)
      : name(_name), value(_value) {
    /* Just set the variables */
  }
};

class MackeKTest {
  friend std::ostream& operator<<(std::ostream& os, const MackeKTest& mackektest);

 public:
  std::vector<MackeKTestObject> objects;

  explicit MackeKTest(const char* ktestfile);
};

std::ostream& operator<<(std::ostream& os, const MackeKTest& mackektest);

#endif  // LIB_MACKEKTEST_H_
