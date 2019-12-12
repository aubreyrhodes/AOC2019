#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "file.h"

typedef struct Body {
    char* name;
    struct Body* firstSat;
    struct Body* nextSibling;
    struct Body* parent;
} Body;

typedef struct Node {
    char* base;
    char* sat;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct Queue {
    Node* first;
    Node* last;
} Queue;


static Body* getBody(Body* root, char* name) {
    if(strcmp(name, "COM") == 0) {
        root->name = name;
        return root;
    }

    if(root->name == NULL) return NULL;

    if(strcmp(name, root->name) == 0) {
        return root;
    }

    Body* child = root->firstSat;
    while(child != NULL) {
        Body* found = getBody(child, name);
        if(found != NULL) return found;
        child = child->nextSibling;
    }

    return NULL;
}

static void addToQueue(Queue* queue, char* base, char* sat) {
    Node* node = malloc(sizeof(Node));
    node->base = base;
    node->sat = sat;
    node->next = NULL;
    node->prev = NULL;
    if(queue->last != NULL){
        queue->last->next = node;
        node->prev = queue->last;
    }
    if(queue->first == NULL) queue->first = node;
    queue->last = node;
}

static Node* getFromQueue(Queue* queue) {
    Node* node = queue->first;
    if(node == NULL) return NULL;
    queue->first = node->next;
    return node;
}
static void loadQueue(Queue* queue, char* source) {
    int start = 0;
    int end = 0;
    char* base;
    char* sat;

    for(;;) {
        switch(source[end]) {
            case ')': {
                int length = end - start;
                base = malloc(sizeof(char) * length + 1);
                memcpy(base, &source[start], length);
                base[length] = '\0';
                end++;
                start = end;
                break;
            }
            case '\n': {
                int length = end - start;
                sat = malloc(sizeof(char) * length + 1);
                memcpy(sat, &source[start], length);
                sat[length] = '\0';
                addToQueue(queue, base, sat);
                end++;
                start = end;
                break;
            }
            case '\0': return;
            default: {
                end++;
                break;
            }
        }
    }
}

static void addSat(Body* base, char* name) {
    Body* satBody = malloc(sizeof(Body));
    satBody->name = name;
    satBody->firstSat = NULL;
    satBody->nextSibling = NULL;
    satBody->parent = base;

    if(base->firstSat == NULL) {
        base->firstSat = satBody;
    } else {
        Body* currentSat = base->firstSat;
        while(currentSat->nextSibling != NULL) {
            currentSat = currentSat->nextSibling;
        }
        currentSat->nextSibling = satBody;
    }
}

static void buildGraph(Body* root, Queue* bodies) {
    Node* currentBody = bodies->first;

    while(currentBody != NULL) {
        Body* base = getBody(root, currentBody->base);
        if(base == NULL) {
            bodies->first = currentBody->next;
            bodies->last->next = currentBody;
            bodies->last = currentBody;
            currentBody->next = NULL;
            currentBody = bodies->first;
        } else {
            addSat(base, currentBody->sat);
            bodies->first = currentBody->next;
            currentBody = bodies->first;
        }
    }
}

static int countGraph(Body* root, int base) {
    if(root->firstSat == NULL) return base;
    int sum = base;
    Body* currentSat = root->firstSat;
    while(currentSat != NULL) {
        sum += countGraph(currentSat, base + 1);
        currentSat = currentSat->nextSibling;
    }
    return sum;
}

int findTransferDistance(Body* root, char* from, char* to) {
    Body* fromBody = getBody(root, from);
    Body* toBody = getBody(root, to);
    Queue* fromQueue = malloc(sizeof(Queue));
    Queue* toQueue = malloc(sizeof(Queue));

    while(fromBody->parent != NULL) {
        addToQueue(fromQueue, fromBody->parent->name, NULL);
        fromBody = fromBody->parent;
    }

    while(toBody->parent != NULL) {
        addToQueue(toQueue, toBody->parent->name, NULL);
        toBody = toBody->parent;
    }

    Node* toNode = toQueue->last;
    Node* fromNode = fromQueue->last;
    while(strcmp(toNode->base, fromNode->base) == 0) {
        toNode = toNode->prev;
        fromNode = fromNode->prev;
    }

    int distance = 0;
    while(toNode != NULL) {
        distance++;
        toNode = toNode->prev;;
    }

    while(fromNode != NULL) {
        distance++;
        fromNode = fromNode->prev;
    }

    return distance;
}

int main(int argc, char const *argv[]) {
    char* source = readFile("input.txt");
    Queue* queue = malloc(sizeof(Queue));
    queue->first = NULL;
    queue->last = NULL;
    loadQueue(queue, source);
    Body* graph = malloc(sizeof(Body));
    graph->name = NULL;
    graph->firstSat = NULL;
    graph->nextSibling = NULL;
    graph->parent = NULL;
    buildGraph(graph, queue);

    int solution = countGraph(graph, 0);
    printf("Solution 1: %d\n", solution);

    int solution2 = findTransferDistance(graph, "YOU", "SAN");
    printf("Solution 2: %d\n", solution2);
    
    return 0;
}
