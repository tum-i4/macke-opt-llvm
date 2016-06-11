#include <stdlib.h>

int divisible_by_2(int i) {
  return (i & 1) == 0;
}

int divisible_by_3(int i) {
  return i % 3 == 0;
}

int divisible_by_5(int i) {
  return i % 5 == 0;
}

int divisible_by_6(int i) {
  return divisible_by_2(i) && divisible_by_3(i);
}

int divisible_by_10(int i) {
  return divisible_by_2(i) && divisible_by_5(i);
}

int divisible_by_30(int i) {
  return divisible_by_3(i) && divisible_by_10(i);
}

int main(int argc, char **argv) {
  int n = 42;

  if (argc == 2) {
    n = atoi(argv[1]);
  }

  return divisible_by_6(n) || divisible_by_10(n) || divisible_by_30(n);
}
