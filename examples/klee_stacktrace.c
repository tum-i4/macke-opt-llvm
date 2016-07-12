#include "klee/klee.h"


int bar() {
  klee_stack_trace();
  return 0;
}

void foo() {
  bar();
}

int main(int argc, char** argv) {
  foo();
}
