#include "klee/klee.h"


int bar(int n) {
  klee_stack_trace();
  return n;
}

void foo(int n) {
  bar(n);
}

int main(int argc, char** argv) {
  foo(argc);
}
