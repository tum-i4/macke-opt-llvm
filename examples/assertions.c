// Functions for each C data type
// see https://en.wikipedia.org/wiki/C_data_types

#include <assert.h>
#include <stdbool.h>


void singlechar(unsigned char c) {
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
  assert(n != 12);
  assert(i != 13);
  assert((n * -1) != i + 1);
}

void indirect(int i) {
  singleint(i);
}

void singlepointer(int *i) {
  assert(*i != 42);
}

struct mypair {
  int i;
  unsigned char c;
};

void singlestruct(struct mypair p) {
  assert(p.i != 21);
  assert(p.c != 'h');
  assert(p.i != 1 || p.c != 'e');
}

void singlestructpointer(struct mypair *p) {
  assert(p->i != 21);
  assert(p->c != 'h');
  assert(p->i != 1 || p->c != 'e');
}

int strcmp(const char *s1, const char *s2) {
  for (; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
      return 0;
  return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

void simplestring(char *str) {
  const char greeting[] = "Hi";
  assert(strcmp(str, greeting) != 0);
}

void singlebool(bool a) {
  assert(a);
}

void compareints(int a, int b) {
  assert(a <= b);
}

void compareintscomplex(int a, int b) {
  int n = a * 2;
  assert(n <= b);
}

void twopointers(unsigned char* a, unsigned char* b) {
  assert(!(a[0] == 'A' && b[0] == 'B'));
}
