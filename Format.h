#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

typedef struct MovieNode {
    int year;
    char* title;
    double budget;
    struct MovieNode* next;
} MovieNode;

#endif