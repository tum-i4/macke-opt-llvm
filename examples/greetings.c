#include <stdio.h>

int english(void) {
  puts("Hello World");
  return 0;
}

int german(void) {
  puts("Hallo Welt!");
  return 0;
}

int french(void) {
  puts("Bonjour le monde!");
  return 0;
}

int spanish(void) {
  puts("Hola mundo");
  return 0;
}

int main(void) {
  return english();
}
