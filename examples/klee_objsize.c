#include "klee/klee.h"
#include <assert.h>
#include <stdio.h>


void assert4allSizes(int* ptr) {
  printf("Memory size: %d\n", klee_get_obj_size(ptr));
  klee_assert((1 * sizeof(int)) != klee_get_obj_size(ptr));
  klee_assert((2 * sizeof(int)) != klee_get_obj_size(ptr));
  klee_assert((4 * sizeof(int)) != klee_get_obj_size(ptr));
  klee_assert((16 * sizeof(int)) != klee_get_obj_size(ptr));
  klee_assert((256 * sizeof(int)) != klee_get_obj_size(ptr));
}
