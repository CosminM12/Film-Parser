#ifndef MOVIES_H
#define MOVIES_H

#include "Format.h"

#define MAX_LINE_LENGTH 1024
#define MAX_TITLE_LENGTH 256

void trim(char* str);

int findIndex(char* header_line, const char* column_name);

void insertInList(MovieNode** head, MovieNode* new_node);


MovieNode* read_csv(const char* csvPath);

MovieNode* createNode(int year, const char* title, double budget);

void printMovie(const MovieNode *current);

void printMovies(const MovieNode* head);

void findByYear(const MovieNode* head, int year);

void findByTitle(const MovieNode* head, const char* search_term);

//void averageBudgetInRange(const MovieNode* head, int start_year, int end_year);

void cleanUp(MovieNode* head);

#endif