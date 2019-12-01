#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

static int processToken(char* token) {
  int value = strtol(token, NULL, 10);
  return (value / 3) - 2;
}

static int processFuelToken(char* token) {
  int value = strtol(token, NULL, 10);
  int fuel = (value / 3) - 2;
  int prevFuel = fuel;
  while(1) {
    int newFuel = (prevFuel / 3) - 2;
    if(newFuel <= 0) {
      break;
    }
    fuel += newFuel;
    prevFuel = newFuel;
  }
  return fuel;
}

int main(int argc, char const *argv[])
{
  char* input = readFile("./input.txt");
  char* token = strtok(input, "\n");
  int sum = 0;
  int fuelSum = 0;

  while(token != NULL) {
    sum += processToken(token);
    fuelSum += processFuelToken(token);
    token = strtok(NULL, "\n");
  }

  printf("Part One: %d\n", sum);
  printf("Part Two: %d\n", fuelSum);

  free(input);

  return 0;
}
