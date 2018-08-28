#include "game_struct.h"

/**==============================TURN CREATION===============================**/

/* Allocates turn_t and returns pointer */
turn_t *make_empty_turn(void) {
    turn_t *turn = (turn_t*)malloc(sizeof(turn_t));
    assert(turn);
    turn->move.entry = turn->move.row = turn->move.col = EMPTY;
    turn->parent = NULL;
    turn->children = NULL;
    turn->num_children = EMPTY;
    turn->win_state = FALSE;
    turn->bad_state = FALSE;
    return turn;
}

/* Initialises a board_t, adds in max moves (or less) & returns num_moves */
int create_board(turn_t *turn, board_t stor) {
    assert(turn);
    
    /* Initialise empty board */
    int row, col;
    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            stor[row][col] = EMPTY;
        }
    }
    
    /* Backtracks to update board */
    turn_t *curr = turn;
    int num_moves = 0;
    while (curr->move.entry != EMPTY && num_moves < MAX_MOVES) {
        stor[curr->move.row][curr->move.col] = curr->move.entry;
        assert(curr->parent);
        curr = curr->parent;
        num_moves++;
    }
    return num_moves;
}

/* Determines next entry */
int next_move(turn_t *parent) {
    assert(parent);
    if (parent->move.entry == EMPTY) {
        return 1;
    } else {
        return (parent->move.entry + 1);
    }
}
/* Identifies a winning turn & returns -1 if none, 0 if even and 1 if odd.
    It is assumed that only one win condition can be present by game nature. */
int is_game_over(turn_t *turn) {
    board_t curr_board;
    int curr_num_moves = create_board(turn, curr_board);
    if (curr_num_moves < 3) {
        return FALSE;
    }
    
    int result, i, val_stor[ROWS];
    int move_row = turn->move.row, move_col = turn->move.col;
    
    /* Check row of the move made */
    for (i = 0; i < COLS; i++) {
        val_stor[i] = curr_board[move_row][i];
    }
    result = three_in_row(val_stor);
    if (result) return result;
    
    /* Check col of the move made */
    for (i = 0; i < COLS; i++) {
        val_stor[i] = curr_board[i][move_col];
    }
    result = three_in_row(val_stor);
    if (result) return result;
    
    /* Check \ diag if move in it */
    if (move_row == move_col) {
        for (i = 0; i < COLS; i++) {
            val_stor[i] = curr_board[i][i];
        }
        result = three_in_row(val_stor);
        if (result) return result;  
    }
    
    /* Check / diag if move in it */
    if (move_row == ROWS - move_col - 1) {
        for (i = 0; i < COLS; i++) {
            val_stor[i] = curr_board[i][ROWS - i - 1];
        }
        result = three_in_row(val_stor);
        if (result) return result;
    }

    return FALSE;
}

/* Checks if passed int array gives win condition and returns the state */
int three_in_row(int val_stor[]) {
    int i;
    for (i = 1; i < ROWS; i++) {
        if (val_stor[i-1] == EMPTY || val_stor[i] == EMPTY) {
            return FALSE;
        }
        if (val_stor[i-1] % BASE != val_stor[i] % BASE) {
            return FALSE;
        }                            
    }
    return TRUE;
}

/**==============================GAME CREATION===============================**/

/* Finds all children turns for a given parent and links parent to children */
void create_children(turn_t *parent) {
    assert(parent);
    /* Create board, get number of children, get next entry to be put in */
    board_t curr_board;
    int curr_num_moves = create_board(parent, curr_board);
    int num_possible_moves = NUM_SQUARES - curr_num_moves;
    
    /* Create nodes for all potential children */
    turn_t **child_arr = (turn_t**)malloc(num_possible_moves*sizeof(turn_t*));
    assert(child_arr);
    turn_t *new_turn;
    int entry = next_move(parent);
    int row, col, i = 0;
    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            if (curr_board[row][col] == EMPTY) {
                /* A new move! Create the child and add ptr to children */
                new_turn = make_empty_turn();
                assert(new_turn);
                new_turn->move = (move_t){
                    .row = row, 
                    .col = col, 
                    .entry = entry
                };
                new_turn->parent = parent;
                new_turn->win_state = is_game_over(new_turn);
                *(child_arr+i) = new_turn;
                i++;
            }
        }
    }
    parent->children = child_arr;
    parent->num_children = num_possible_moves;
    return;
}

/* If a winner, renders parent's parent BAD */
void update_bad_states(turn_t *parent) {
    assert(parent);
    if (parent->num_children == EMPTY && parent->win_state) {
        /* An winning scenario at the frontier of generations */
        parent->parent->bad_state = TRUE;
        return;
    } 
}

/* Check if all parent's children are BAD, and make it a winner if so */
void update_win_states(turn_t *parent) {
    assert(parent);
    if (parent->num_children && parent->win_state == FALSE) {
        int i;
        if (parent->bad_state == FALSE) {
            /* Need to check if all children are bad or not. */
            int children_all_bad_state = TRUE;
            for (i = 0; i < parent->num_children; i++) {
                if (parent->children[i]->bad_state != TRUE) {
                    children_all_bad_state = FALSE;
                    break;
                }
            }
            if (children_all_bad_state) {
                parent->win_state = TRUE;
                if (parent->parent != NULL) {
                    parent->parent->bad_state = TRUE;
                }
            }       
        }
    }
}

/* Recursively goes down to find endpoints and creates new turns */
void traverse_and_create(turn_t *parent) {
    assert(parent);
    if (parent->win_state || parent->bad_state) {
        if (parent->move.entry != EMPTY) return;
    }
    if (parent->num_children == EMPTY) {
        create_children(parent);
        return;
    } else {
        int i;
        for (i = 0; i < parent->num_children; i++) {
            traverse_and_create(parent->children[i]);
        }
    }
}

/* Recursion to find tree endpoints and updates win/bad states from bottom up */
void traverse_and_update(turn_t *parent) {
    assert(parent);
    if (parent->num_children) {
        int i;
        for (i = 0; i < parent->num_children; i++) {
            traverse_and_update(parent->children[i]);
        }
    }
    update_bad_states(parent);
    update_win_states(parent);
}

/* Generate children depth extra layers starting at root */
void generate_children(turn_t *root, int depth) {
    assert(root);
    /* Generate the children at endpoints of tree, depth times */
    int i;
    for (i = 0; i < depth; i++) {
        traverse_and_create(root);
        traverse_and_update(root);
    }
}

/* Determines best option for opponent, and the depth from parent, as struct 
 * Note: if many children with winning tags, does not compare them */
best_child_t best_child(turn_t *parent) {
    assert(parent);
    int i;
    turn_t *tmp;
    best_child_t tmp_best = {.best = NULL, .depth = 0};
    best_child_t curr_best = tmp_best;
    for (i = 0; i < parent->num_children; i++) {
        tmp = parent->children[i];
        if (tmp->win_state) {   /* Child will lead to a win; this is best */
            if (tmp->num_children) {
                curr_best = best_child(tmp);
                curr_best.best = tmp;
                break;
            } else {
                curr_best = (best_child_t) {.best = tmp, .depth = 0};
                break;
            }
        } else if (tmp->bad_state) {    /* Bad; avoid at all cost */
            continue;
        } else if (curr_best.best == NULL) {    /* First non_bad, non_win */
            /* Calculates best option for player using PARENT, i.e. worst 
                move for opponent */
            curr_best = best_child(tmp);    
            curr_best.best = tmp;
        } else {
            tmp_best = best_child(tmp);
            tmp_best.best = tmp;
            /* Since the best_child here determines the path fastest for the 
                parent, and not opponent, choose worst of tmp and curr_best */
            if (tmp_best.depth > curr_best.depth) curr_best = tmp_best;
        }
    }
    if (curr_best.best == NULL) {/* All bad children; pick the least worst */
        for (i = 0; i < parent->num_children; i++) {
            tmp = parent->children[i];
            if (curr_best.best == NULL) {   /* First bad */
                curr_best = best_child(tmp);    
                curr_best.best = tmp;
            } else {    /* Compare curr_best and tmp */
                best_child_t tmp_best = best_child(tmp);
                tmp_best.best = tmp;
                if (tmp_best.depth > curr_best.depth) curr_best = tmp_best;
            }
        }
    }
    curr_best.depth++;
    return curr_best;
}

/* Free turn_t if free_root and all below it recursively */
void free_tree(turn_t *root, int free_root) {
    assert(root);
    int i;
    for (i = 0; i < root->num_children; i++) {
        free_tree(root->children[i], TRUE);
    }
    if (free_root) {
        free(root);
    }
}