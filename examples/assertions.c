// Functions for each C data type
// see https://en.wikipedia.org/wiki/C_data_types

#include <assert.h>
#include <stdbool.h>


void singlechar(char c) {
  assert(c != 'x');
}

void largeint(unsigned long long n) {
  assert(n != 9223372036854775807);
}

void singledouble(double d) {
  assert(d != 3.1415);
}

void singleint(int i) {
  assert(i != -1);
}

int withreturn(int i) {
  assert(i != 42);
  return i;
}

void twoints(unsigned int n, int i) {
  assert(n != 1);
  assert(i != 0);
  assert((n * -1) != i);
}

void indirect(int i) {
  singleint(i);
}

void singlepointer(int *i) {
  assert(i);
}

struct mypair {
  int i;
  char c;
};

void singlestruct(struct mypair p) {
  assert(p.i != 21);
  assert(p.c != 'h');
  assert(p.i != 1 && p.c != 'e');
}

int strcmp(const char *s1, const char *s2) {
  for (; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
      return 0;
  return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

void simplestring(char *str) {
  char greeting[] = "Hi";
  assert(strcmp(str, greeting) != 0);
}

void singlebool(bool a) {
  assert(a);
}
