#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "logic.h"
#include "pos.h"
#include "board.h"


/*Pos tests*/

Test(pos, make_pos_test) {
    pos p = make_pos(3, 4);
    cr_assert_eq(p.r, 3, "Row should be 3");
    cr_assert_eq(p.c, 4, "Column should be 4");
}

Test(posqueue, posqueue_tests) {
    posqueue* q = posqueue_new();
    cr_assert_not_null(q, "Queue should be initialized");

    pos p1 = make_pos(0, 0);
    pos p2 = make_pos(1, 1);
    pos p3 = make_pos(2, 2);

    pos_enqueue(q, p1);
    pos_enqueue(q, p2);
    pos_enqueue(q, p3);

    cr_assert_eq(q->len, 3, "Queue length should be 3 after enqueuing 3 elements");

    pos dequeued = pos_dequeue(q);
    cr_assert_eq(dequeued.r, 0, "Dequeued row should be 0");
    cr_assert_eq(dequeued.c, 0, "Dequeued column should be 0");
    cr_assert_eq(q->len, 2, "Queue length should be 2 after dequeue");

    pos rembacked = posqueue_remback(q);
    cr_assert_eq(rembacked.r, 2, "Removed back row should be 2");
    cr_assert_eq(rembacked.c, 2, "Removed back column should be 2");
    cr_assert_eq(q->len, 1, "Queue length should be 1 after removing back");

    posqueue_free(q);
}

/*Board tests*/

Test(board, create_board_mat) {
    board* b = board_new(5, 5, MATRIX);
    cr_assert_not_null(b, "Board should be initialized");

    cr_assert_eq(b->width, 5, "Board width should be 5");
    cr_assert_eq(b->height, 5, "Board height should be 5");
    cr_assert_eq(b->type, MATRIX, "Board type should be MATRIX");

    board_free(b);
}

Test(board, create_board_bit) {
    board* b = board_new(5, 5, BITS);
    cr_assert_not_null(b, "Board should be initialized");

    cr_assert_eq(b->width, 5, "Board width should be 5");
    cr_assert_eq(b->height, 5, "Board height should be 5");
    cr_assert_eq(b->type, BITS, "Board type should be BITS");

    board_free(b);
}

Test(board, set_get) {
    board* b = board_new(5, 5, BITS);
    pos p = make_pos(1, 1);

    board_set(b, p, BLACK);
    cr_assert_eq(board_get(b, p), BLACK, "Cell at (1, 1) should be BLACK");

    board_set(b, p, WHITE);
    cr_assert_eq(board_get(b, p), WHITE, "Cell at (1, 1) should be WHITE");

    board_free(b);
}

Test(board, board_show) {
    board* b = board_new(3, 3, MATRIX);

    pos p1 = make_pos(0, 0);
    pos p2 = make_pos(1, 1);
    board_set(b, p1, BLACK);
    board_set(b, p2, WHITE);

    board_show(b); 
    /*not yet sure how to test for show*/
    board_free(b);
}

/*Logic tests*/

Test(logic, create_game) {
    game* g = new_game(4, 7, 6, BITS);
    cr_assert_not_null(g, "Game should be initialized");

    cr_assert_eq(g->run, 4, "Run should be 4");
    cr_assert_eq(g->b->width, 7, "Board width should be 7");
    cr_assert_eq(g->b->height, 6, "Board height should be 6");
    cr_assert_eq(g->player, BLACKS_TURN, "Initial turn should be WHITES_TURN");

    game_free(g);
}

Test(logic, drop_piece) {
    game* g = new_game(4, 7, 6, MATRIX);

    bool success = drop_piece(g, 0);
    cr_assert(success, "Dropping piece should succeed");
    cr_assert_eq(board_get(g->b, make_pos(5, 0)), BLACK, "Bottom cell of column 0 should be WHITE");

    success = drop_piece(g, 0);
    cr_assert(success, "Dropping second piece in same column should succeed");
    cr_assert_eq(board_get(g->b, make_pos(4, 0)), WHITE, "Next cell of column 0 should be BLACK");

    game_free(g);
}

Test(logic, drop_piece_fulcol) {
    game* g = new_game(4, 7, 2, BITS);

    drop_piece(g, 0);
    drop_piece(g, 0);
    bool success = drop_piece(g, 0);
    cr_assert(!success, "Dropping piece should not succeed as column is full");

    game_free(g);
}

Test(logic, drop_piece_postspecial) {
    game* g = new_game(2, 7, 2, MATRIX);

    drop_piece(g, 0);
    disarray(g);
    drop_piece(g, 0);
    cr_assert_eq(game_outcome(g), BLACK_WIN, "Black should have filled column 0");

    game_free(g);
}

//each disarray test doubled (one for pthread MATRIX method, other for original BITS method)
Test(logic, disarray_m) {
    game* g = new_game(4, 7, 6, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);


    disarray(g);

    cr_assert_eq(board_get(g->b, make_pos(5, 0)), WHITE, "Black piece should move to bottom");
    cr_assert_eq(board_get(g->b, make_pos(5, 1)), WHITE, "Black piece should move to bottom");

    game_free(g);
}

Test(logic, simple_disarray_m) {
    game* g = new_game(4, 7, 6, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);


    disarray(g);

    cr_assert_eq(board_get(g->b, make_pos(5, 0)), BLACK, "Black piece should stay on bottom");
    cr_assert_eq(board_get(g->b, make_pos(5, 3)), WHITE, "White piece should stay on bottom");

    game_free(g);
}

Test(logic, complex_disarray_m) {
    game* g = new_game(4, 7, 6, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 4);


    disarray(g);

    //col 0
    cr_assert_eq(board_get(g->b, make_pos(5, 0)), WHITE, "White blocker in col 0 should move to bottom");
    cr_assert_eq(board_get(g->b, make_pos(4, 0)), BLACK, "First black in col 0 should move up");

    //simple col 4
    cr_assert_eq(board_get(g->b, make_pos(5, 4)), WHITE, "SIngular white in col 4 should not move");

    //symmetric col 2
    cr_assert_eq(board_get(g->b, make_pos(5, 2)), BLACK, "Col 2 should not change");
    cr_assert_eq(board_get(g->b, make_pos(4, 2)), WHITE, "Col 2 should not change");


    game_free(g);
}

Test(logic, disarray_b) {
    game* g = new_game(4, 7, 6, BITS);

    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);


    disarray(g);

    cr_assert_eq(board_get(g->b, make_pos(5, 0)), WHITE, "Black piece should move to bottom");
    cr_assert_eq(board_get(g->b, make_pos(5, 1)), WHITE, "Black piece should move to bottom");

    game_free(g);
}

Test(logic, simple_disarray_b) {
    game* g = new_game(4, 7, 6, BITS);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);


    disarray(g);

    cr_assert_eq(board_get(g->b, make_pos(5, 0)), BLACK, "Black piece should stay on bottom");
    cr_assert_eq(board_get(g->b, make_pos(5, 3)), WHITE, "White piece should stay on bottom");

    game_free(g);
}

Test(logic, complex_disarray_b) {
    game* g = new_game(4, 7, 6, BITS);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 4);


    disarray(g);

    //col 0
    cr_assert_eq(board_get(g->b, make_pos(5, 0)), WHITE, "White blocker in col 0 should move to bottom");
    cr_assert_eq(board_get(g->b, make_pos(4, 0)), BLACK, "First black in col 0 should move up");

    //simple col 4
    cr_assert_eq(board_get(g->b, make_pos(5, 4)), WHITE, "SIngular white in col 4 should not move");

    //symmetric col 2
    cr_assert_eq(board_get(g->b, make_pos(5, 2)), BLACK, "Col 2 should not change");
    cr_assert_eq(board_get(g->b, make_pos(4, 2)), WHITE, "Col 2 should not change");


    game_free(g);
}

Test(logic, offset) {
    game* g = new_game(4, 7, 6, BITS);

    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1); 

    pq_entry* current = g->white_queue->tail;
    current->p.r = 0;


    bool success = offset(g);

    cr_assert(success, "Offset should succeed");
    cr_assert_eq(board_get(g->b, make_pos(3, 0)), EMPTY, "Oldest WHITE piece should be removed");
    cr_assert_eq(board_get(g->b, make_pos(5, 1)), EMPTY, "Newest BLACK piece should be removed");

    game_free(g);
}

Test(logic, offset_after_dissaray) {

    game* g = new_game(4, 7, 6, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0); 

    disarray(g);

    offset(g);
    
    cr_assert_eq(board_get(g->b, make_pos(5, 0)), BLACK, "Black piece should move to bottom in col 0");
    cr_assert_eq(board_get(g->b, make_pos(5, 1)), WHITE, "White piece should move to bottom in col 1");

}

Test(logic, outcome_horizontal_win) {
    game* g = new_game(4, 7, 6, BITS);

    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 2);
    drop_piece(g, 3);

    cr_assert_eq(game_outcome(g), BLACK_WIN, "Black should win with 4 in a row");

    game_free(g);
}

Test(logic, outcome_vertical_win) {
    game* g = new_game(4, 7, 6, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 0);

    cr_assert_eq(game_outcome(g), BLACK_WIN, "Black should win with 4 in a row");

    game_free(g);
}

Test(logic, outcome_ldiagonal_win) {
    game* g = new_game(4, 7, 6, BITS);

    //r1
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);

    //r2
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 3);

    //r3
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 3);

    //r4
    drop_piece(g, 3);

    cr_assert_eq(game_outcome(g), BLACK_WIN, "Black should win with 4 in a row");
}

Test(logic, outcome_rdiagonal_win) {
    game* g = new_game(4, 7, 6, MATRIX);

    //r1
    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 3);

    //r2
    drop_piece(g, 1);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 3);

    //r3
    drop_piece(g, 2);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 3);

    //r4
    drop_piece(g, 1);
    drop_piece(g, 0);
    

    cr_assert_eq(game_outcome(g), WHITE_WIN, "White should win with 4 in a row");
}

Test(logic, outcome_draw) {
    game* g = new_game(3, 3, 2, BITS);

    drop_piece(g, 0);
    drop_piece(g, 1);
    drop_piece(g, 1);
    drop_piece(g, 0);
    drop_piece(g, 0);
    drop_piece(g, 2);
    drop_piece(g, 0);
    drop_piece(g, 2);

    cr_assert_eq(game_outcome(g), DRAW, "Game should result in a draw");

    game_free(g);
}

Test(logic, outcome_in_progress) {
    game* g = new_game(3, 3, 2, MATRIX);

    drop_piece(g, 0);
    drop_piece(g, 2);

    cr_assert_eq(game_outcome(g), IN_PROGRESS, "Game should still be in progress");

    game_free(g);
}