#ifndef BOARD_H
#define BOARD_H

#include "pos.h"


enum cell {
    EMPTY,
    BLACK,
    WHITE
};

typedef enum cell cell;


union board_rep {
    enum cell** matrix;
    unsigned int* bits;
};

typedef union board_rep board_rep;

enum type {
    MATRIX, BITS
};


struct board {
    unsigned int width, height;
    enum type type;
    board_rep u;
};

typedef struct board board;

/*Given a width, height, and type, this function initiates an empty board with those characteristics*/
board* board_new(unsigned int width, unsigned int height, enum type type);

/*Given a board, this function entirely frees its memory*/
void board_free(board* b);

/*Given a board, his function prints it out*/
void board_show(board* b);

/*Given a board and a position, this retrieves the type of the piece at that position*/
cell board_get(board* b, pos p);

/*Given a board, a position, and a cell type, this assigns that type to the given position*/
void board_set(board* b, pos p, cell c);

#endif /* BOARD_H */