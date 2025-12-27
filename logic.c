#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "logic.h"

game* new_game(unsigned int run, unsigned int width,
               unsigned int height, enum type type) {
    if ((run > width && run > height) || run == 0) {
        fprintf(stderr, "Error: Run requirement unattainable with these board dimensions (run = %d, width = %d, height = %d)\n", run, width, height);
        exit(1);
    }

    game* new_game = (game*)malloc(sizeof(game));
    if (new_game == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for game\n");
        exit(1);
    }

    new_game->run = run;

    new_game->b = board_new(width, height, type);
    if (new_game->b == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for board\n");
        exit(1);
    }

    new_game->black_queue = posqueue_new();
    if (new_game->black_queue == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for black queue\n");
        free(new_game); 
        exit(1);
    }

    new_game->white_queue = posqueue_new();
    if (new_game->white_queue == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for white queue\n");
        posqueue_free(new_game->black_queue); 
        free(new_game); 
        exit(1);
    }

    new_game->player = BLACKS_TURN;

    return new_game;
}

void game_free(game* g) {
    if (g == NULL) {
        return;
    }

    if (g->white_queue != NULL) {
        posqueue_free(g->white_queue);
    }

    if (g->black_queue != NULL) {
        posqueue_free(g->black_queue);
    }

    if (g->b != NULL) {
        board_free(g->b);
    }

    free(g);
}

/*Helper function for the possible player actions, changes whose players turn it is*/
void next_turn(game* g) {
    if (g->player == BLACKS_TURN) {
        g->player = WHITES_TURN;
    } else {
        g->player = BLACKS_TURN;
    }
}

bool drop_piece(game* g, unsigned int column) {
    if (g == NULL || g->b == NULL) {
        fprintf(stderr, "Game is null\n");
        exit(1);
    }

    if (column >= g->b->width) {
        fprintf(stderr, "Drop_piece column out of range\n");
        return false; 
    }

    /*place the piece*/
    int i = g->b->height - 1;

    pos potential_spot;
    while (i >= 0) {
        potential_spot = make_pos(i, column);

        if (board_get(g->b, potential_spot) == EMPTY) {
            if (g->player == BLACKS_TURN) {
                board_set(g->b, potential_spot, BLACK);
                pos_enqueue(g->black_queue, potential_spot);
            } else {
                board_set(g->b, potential_spot, WHITE);
                pos_enqueue(g->white_queue, potential_spot);
            }

            next_turn(g);
            return true;
        }

        i--;
    }

    return false;
}

/*Sets all positions to EMPTY, helper for offset*/
void board_clear(board* b) {
    for (unsigned int i = 0; i < b->height; i++) {
        for (unsigned int j = 0; j < b->width; j++) {
            pos current_pos = make_pos(i, j);
            board_set(b, current_pos, EMPTY);
        }
    }
}

//struct to support a column 
typedef struct {
    game* g;
    int column;
} disarray_args;

//pthread function for each column
void* disarray_column(void* arg) {
    disarray_args* args = (disarray_args*)arg;
    game* g = args->g;
    int col = args->column;

    //Horizontal Flip
    for (int i = 0; i < g->b->height / 2; i++) {
        pos top_pos = make_pos(i, col);
        pos bottom_pos = make_pos(g->b->height - 1 - i, col);

        cell top_cell = board_get(g->b, top_pos);
        cell bottom_cell = board_get(g->b, bottom_pos);

        board_set(g->b, top_pos, bottom_cell);
        board_set(g->b, bottom_pos, top_cell);

        //Update queue
        pq_entry* current;
        if (top_cell != EMPTY) {
            current = (top_cell == BLACK) ? g->black_queue->head : g->white_queue->head;
            while (current != NULL) {
                if (current->p.r == i && current->p.c == col) {
                    current->p = bottom_pos;
                    break;
                }
                current = current->next;
            }
        }
        if (bottom_cell != EMPTY) {
            current = (bottom_cell == BLACK) ? g->black_queue->head : g->white_queue->head;
            while (current != NULL) {
                if (current->p.r == g->b->height - 1 - i && current->p.c == col) {
                    current->p = top_pos;
                    break;
                }
                current = current->next;
            }
        }
    }

    //Gravity
    int target_row = g->b->height - 1; 
    for (int i = g->b->height - 1; i >= 0; i--) {
        pos p = make_pos(i, col);
        cell c = board_get(g->b, p);

        if (c != EMPTY) {
            if (i != target_row) {
                pos target_pos = make_pos(target_row, col);
                board_set(g->b, target_pos, c);
                board_set(g->b, p, EMPTY);

                //update queue
                pq_entry* current = (c == BLACK) ? g->black_queue->head : g->white_queue->head;
                while (current != NULL) {
                    if (current->p.r == i && current->p.c == col) {
                        current->p = target_pos;
                        break;
                    }
                    current = current->next;
                }
            }
            target_row--; 
        }
    }

    return NULL;
}

//dissaray for mat type
void disarray_matrix(game* g) {
    if (g == NULL || g->b == NULL) {
        fprintf(stderr, "Game is NULL\n");
        exit(1);
    }

    if (g->b->type != MATRIX) {
        fprintf(stderr, "Multithreading only works for MATRIX type\n");
        return;
    }

    pthread_t threads[g->b->width];
    disarray_args args[g->b->width];

    for (int j = 0; j < g->b->width; j++) {
        args[j].g = g;
        args[j].column = j;
        pthread_create(&threads[j], NULL, disarray_column, &args[j]);
    }

    for (int j = 0; j < g->b->width; j++) {
        pthread_join(threads[j], NULL);
    }

    next_turn(g);
}

//disarray for BITS type
void disarray_bits(game* g) {
    if (g == NULL || g->b == NULL) {
        fprintf(stderr, "Game is null\n");
        exit(1);
    }

    int* col_count = (int*)calloc(g->b->width, sizeof(int));
    if (col_count == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    /*Horizontal flip*/
    pq_entry* current = g->black_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, EMPTY);
        current->p.r = g->b->height - current->p.r - 1;
        board_set(g->b, current->p, BLACK);
        col_count[current->p.c]++;
        current = current->next;
    }
    current = g->white_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, EMPTY);
        current->p.r = g->b->height - current->p.r - 1;
        board_set(g->b, current->p, WHITE);
        col_count[current->p.c]++;
        current = current->next;
    }

    //printf("%d \n", col_count[1]);
    //board_show(g->b);

    /*Gravity*/
    current = g->black_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, EMPTY);
        current->p.r += (g->b->height - col_count[current->p.c]);
        board_set(g->b, current->p, BLACK);
        current = current->next;
    }
    current = g->white_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, EMPTY);
        current->p.r += (g->b->height - col_count[current->p.c]);
        board_set(g->b, current->p, WHITE);
        current = current->next;
    }
    
    free(col_count);
    next_turn(g);
}

void disarray(game* g) {
    switch(g->b->type) {
        case MATRIX:
            disarray_matrix(g);
            break;
        case BITS:
            disarray_bits(g);
            break;
        default:
            break;
    }
}

bool offset(game* g) {
    if (g == NULL || g->b == NULL) {
        fprintf(stderr, "Game is null\n");
        return false;
    }

    if (g->white_queue->len == 0 || g->black_queue->len == 0) {
        return false;
    }

    pos last_white = posqueue_remback(g->white_queue);
    board_set(g->b, last_white, EMPTY);
    pos last_black = posqueue_remback(g->black_queue);
    board_set(g->b, last_black, EMPTY);

    /* Updating the queues. */
    pq_entry* current = g->black_queue->head;
    while (current != NULL) {
        if (current->p.c == last_white.c && current->p.r < last_white.r) {
            current->p.r++;
        }
        if (current->p.c == last_black.c && current->p.r < last_black.r) {
            current->p.r++;
        }
        current = current->next;
    }

    current = g->white_queue->head;
    while (current != NULL) {
        if (current->p.c == last_white.c && current->p.r < last_white.r) {
            current->p.r++;
        }
        if (current->p.c == last_black.c && current->p.r < last_black.r) {
            current->p.r++;
        }
        current = current->next;
    }

    /* Updating the board: */
    board_clear(g->b);
    current = g->black_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, BLACK);
        current = current->next;
    }

    current = g->white_queue->head;
    while (current != NULL) {
        board_set(g->b, current->p, WHITE);
        current = current->next;
    }

    next_turn(g);
    return true;
}

outcome game_outcome(game* g) {
    if (g == NULL || g->b == NULL) {
        fprintf(stderr, "Error: Game or board is NULL\n");
        exit(1);
    }

    bool board_full = true;

    for (unsigned int i = 0; i < g->b->height; i++) {
        for (unsigned int j = 0; j < g->b->width; j++) {
            pos current_pos = make_pos(i, j);
            cell piece = board_get(g->b, current_pos);

            if (piece == EMPTY) {
                board_full = false;
                continue;
            }

            /*Horizontal check*/
            if (j + g->run <= g->b->width) {
                bool win = true;
                for (unsigned int k = 0; k < g->run; k++) {
                    pos check_pos = make_pos(i, j + k);
                    if (board_get(g->b, check_pos) != piece) {
                        win = false;
                        break;
                    }
                }
                if (win) return (piece == BLACK) ? BLACK_WIN : WHITE_WIN;
            }

            /*Vertical check*/
            if (i + g->run <= g->b->height) {
                bool win = true;
                for (unsigned int k = 0; k < g->run; k++) {
                    pos check_pos = make_pos(i + k, j);
                    if (board_get(g->b, check_pos) != piece) {
                        win = false;
                        break;
                    }
                }
                if (win) return (piece == BLACK) ? BLACK_WIN : WHITE_WIN;
            }

            /*Left diagonal check*/
            if (i + g->run <= g->b->height && j + g->run <= g->b->width) {
                bool win = true;
                for (unsigned int k = 0; k < g->run; k++) {
                    pos check_pos = make_pos(i + k, j + k);
                    if (board_get(g->b, check_pos) != piece) {
                        win = false;
                        break;
                    }
                }
                if (win) return (piece == BLACK) ? BLACK_WIN : WHITE_WIN;
            }

            /*Right diagonal check*/
            if (i + g->run <= g->b->height && j >= g->run - 1) {
                bool win = true;
                for (unsigned int k = 0; k < g->run; k++) {
                    pos check_pos = make_pos(i + k, j - k); 
                    if (board_get(g->b, check_pos) != piece) {
                        win = false;
                        break;
                    }
                }
                if (win) return (piece == BLACK) ? BLACK_WIN : WHITE_WIN;
            }
        }
    }

    if (board_full) {
        return DRAW;
    }

    return IN_PROGRESS;
}