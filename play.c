#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "logic.h"
#include "pos.h"
#include "board.h"

char scan_command(game* g) {
    char col;
    if (g->player == BLACKS_TURN) {
        printf("\nBlack: ");
        scanf(" %c", &col);
    }
    else {
        printf("\nWhite: ");
        scanf(" %c", &col);
    }

    return col;
}

void execute_command(game* g, char col) {

    if (col == '!'){
        bool offseted = offset(g);
        if (!offseted) {
            printf("\nOffset is not possible, please do something else.");
            char newcol = scan_command(g);
            execute_command(g, newcol);
        }
    }

    else if (col == '^'){
        disarray(g);
    }

    else {
        unsigned int colnum = 0;
        if ((unsigned int)col >= 48 && (unsigned int)col <= 57) {
            colnum = col - 48;
        }
        else if ((unsigned int)col >= 65 && (unsigned int)col <= 90) {
            colnum = col - 55;
        }
        else if ((unsigned int)col >= 97 && (unsigned int)col <= 122) {
            colnum = col - 61;
        }
        else {
            printf("\nCommand not possible because instruction character is %d. Please do something else.", (int)col);
            char newcol = scan_command(g);
            execute_command(g, newcol);
        }
        bool dropped = drop_piece(g, colnum);
        if (!dropped) {
            printf("\nCannot drop a piece in this column, please do something else.");
            char newcol = scan_command(g);
            execute_command(g, newcol);
        }
    }
}

int main(int argc, char *argv[]) {

    //game init 
    if (argc != 8) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }

    unsigned int run = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    enum type type;

    for (int i = 1; i < 8; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            if (i + 1 >= argc || atoi(argv[i + 1]) <= 0) {
                fprintf(stderr, "Invalid input order\n");
                return 1;
            }
            else { 
                height = atoi(argv[i + 1]);
                i++;
            }
        }

        else if (strcmp(argv[i], "-w") == 0) {
            if (i + 1 >= argc || atoi(argv[i + 1]) <= 0) {
                fprintf(stderr, "Invalid input order\n");
                return 1;
            }
            else { 
                width = atoi(argv[i + 1]);
                i++;
            }
        }

        else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 >= argc || atoi(argv[i + 1]) <= 0) {
                fprintf(stderr, "Invalid input order\n");
                return 1;
            }
            else {
                run = atoi(argv[i + 1]);
                i++;
            } 
        }

        else if (strcmp(argv[i], "-b") == 0) {
            type = BITS;
        }

        else if (strcmp(argv[i], "-m") == 0) {
            type = MATRIX;
        }

        else {
            fprintf(stderr, "Invalid input: %s\n", argv[i]);
            return 1;
        }

    }
    
    game* g = new_game(run, width, height, type);
    int done = 0;

    //game loop
    while (!done) {
        //1
        board_show(g->b);

        //2 & 3
        char col = scan_command(g);

        //4 & 5 & 6 & 7 & 8
        execute_command(g, col);

        //9 & 10
        if (game_outcome(g) != IN_PROGRESS) {
            printf("\n");
            board_show(g->b);

            switch(game_outcome(g)) {
                case BLACK_WIN:
                    printf("\nBlack wins the game!\n");
                    break;
                case WHITE_WIN:
                    printf("\nWhite wins the game!\n");
                    break;
                case DRAW:
                    printf("\nThe game ends in a tie\n");
                    break;
                default:
                    fprintf(stderr, "\ngame outcome non-fitting\n");
                    return 1;
            }

            done = 1;
        }
    } 
}