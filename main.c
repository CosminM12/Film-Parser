#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Format.h"
#include "Movies.h"

#define MAX_LINE_LENGTH 1024
#define MAX_TITLE_LENGTH 256



int main(int argc, char* argv[]) {

    int answer = -1;
    MovieNode* list = read_csv("movies.csv");

    while (answer != 0) {
        printf("\n[MENU]\n");
        printf("1. Display all movies (sorted by year and title)\n");
        printf("2. Search movies by year\n");
        printf("3. Search movies by title\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &answer);

        switch(answer) {
            case 1:
                printf("->All movies sorted by year & title:\n");
                printMovies(list);
                break;
            case 2:
                int year;
                scanf("%d", &year);
                
                if(year > 2013 || year < 1970) {
                    printf("No movies in the selected year! Please try again!\n");
                }
                else {
                    findByYear(list, year);
                }
                break;
            case 3:
                char title[MAX_TITLE_LENGTH];
                scanf("%s", title);
                title[strcspn(title, "\n")] = 0;
                
                findByTitle(list, title);
                break;
            case 0:
                printf("Good bye!\n");
                break;
            default:
                printf("Invalid entry, please try again!\n");
        }
    }


    cleanUp(list);
    return 0;
}