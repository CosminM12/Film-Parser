#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "Movies.h"


void trim(char* str) {

    if (str == NULL) return;

    //Spaces in front
    char* start = str;
    while(isspace(*start)) {
        start++;
    }
    if(start != str) {
        memmove(str, start, strlen(start)+1);
    }

    //Spaces at the end
    char* end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';
    // if(end != str) {
    //     memmove(str, end, strlen(end));
    // }
}

int findIndex(char* header_line, const char* column_name) {

    char* token;
    char* rest = header_line;
    int index = 0;

    while ((token = strtok_r(rest, ",", &rest)) != NULL) {
        trim(token);
        if (strcasecmp(token, column_name) == 0) {
            return index;
        }
        index++;
    }

    return -1;
}

void insertInList(MovieNode** head, MovieNode* new_node) {

    MovieNode* current;

    //node insert in head
    if (*head == NULL || (*head)->year > new_node->year ||
        ((*head)->year == new_node->year && strcmp((*head)->title, new_node->title) > 0)) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    //search position for insertion
    current = *head;
    while (current->next != NULL &&
           (current->next->year < new_node->year ||
            (current->next->year == new_node->year && strcmp(current->next->title, new_node->title) < 0))) {
        current = current->next;
    }

    //insert node
    new_node->next = current->next;
    current->next = new_node;
}


MovieNode* read_csv(const char* csvPath) {

    FILE* file = fopen(csvPath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", csvPath);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    char header_copy[MAX_LINE_LENGTH];
    MovieNode* head = NULL;

    //Read header
    if (fgets(line, sizeof(line), file) == NULL) {
        fprintf(stderr, "Empty file or error reading header\n");
        fclose(file);
        exit(1);
    }

    //Replace \n with \0
    size_t len = strlen(line);
    if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
        line[len-1] = '\0';
    }

    // --- making a copy of the header for tokenization ---
    strcpy(header_copy, line);

    // --- finding the indices of the columns we need ---
    int year_index = findIndex(header_copy, "year");
    strcpy(header_copy, line); // Reset for the next search
    int title_index = findIndex(header_copy, "title");
    strcpy(header_copy, line);
    int budget_index = findIndex(header_copy, "budget");

    if (year_index == -1 || title_index == -1 || budget_index == -1) {
        fprintf(stderr, "Required columns not found in CSV header\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // --- reading the data lines ---
    while (fgets(line, sizeof(line), file) != NULL) {
        // removing newline character if present
        len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
        }

        char* token;
        char* rest = line;
        int column_index = 0;
        int year = 0;
        char title[MAX_TITLE_LENGTH] = "";
        double budget = 0.0;

        // parsing the line
        while ((token = strtok_r(rest, ",", &rest)) != NULL) {
            trim(token);

            if (column_index == year_index) {
                year = atoi(token);

            } else if (column_index == title_index) {
                // checking if this is a quoted field (which may contain commas)
                if (token[0] == '"' && token[strlen(token)-1] != '"') {
                    char quoted_field[MAX_TITLE_LENGTH] = "";
                    strncpy(quoted_field, token + 1, MAX_TITLE_LENGTH - 1);
                    strcat(quoted_field, ",");

                    // keeping to read tokens until we find the closing quote
                    while ((token = strtok_r(rest, ",", &rest)) != NULL) {
                        column_index++;

                        size_t token_len = strlen(token);
                        if (token[token_len-1] == '"') {
                            token[token_len-1] = '\0';
                            strncat(quoted_field, token, MAX_TITLE_LENGTH - strlen(quoted_field) - 1);
                            strncpy(title, quoted_field, MAX_TITLE_LENGTH - 1);
                            break;
                        } else {
                            strncat(quoted_field, token, MAX_TITLE_LENGTH - strlen(quoted_field) - 1);
                            strcat(quoted_field, ",");
                        }
                    }
                } else {
                    if (token[0] == '"' && token[strlen(token)-1] == '"') {
                        // removing quotes if present
                        token[strlen(token)-1] = '\0';
                        strncpy(title, token + 1, MAX_TITLE_LENGTH - 1);
                    } else {
                        strncpy(title, token, MAX_TITLE_LENGTH - 1);
                    }
                }
                title[MAX_TITLE_LENGTH - 1] = '\0';
            } else if (column_index == budget_index) {
                budget = atof(token);
            }

            column_index++;
        }

        // --- creating and insert a new node if we have valid data ---
        if (year > 0 && strlen(title) > 0) {
            MovieNode* new_node = createNode(year, title, budget);
            insertInList(&head, new_node);
        }
    }

    fclose(file);
    return head;
}

MovieNode* createNode(int year, const char* title, double budget) {

    MovieNode* new_node = (MovieNode*)malloc(sizeof(MovieNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    new_node->year = year;
    new_node->title = strdup(title);
    if (new_node->title == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        free(new_node);
        exit(1);
    }
    new_node->budget = budget;
    new_node->next = NULL;

    return new_node;
}

void printMovie(const MovieNode *current) {
    char budget_str[32];
    if (current->budget >= 1000000) {
        sprintf(budget_str, "$%.2f million", current->budget / 1000000.0);
    } else if (current->budget > 0) {
        sprintf(budget_str, "$%.2f", current->budget);
    } else {
        strcpy(budget_str, "N/A");
    }

    printf("%-7d %-50.50s %s\n", current->year, current->title, budget_str);
}

void printMovies(const MovieNode* head) {
    const MovieNode* current = head;
    
    printf("===================================================================\n");
    printf("Year    Title                                                Budget\n");
    
    while (current != NULL) {
        printMovie(current);

        current = current->next;
    }
}

void findByYear(const MovieNode* head, int year) {

    const MovieNode* current = head;
    bool found = 0;

    printf("\nMovies released in %d:\n", year);
    printf("----------------------------------------------------------------------\n");

    while (current != NULL) {
        if (current->year == year) {
            printMovie(current);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No movies found for year %d\n", year);
    }
}

void findByTitle(const MovieNode* head, const char* search_term) {

    const MovieNode* current = head;
    int found = 0;

    printf("\nMovies with title containing \"%s\":\n", search_term);
    printf("----------------------------------------------------------------------\n");

    while (current != NULL) {
        // --- case-insensitive search ---
        if (strcasestr(current->title, search_term) != NULL) {
            printMovie(current);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No movies found with \"%s\" in the title\n", search_term);
    }
}

/*void averageBudgetInRange(const MovieNode* head, int start_year, int end_year) {

    const MovieNode* current = head;
    double total_budget = 0.0;
    int count = 0;

    while (current != NULL) {
        if (current->year >= start_year && current->year <= end_year && current->budget > 0) {
            total_budget += current->budget;
            count++;
        }
        current = current->next;
    }

    printf("\nAverage budget for movies between %d and %d:\n", start_year, end_year);
    if (count > 0) {
        double average = total_budget / count;
        if (average >= 1000000) {
            printf("$%.2f million (based on %d movies)\n", average / 1000000.0, count);
        } else {
            printf("$%.2f (based on %d movies)\n", average, count);
        }
    } else {
        printf("No movies with budget information found for this period\n");
    }
}*/

void cleanUp(MovieNode* head) {

    MovieNode* current = head;
    MovieNode* next;

    while (current != NULL) {
        next = current->next;
        free(current->title);
        free(current);
        current = next;
    }
}