#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "file.h"

#define WIDTH 25
#define HEIGHT 6

static int* loadLayers(const char* source, int num) {
    int* layers = malloc(num * WIDTH * HEIGHT * sizeof(int));
    int currentPos = 0;
    for(;;) {
        if(source[currentPos] == '\0') return layers;
        layers[currentPos] = ((int)source[currentPos]) - 48;
        currentPos++;
    }
}

static int countLayers(const char* source) {
    return strlen(source) / WIDTH / HEIGHT;
}

static int findOffsetWithLeastZeros(int* layers, int length) {
    int minOffset = 0;
    int minCount = WIDTH * HEIGHT;
    for(int i=0; i < length; i ++) {
        int numZeros = 0;
        for(int j=0; j < WIDTH * HEIGHT; j++) {
            if(layers[(i*WIDTH*HEIGHT) + j] == 0) numZeros++;
        }
        if(numZeros < minCount) {
            minCount = numZeros;
            minOffset = i;
        }
    }

    return minOffset;
}

static int findSolutionOne(int* layers, int layer){
    int numOnes = 0;
    int numTwos = 0;
    for(int i=0; i<WIDTH * HEIGHT; i++) {
        if(layers[(layer*WIDTH*HEIGHT) + i] == 1) numOnes++;
        if(layers[(layer*WIDTH*HEIGHT) + i] == 2) numTwos++;
    }
    return numOnes * numTwos;
}

static void printImage(int* layers, int length){
    char* img = malloc(WIDTH*HEIGHT*sizeof(char));
    for(int i=0; i<WIDTH*HEIGHT; i++){
        char pixel = '+';
        for(int j=0; j<length; j++) {
            if(layers[(j*WIDTH*HEIGHT)+i] == 0) {
                pixel = ' ';
                break;
            }
            if(layers[(j*WIDTH*HEIGHT)+i] == 1) {
                pixel = '#';
                break;
            }
        }
        img[i] = pixel;
    }

    for(int i=0; i < WIDTH * HEIGHT; i++) {
        if(i % WIDTH == 0) printf("\n");
        printf("%c", img[i]);
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    const char* source = readFile("./input.txt");
    int numLayers = countLayers(source);
    int* layers = loadLayers(source, numLayers);
    int layer = findOffsetWithLeastZeros(layers, numLayers);
    printf("Layer: %d\n", layer + 1);
    int solutionOne = findSolutionOne(layers, layer);
    printf("Solution 1: %d\n", solutionOne);
    printImage(layers, numLayers);
    return 0;
}
