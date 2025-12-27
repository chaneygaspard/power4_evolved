#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "board.h"


board* board_new(unsigned int width, unsigned int height, enum type type) {
    board* new_board = (board*)malloc(sizeof(new_board));
    if (new_board == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for board\n");
        exit(1);        
    }

    new_board->width = width;
    new_board->height = height;

    switch(type) {
        case MATRIX:
            new_board->u.matrix = (cell**)malloc(height * sizeof(cell*));
            if (new_board->u.matrix == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for board matrix\n");
                free(new_board);
                exit(1);        
            }

            for (unsigned int i = 0; i < height; i++) {
                new_board->u.matrix[i] = (cell*)malloc(width * sizeof(cell));

                if (new_board->u.matrix[i] == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for line in board matrix\n");
                    free(new_board);
                    exit(1);        
                }

                for (unsigned int j = 0; j < width; j++) {
                    new_board->u.matrix[i][j] = EMPTY;
                }
            }
            break;

        case BITS:
            if (width >= 16) {
                fprintf(stderr, "Width too big for bit packing\n");
                exit(1);
            }

            new_board->u.bits = (unsigned int*)malloc(height * sizeof(unsigned int));
            if (new_board->u.bits == NULL) {
                fprintf(stderr, "Error: Memory allocation failed for board bits\n");
                free(new_board);
                exit(1);        
            }

            for (unsigned int i = 0; i < height; i++) {
                new_board->u.bits[i] = 0; //iniate each row to all 0 32-bit
            }
            break;

        default:
            fprintf(stderr, "Error: Incompatible board type\n");
            exit(1);
    }

    new_board->type = type;
    return new_board;
}

void board_free(board* b) {
    if (b == NULL) {
        return;
    }

    switch(b->type) {
        case MATRIX:
            for (unsigned int i = 0; i < b->height; i++) {
                free(b->u.matrix[i]);
            }
            free(b->u.matrix);
            break;

        case BITS:
            free(b->u.bits);
            break;

        default:
            fprintf(stderr, "Error: Incompatible board type\n");
            exit(1); 
    }

    free(b);
}

void board_show(board* b) {
    if (b == NULL) {
        fprintf(stderr, "Error: Board is empty\n");
        exit(1);
    }

    printf("  ");
    for (unsigned int k = 0; k < b->width; k++) {
        if (k < 10) {
            printf("%d ", k);
        }
        else {
            printf("%c", k - 10 + 'A');
        }
    }
    printf("\n\n");


    for (unsigned int i = 0; i < b->height; i++) {

        if (i < 10) {
            printf("%d ", i);
        }
        else {
            printf("%c", i - 10 + 'A');
        }

        for (unsigned int j = 0; j < b->width; j++) {
            switch(board_get(b, make_pos(i, j))) {
                case WHITE:
                    printf("o ");
                    break;
                case BLACK:
                    printf("* ");
                    break;
                default:
                    printf(". ");
                    break;
            }
        }
        printf("\n\n");
    }
}

cell board_get(board* b, pos p) {
    if (b == NULL) {
        fprintf(stderr, "Error: Board is empty\n");
        exit(1);
    }

    if (p.r >= b->height || p.c >= b->width) {
        fprintf(stderr, "Position out of board range\n");
        exit(1);
    }

    switch (b->type) {
        case MATRIX:
            return b->u.matrix[p.r][p.c];
        
        case BITS: {
            unsigned int row = b->u.bits[p.r];
            unsigned int shift = 2 * p.c;
            unsigned int colval = (row >> shift) & 3;

            switch (colval) {
                case 0:
                    return EMPTY;
                case 1:
                    return BLACK;
                case 2:
                    return WHITE;
                default:
                    fprintf(stderr, "Invalid bit");
                    exit(1);
            }       
        }

        default:
            fprintf(stderr, "Invalid board type");
            exit(1);
    }
}

void board_set(board* b, pos p, cell c) {
    if (b == NULL || b->u.matrix == NULL) {
        fprintf(stderr, "Error: Board is empty\n");
        exit(1);
    }
    
    if (p.r >= b->height || p.c >= b->width) {
        fprintf(stderr, "Position out of board range\n");
        exit(1);
    }

    switch(b->type) {
        case MATRIX:
            b->u.matrix[p.r][p.c] = c;
            break;

        case BITS: {
            unsigned int shift = 2 * p.c; 
            b->u.bits[p.r] &= ~(3 << shift); 

            switch(c) {
                case BLACK:
                    b->u.bits[p.r] += (1 << shift);
                    break;
                case WHITE:
                    b->u.bits[p.r] += (2 << shift);
                    break;
                case EMPTY:
                    break;
                default:
                    fprintf(stderr, "Invalid case type");
                    exit(1);    
            }
            break;
        }
        
        default:
            fprintf(stderr, "Invalid board type");
            exit(1);
    }
}