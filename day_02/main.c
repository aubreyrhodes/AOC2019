#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

static int countCommands(char* source) {
  int sum = 0;
  int i = 0;

  for(;;) {
    if(source[i] == ',') sum++;
    if(source[i] == '\0') return sum + 1; // account for last command without a trailing ,
    i++;
  }
}

static void loadMemory(int* memory, char* source) {
  char* token = strtok(source, ",");
  int i = 0;
  while(token != NULL) {
    memory[i] = strtol(token, NULL, 10);
    i++;
    token = strtok(NULL, ",");
  }
}

static void runMachine(int* memory, int noun, int verb) {
  int ip = 0;
  memory[1] = noun;
  memory[2] = verb;

  for(;;) {
    switch(memory[ip]) {
      case 1: {
        int pos1 = memory[ip + 1];
        int pos2 = memory[ip + 2];
        int pos3 = memory[ip + 3];
        memory[pos3] = memory[pos1] + memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        ip = ip + 4;
        break;
      }
      case 2: {        
        int pos1 = memory[ip + 1];
        int pos2 = memory[ip + 2];
        int pos3 = memory[ip + 3];
        memory[pos3] = memory[pos1] * memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        ip = ip + 4;
        break;
      }
      case 99: {
        return;
      }
      default: {
        printf("Got unkown opcode: %d at ip: %d\n", memory[ip], ip);
        return;
      }
    }
  }
}

int main(int argc, char const *argv[])
{
  char* source = readFile("./input.txt");

  int numCommands = countCommands(source);
  int* memory = malloc(numCommands * sizeof(int));

  loadMemory(memory, source);
  runMachine(memory, 12, 2);

  printf("Solution 1: %d\n", memory[0]);

  for(int i=0; i<=99;i++) {
    for(int j=0; j<=99; j++) {
      char* newSource = readFile("./input.txt");
      loadMemory(memory, newSource);
      runMachine(memory, i, j);
      // printf("Tried %d and %d\n", i, j);
      if(memory[0] == 19690720) {
        printf("Solution2: %d, noun: %d, verb: %d\n", (100 * i) + j, i, j);
        free(newSource);
        break;
      }
      free(newSource);
    }
  }

  free(source);
  free(memory);
  return 0;
}
