#include "klee/klee.h"
#include <assert.h>

void faulty(int a) {
  assert(a != 21);
  assert(a != 42);
  assert(a != 1337);
}

int main() {
  int a = klee_int("a");
  faulty(a);
  return 0;
}
