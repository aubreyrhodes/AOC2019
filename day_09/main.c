#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "file.h"

typedef enum VM_STATUS {
  VM_READY,
  VM_GIVING_OUTPUT,
  VM_NEEDS_INPUT,
  VM_HALTED
} VM_STATUS;

typedef struct VM {
  int ip;
  int64_t output;
  VM_STATUS status;
  int relOffset;
  int64_t* memory;
} VM;

static void initVM(VM* vm, int64_t* memory, int numCommands) {
  vm->ip = 0;
  vm->output = -1;
  vm->status = VM_READY;
  vm->relOffset = 0;
  vm->memory = malloc(sizeof(int64_t) * numCommands);
  memcpy(vm->memory, memory, numCommands * sizeof(int64_t));
}

static void freeVM(VM* vm) {
  free(vm->memory);
  free(vm);
}

static int countCommands(const char* source) {
  int sum = 0;
  int i = 0;

  for(;;) {
    if(source[i] == ',') sum++;
    if(source[i] == '\0') return sum + 1; // account for last command without a trailing ,
    i++;
  }
}

static void loadMemory(int64_t* memory, const char* source) {
  char* token = strtok((char *)source, ",");
  int i = 0;
  while(token != NULL) {
    memory[i] = strtol(token, NULL, 10);
    i++;
    token = strtok(NULL, ",");
  }
}

static int getPos(int mode, int ip, int offset, int64_t* memory, int relOffset) {
  switch(mode) {
    case 0:
      return memory[ip + offset];
    case 1:
      return ip + offset;
    case 2:
      return (relOffset + memory[ip + offset]);
    default:
      printf("Got unknown param mode: %d\n", mode);
      return -1;
  }  
}

static void runMachine(VM* vm, int currentInput) {
  int64_t* memory = vm->memory;

  for(;;) {
    int instr = memory[vm->ip];
    int op = instr % 100;
    int modeC = (instr / 100) % 10;
    int modeB = (instr / 1000) % 10;
    int modeA = (instr / 10000) % 10;
    switch(op) {
      case 1: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        int pos3 = getPos(modeA, vm->ip, 3, memory, vm->relOffset);
        memory[pos3] = memory[pos1] + memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        vm->ip = vm->ip + 4;
        break;
      }
      case 2: {        
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        int pos3 = getPos(modeA, vm->ip, 3, memory, vm->relOffset);
        memory[pos3] = memory[pos1] * memory[pos2];
        // printf("Updated pos: %d with value: %d\n", pos3, memory[pos3]);
        vm->ip = vm->ip + 4;
        break;
      }
      case 3: {
        if (modeC == 0) {
          int pos1 = memory[vm->ip+1];
          memory[pos1] = currentInput;
        } else if (modeC == 2) {
          int pos1 = vm->relOffset + memory[vm->ip + 1];
          memory[pos1] = currentInput;
        } else {
          printf("Don't how to get input\n");
          vm->status = VM_HALTED;
          return;
        }
        vm->ip = vm->ip + 2;
        vm->status = VM_NEEDS_INPUT;
        return;
      }
      case 4: {
        if (modeC == 1) {
            vm->output = memory[vm->ip+1];
        } else if (modeC == 0){
            vm->output = memory[memory[vm->ip+1]];
        } else {
          vm->output = memory[vm->relOffset + memory[vm->ip + 1]];
        }
        vm->ip += 2;
        vm->status = VM_GIVING_OUTPUT;
        return;
      }
      case 5: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        if(memory[pos1] != 0) {
          // printf("Jumping to: %d\n", pos2);
          vm->ip = memory[pos2];
        } else {
          vm->ip = vm->ip + 3;
        }
        break;
      }
      case 6: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        if(memory[pos1] == 0) {
          // printf("Jumping to: %d\n", pos2);
          vm->ip = memory[pos2];
        } else {
          vm->ip = vm->ip + 3;
        }
        break;
      }
      case 7: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        int pos3 = getPos(modeA, vm->ip, 3, memory, vm->relOffset);
        if(memory[pos1] < memory[pos2]) {
          // printf("Setting pos: %d with value 1\n", pos3);
          memory[pos3] = 1;
        } else {
          // printf("Setting pos: %d with value 0\n", pos3);
          memory[pos3] = 0;
        }
        vm->ip = vm->ip + 4;
        break;
      }
      case 8: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        int pos2 = getPos(modeB, vm->ip, 2, memory, vm->relOffset);
        int pos3 = getPos(modeA, vm->ip, 3, memory, vm->relOffset);
        if(memory[pos1] == memory[pos2]) {
          // printf("Setting pos: %d with value 1\n", pos3);
          memory[pos3] = 1;
        } else {
          // printf("Setting pos: %d with value 0\n", pos3);
          memory[pos3] = 0;
        }
        vm->ip = vm->ip + 4;
        break;
      }
      case 9: {
        int pos1 = getPos(modeC, vm->ip, 1, memory, vm->relOffset);
        vm->relOffset += memory[pos1];
        vm->ip = vm->ip + 2;
        break;
      }
      case 99: {
        vm->status = VM_HALTED;
        return;
      }
      default: {
        printf("Got unkown opcode: %lld at ip: %d\n", memory[vm->ip], vm->ip);
        vm->status = VM_HALTED;
        return;
      }
    }
  }
}

int main(int argc, char const *argv[])
{
  const char* source = readFile("./input.txt");
  int numCommands = countCommands(source);

  int64_t* program = malloc(numCommands * sizeof(int64_t));
  loadMemory(program, source);

  VM* vm = malloc(sizeof(VM));
  initVM(vm, program, numCommands);
  int currentInput = 2;

  while(vm->status != VM_HALTED) {
    runMachine(vm, currentInput);
    switch (vm->status) {
      case VM_GIVING_OUTPUT: {
        printf("%lld ", vm->output);
        break;
      }
      case VM_NEEDS_INPUT: {
        currentInput = 0;
        break;
      }
      default: break;
    }
  }

  printf("\n");
  freeVM(vm);
  free(program);
  free((void*) source);
  return 0;
}
