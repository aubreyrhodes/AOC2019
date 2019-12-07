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

static void runMachine(int* memory, int input) {
  int ip = 0;

  for(;;) {
    int instr = memory[ip];
    int op = instr % 100;
    int modeC = (instr / 100) % 10;
    int modeB = (instr / 1000) % 10;
    int modeA = (instr / 10000) % 10;
    switch(op) {
      case 1: {
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        int pos3 = memory[ip + 3];
        memory[pos3] = memory[pos1] + memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        ip = ip + 4;
        break;
      }
      case 2: {        
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        int pos3 = memory[ip + 3];
        memory[pos3] = memory[pos1] * memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        ip = ip + 4;
        break;
      }
      case 3: {
        if (modeC == 0) {
          int pos1 = memory[ip+1];
          memory[pos1] = input;
          // printf("Updated pos: %d with value: %d\n", pos1, input);
        } else {
          printf("Don't how to get input\n");
        }
        ip = ip + 2;
        break;
      }
      case 4: {
        if (modeC == 1) {
          printf("%d\n", memory[ip+1]);
        } else {
          printf("%d\n", memory[memory[ip+1]]);
        }
        ip = ip + 2;
        break;
      }
      case 5: {
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        if(memory[pos1] != 0) {
          // printf("Jumping to: %d\n", pos2);
          ip = memory[pos2];
        } else {
          ip = ip + 3;
        }
        break;
      }
      case 6: {
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        if(memory[pos1] == 0) {
          // printf("Jumping to: %d\n", pos2);
          ip = memory[pos2];
        } else {
          ip = ip + 3;
        }
        break;
      }
      case 7: {
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        int pos3 = (modeA == 1) ? (ip + 3) : memory[ip + 3];
        if(memory[pos1] < memory[pos2]) {
          // printf("Setting pos: %d with value 1\n", pos3);
          memory[pos3] = 1;
        } else {
          // printf("Setting pos: %d with value 0\n", pos3);
          memory[pos3] = 0;
        }
        ip = ip + 4;
        break;
      }
      case 8: {
        int pos1 = (modeC == 1) ? (ip + 1) : memory[ip + 1];
        int pos2 = (modeB == 1) ? (ip + 2) : memory[ip + 2];
        int pos3 = (modeA == 1) ? (ip + 3) : memory[ip + 3];
        if(memory[pos1] == memory[pos2]) {
          // printf("Setting pos: %d with value 1\n", pos3);
          memory[pos3] = 1;
        } else {
          // printf("Setting pos: %d with value 0\n", pos3);
          memory[pos3] = 0;
        }
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
  runMachine(memory, 5);

  free(source);
  free(memory);
  return 0;
}
