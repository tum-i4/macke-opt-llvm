#include "MackeKTest.h"
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "klee/Internal/ADT/KTest.h"

MackeKTest::MackeKTest(const char* ktestfile) {
  // Test, if everything is readable
  assert(kTest_getCurrentVersion() == 3);
  assert(kTest_isKTestFile(ktestfile));

  // Read test from file into primitive klee struct
  KTest* ktest = kTest_fromFile(ktestfile);

  // Read the list of objects
  for (int i = 0; i < ktest->numObjects; i++) {
    std::vector<unsigned char> payload{};

    // Foreach object, load the complete payload
    for (int j = 0; j < ktest->objects[i].numBytes; j++) {
      payload.emplace_back(ktest->objects[i].bytes[j]);
    }
    objects.emplace_back(ktest->objects[i].name, payload);
  }

  // Free memory of primitive klee struct
  free(ktest);
}


std::ostream& operator<<(std::ostream& os, const MackeKTest& mackektest) {
  for (auto& elem : mackektest.objects) {
    os << " " << elem.name << " = ";

    os << "0x";
    for (auto& v : elem.value) {
      os << std::hex << std::setfill('0') << std::setw(2) << int(v);
    }
    os << std::endl;
  }
}
