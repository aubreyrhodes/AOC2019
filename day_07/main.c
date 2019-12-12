#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "file.h"

static int perms[120][5] = {
    {0,1,2,3,4},
    {1,0,2,3,4},
    {2,0,1,3,4},
    {0,2,1,3,4},
    {1,2,0,3,4},
    {2,1,0,3,4},
    {2,1,3,0,4},
    {1,2,3,0,4},
    {3,2,1,0,4},
    {2,3,1,0,4},
    {1,3,2,0,4},
    {3,1,2,0,4},
    {3,0,2,1,4},
    {0,3,2,1,4},
    {2,3,0,1,4},
    {3,2,0,1,4},
    {0,2,3,1,4},
    {2,0,3,1,4},
    {1,0,3,2,4},
    {0,1,3,2,4},
    {3,1,0,2,4},
    {1,3,0,2,4},
    {0,3,1,2,4},
    {3,0,1,2,4},
    {4,0,1,2,3},
    {0,4,1,2,3},
    {1,4,0,2,3},
    {4,1,0,2,3},
    {0,1,4,2,3},
    {1,0,4,2,3},
    {1,0,2,4,3},
    {0,1,2,4,3},
    {2,1,0,4,3},
    {1,2,0,4,3},
    {0,2,1,4,3},
    {2,0,1,4,3},
    {2,4,1,0,3},
    {4,2,1,0,3},
    {1,2,4,0,3},
    {2,1,4,0,3},
    {4,1,2,0,3},
    {1,4,2,0,3},
    {0,4,2,1,3},
    {4,0,2,1,3},
    {2,0,4,1,3},
    {0,2,4,1,3},
    {4,2,0,1,3},
    {2,4,0,1,3},
    {3,4,0,1,2},
    {4,3,0,1,2},
    {0,3,4,1,2},
    {3,0,4,1,2},
    {4,0,3,1,2},
    {0,4,3,1,2},
    {0,4,1,3,2},
    {4,0,1,3,2},
    {1,0,4,3,2},
    {0,1,4,3,2},
    {4,1,0,3,2},
    {1,4,0,3,2},
    {1,3,0,4,2},
    {3,1,0,4,2},
    {0,1,3,4,2},
    {1,0,3,4,2},
    {3,0,1,4,2},
    {0,3,1,4,2},
    {4,3,1,0,2},
    {3,4,1,0,2},
    {1,4,3,0,2},
    {4,1,3,0,2},
    {3,1,4,0,2},
    {1,3,4,0,2},
    {2,3,4,0,1},
    {3,2,4,0,1},
    {4,2,3,0,1},
    {2,4,3,0,1},
    {3,4,2,0,1},
    {4,3,2,0,1},
    {4,3,0,2,1},
    {3,4,0,2,1},
    {0,4,3,2,1},
    {4,0,3,2,1},
    {3,0,4,2,1},
    {0,3,4,2,1},
    {0,2,4,3,1},
    {2,0,4,3,1},
    {4,0,2,3,1},
    {0,4,2,3,1},
    {2,4,0,3,1},
    {4,2,0,3,1},
    {3,2,0,4,1},
    {2,3,0,4,1},
    {0,3,2,4,1},
    {3,0,2,4,1},
    {2,0,3,4,1},
    {0,2,3,4,1},
    {1,2,3,4,0},
    {2,1,3,4,0},
    {3,1,2,4,0},
    {1,3,2,4,0},
    {2,3,1,4,0},
    {3,2,1,4,0},
    {3,2,4,1,0},
    {2,3,4,1,0},
    {4,3,2,1,0},
    {3,4,2,1,0},
    {2,4,3,1,0},
    {4,2,3,1,0},
    {4,1,3,2,0},
    {1,4,3,2,0},
    {3,4,1,2,0},
    {4,3,1,2,0},
    {1,3,4,2,0},
    {3,1,4,2,0},
    {2,1,4,3,0},
    {1,2,4,3,0},
    {4,2,1,3,0},
    {2,4,1,3,0},
    {1,4,2,3,0},
    {4,1,2,3,0},
};

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

static int64_t runMachine(int* memory, int input1, int input2, bool* halted, int* extIp, bool* gotFirst) {
  int ip = *extIp;
  int64_t output = -1;
  bool got1 = *gotFirst;

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
          memory[pos1] = got1 ? input2 : input1;
          if(!got1) {
              got1 = true;
              *gotFirst = true;
          }
          // printf("Updated pos: %d with value: %d\n", pos1, input);
        } else {
          printf("Don't how to get input\n");
        }
        ip = ip + 2;
        break;
      }
      case 4: {
        if (modeC == 1) {
            output = memory[ip+1];
        } else {
            output = memory[memory[ip+1]];
        }
        ip = ip + 2;
        *extIp = ip;
        return output;
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
        *halted = true;
        return -1;
      }
      default: {
        printf("Got unkown opcode: %d at ip: %d\n", memory[ip], ip);
        *halted = true;
        return -1;
      }
    }
  }
}

int main(int argc, char const *argv[])
{
  char* source = readFile("./input.txt");

  int numCommands = countCommands(source);
  int* memory = malloc(numCommands * sizeof(int));
  int* memoryA = malloc(numCommands * sizeof(int));
  int* memoryB = malloc(numCommands * sizeof(int));
  int* memoryC = malloc(numCommands * sizeof(int));
  int* memoryD = malloc(numCommands * sizeof(int));
  int* memoryE = malloc(numCommands * sizeof(int));
  int64_t maxSignal = 1;
  int maxPerm = 0;

  loadMemory(memory, source);

  for(int i=0; i<120; i++) {
      int64_t signal = 0;

      memcpy(memoryA, memory, numCommands * sizeof(int));
      memcpy(memoryB, memory, numCommands * sizeof(int));
      memcpy(memoryC, memory, numCommands * sizeof(int));
      memcpy(memoryD, memory, numCommands * sizeof(int));
      memcpy(memoryE, memory, numCommands * sizeof(int));

      int ipA = 0;
      int ipB = 0;
      int ipC = 0;
      int ipD = 0;
      int ipE = 0;

      bool haltedA = false;
      bool haltedB = false;
      bool haltedC = false;
      bool haltedD = false;
      bool haltedE = false;

      bool gfA = false;
      bool gfB = false;
      bool gfC = false;
      bool gfD = false;
      bool gfE = false;
      int64_t out;

      while(!haltedE) {
        out = runMachine(memoryA, perms[i][0] + 5, signal, &haltedA, &ipA, &gfA);
        signal = (out != -1) ? out : signal;
        out = runMachine(memoryB, perms[i][1] + 5, signal, &haltedB, &ipB, &gfB);
        signal = (out != -1) ? out : signal;
        out = runMachine(memoryC, perms[i][2] + 5, signal, &haltedC, &ipC, &gfC);
        signal = (out != -1) ? out : signal;
        out = runMachine(memoryD, perms[i][3] + 5, signal, &haltedD, &ipD, &gfD);
        signal = (out != -1) ? out : signal;
        out = runMachine(memoryE, perms[i][4] + 5, signal, &haltedE, &ipE, &gfE);
        signal = (out != -1) ? out : signal;
      }

      if(signal > maxSignal) {
          maxSignal = signal;
          maxPerm = i;
      }
  }
  printf("Solution 1: %lld - %d,%d,%d,%d,%d\n", maxSignal, perms[maxPerm][0],perms[maxPerm][1],perms[maxPerm][2],perms[maxPerm][3],perms[maxPerm][4] );

  free(source);
  free(memoryA);
  free(memoryB);
  free(memoryC);
  free(memoryD);
  free(memoryE);
  return 0;
}