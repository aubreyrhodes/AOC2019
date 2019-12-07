#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static bool isDouble(int password) {
  char buffer[8];
  snprintf(buffer, 8, "%d", password);
  for(int i = 0; i < 8; i++) {
    if(buffer[i] == '\0') return false;
    if(buffer[i] == buffer[i+1]) return true;
  }
  return false;
}

static bool isSingleDouble(int password) {
  char buffer[8];
  char skip;
  snprintf(buffer, 8, "%d", password);
  for(int i = 0; i < 8; i++) {
    if(skip == buffer[i]) continue;
    if(buffer[i + 1] == '\0') return false;
    if(buffer[i] == buffer[i+1]) {
      if(buffer[i] != buffer[i+2]) return true;
      skip = buffer[i];
    }
  }
  return false;
}

static bool isIncreasing(int password) {
  char buffer[8];
  snprintf(buffer, 8, "%d", password);
  for(int i = 0; i < 8; i++) {
    if(buffer[i + 1] == '\0') return true;
    if(buffer[i] > buffer[i+1]) return false;
  }
  return false;
}

static bool check(int password) {
  return isDouble(password) && isIncreasing(password);
}

static bool check2(int password) {
  return isSingleDouble(password) && isIncreasing(password);
}

int main(int argc, char const *argv[])
{
  int numFound1 = 0;
  int numFound2 = 0;
  for(uint64_t i = 271973; i <= 785961; i++) {
    if(check(i)) numFound1++;
    if(check2(i)) numFound2++;
  }

  printf("Solution 1: %d\n", numFound1);
  printf("Solution 2: %d\n", numFound2);

  return 0;
}
