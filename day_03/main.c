#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

typedef struct Point_t {
  int x;
  int y;
  struct Point_t* next;
} Point;

typedef struct Wires_t {
  Point* wire1;
  Point* wire2;
} Wires;

Point* processInstruction(char direction, int length, Point* startPoint) {
  printf("D: %c L: %d\n", direction, length);

  int x = startPoint->x;
  int y = startPoint->y;
  Point* currentPoint = startPoint;

  for(int i = length; i > 0; i--) {
    switch(direction) {
      case 'U': y++; break;
      case 'D': y--; break;
      case 'R': x++; break;
      case 'L': x--; break;
    }

    currentPoint->next = malloc(sizeof(Point));
    currentPoint = currentPoint->next;
    currentPoint->x = x;
    currentPoint->y = y;
    printf("(%d, %d)", x, y);
  }
  printf("\n");
  return currentPoint;
}

Wires* getWires(char* source) {
  int pointer = 0;
  Wires* wires = malloc(sizeof(Wires));
  Point* currentPoint = malloc(sizeof(Point));
  currentPoint->x = 0;
  currentPoint->y = 0;
  wires->wire1 = currentPoint;

  for(;;) {
    switch(source[pointer]) {
      case '\0':
        return wires;
      case ',':
        pointer++;
        break;
      case '\n':
        printf("Switching Wires\n");
        currentPoint = malloc(sizeof(Point));
        currentPoint->x = 0;
        currentPoint->y = 0;
        wires->wire2 = currentPoint;
        pointer++;
      case 'U':
      case 'D':
      case 'L':
      case 'R': {
        int end = pointer + 1;
        for(;;) {
          if(source[end] == ',' || source[end] == '\n' || source[end] == '\0') break;
          end++;
        }
        int length = end - pointer;
        char* buf = malloc(sizeof(char) * (length));
        memcpy(buf, &source[pointer + 1], (length - 1));
        buf[length] = '\0';
        char direction = source[pointer];
        int lineLength = strtol(buf, NULL, 10);
        free(buf);
        currentPoint = processInstruction(direction, lineLength, currentPoint);
        pointer = pointer + length;
        break;
      }
    }
  }
}

int findDistance(int x, int y) {
  if(x < 0) {
    x *= -1;
  }

  if(y < 0) {
    y *= -1;
  }

  return x + y;
}

int findClosestIntersection(Point* a, Point* b) {
  int min = UINT16_MAX;
  Point* currentA = a->next;
  while(currentA != NULL) {
    Point* currentB = b->next;
    while(currentB != NULL) {
      if(currentA->x == currentB->x && currentA->y == currentB->y) {
        int distance = findDistance(currentA->x, currentA->y);
        if(distance < min) min = distance;
        printf("Found Intersection: (%d, %d)\n", currentA->x, currentA->y);
        currentB = NULL;
      } else {
        currentB = currentB->next;
      }
    }
    currentA = currentA->next;
  }

  return min;
}

int findNearestIntersection(Point* a, Point* b) {
  int min = UINT16_MAX;
  int stepsA = 1;
  Point* currentA = a->next;
  while(currentA != NULL) {
    Point* currentB = b->next;
    int stepsB = 1;
    while(currentB != NULL) {
      if(currentA->x == currentB->x && currentA->y == currentB->y) {
        int distance = stepsA + stepsB;
        if(distance < min) min = distance;
        printf("Found Intersection: (%d, %d)\n", currentA->x, currentA->y);
        currentB = NULL;
      } else {
        currentB = currentB->next;
        stepsB++;
      }
    }
    currentA = currentA->next;
    stepsA++;
  }

  return min;
}

 
int main(int argc, char const *argv[])
{
  char* source = readFile("./input.txt");
  Wires* wires = getWires(source);
  int solution = findClosestIntersection(wires->wire1, wires->wire2);
  printf("Solution 1: %d\n", solution);
  int sol2 = findNearestIntersection(wires->wire1, wires->wire2);
  printf("Solution 2: %d\n", sol2);
  return 0;
}
