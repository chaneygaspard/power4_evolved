#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>
#include "board.h"


enum turn {
    BLACKS_TURN,
    WHITES_TURN
};

typedef enum turn turn;


enum outcome {
    IN_PROGRESS,
    BLACK_WIN,
    WHITE_WIN,
    DRAW
};

typedef enum outcome outcome;


struct game {
    unsigned int run;
    board* b;
    posqueue *black_queue, *white_queue;
    turn player;
};

typedef struct game game;


/*Given a run requirement, a width and height, and type, this function creates*/
/*a new game with those requirements, containing two empty queues, an empty board*/
game* new_game(unsigned int run, unsigned int width,
               unsigned int height, enum type type);

/*This function entirely frees a game from memory*/
void game_free(game* g);

/*Given a column, drops the right player's piece into that column (as low as it can go)*/
/*returns true if it can be placed, false otherwise.*/
bool drop_piece(game* g, unsigned int column);

/*Performs the dissaray move. At this stage, it is programmed in two steps: */
/*first the horizontal flip, then the gravity fall*/
void disarray(game* g);

/*Removes that last piece that each player has placed, and lets any pieces above them fall due to gravity*/
bool offset(game* g);

/*Checks the outcome of the game; white has one, black has one, draw, or in prgress.*/
outcome game_outcome(game* g);

#endif /* LOGIC_H */