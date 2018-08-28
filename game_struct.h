#ifndef _GAME_STRUCT
#define _GAME_STRUCT

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ROWS 3
#define COLS 3
#define NUM_SQUARES 9
#define MAX_MOVES 6
#define FALSE 0
#define TRUE 1
#define EMPTY 0
#define BASE 2

typedef int row_t[ROWS];
typedef row_t board_t[COLS];    /* i.e. board_t[ROW#][COL#] */

/* Move information struct */
typedef struct {
    int row, col, entry;
} move_t;

/* Turn information struct */   
typedef struct turn_s turn_t;   
struct turn_s {
    move_t move;
    turn_t *parent;
    int num_children;
    int win_state;      /* Flag if a turn wins */
    int bad_state;      /* Flag TRUE if chooosing guarantees opponent wins */
    turn_t **children;
};

/* Data struct for child optimisation */
typedef struct {
    turn_t *best;
    int depth;
} best_child_t;

/* Turn creation */
turn_t *make_empty_turn(void);
int create_board(turn_t *turn, board_t stor);
int next_move(turn_t *parent);
int is_game_over(turn_t *turn);
int three_in_row(int val_stor[]);
/* Game creation */
void create_children(turn_t *parent);
void update_win_states(turn_t *parent);
void update_bad_states(turn_t *parent);
void traverse_and_create(turn_t *parent);
void traverse_and_update(turn_t *parent);
void generate_children(turn_t *root, int depth);
best_child_t best_child(turn_t *parent);
void free_tree(turn_t *root, int free_root);

#endif