/**=================================DESCRIPTION=============================**/

/* The program attempts to brute-force solve a game that, to my knowledge, was
 * invented by an associate and titled "Odds and evens". Highly similar to
 * tic-tac-toe, the basic rules of the game are as follows:
 *  - One player is designated odd (and goes first) and the other even. 
 *  - Given a 3x3 square grid, players take turns writing numbers into squares 
 *      increasing as they go e.g. player 1 writes 1, player 2 then writes 2, 
 *      then player 1 will write 3, etc.
 *  - The maximum number of numbers each player can have on the board is three.
 *      hence, when player 1 writes a 7, the 1 must be erased AFTER the 7 is 
 *      written down. When player 1 writes 8, the 2 is erased AFTER, etc...
 *  - Aim of game is for one player to get 3 in a row. 
 *  - Ties are permissible at each player's discretion.
 * The program generates board states for a specified depth of generation, and
 * simultaneously evaluates winning moves; moves that would be avoided; and
 * moves that ultimately, force a win in either player's favour. 
 * Given sufficient generation, it can be shown that player 1 can always win
 * by playing their first move on any central point on the 4 board edges.
 * The simulator called at runtime allows for users to play against 
 * themselves. An "AI" function best_child can be used to determine best move
 * for a given turn, facilitating an automated second player. 
 *
 * Author: Callum Holmes
 * Date: July 2018
 *
 * Implementation history:
 *  1.  Used turn structures storing every board state explicitly. Memory waste.
 *  2.  Used structures storing moves, and board states derived implicitly.
        Serious errors were encountered when attempting to simplify tree
        pathways to maximise memory availability for deeper generations.
 *  3.  Excluded any attempt to simplify/delete tree pathways, leaving all
        board states completely available. 
 */
 
/**==========================LIBRARIES AND CONSTANTS=========================**/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#define ROWS 3
#define COLS 3
#define NUM_SQUARES 9
#define MAX_MOVES 6
#define FALSE 0
#define TRUE 1
#define EMPTY 0
#define BASE 2
#define ANY_CHILD -1
#define ZERO_C '0'
#define ONE_C '1'
#define TWO_C '2'
#define Y_CHAR 'y'
#define BAD_ENTRY 11
#define BANNER "=============================================================\n"

/**======================TYPEDEFS, STRUCTS, PROTOTYPES=======================**/

/* Board typedef */
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
/* Generation data storage struct */
typedef struct {
    int num_children;
    int count_num;
    int count_win;
    int count_bad;
} data_t;
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
void free_tree(turn_t *root, int free_root);
/* Game simulation */
int simulator(turn_t *new_game, int hints, int board_print, int one_player, 
        int comp_turn);
best_child_t best_child(turn_t *parent);
/* Printing functions */
void print_turn(turn_t *turn, int print_children, int board_print, int hints);
void print_board(turn_t *turn);
void print_move(turn_t *turn, int hints);
void help_information(void);
/* Analytical */
data_t *make_empty_data(int num_children);
void traverse_and_analyze(turn_t *parent, int depth, data_t *total, 
        data_t *depth_total, data_t **depth_sorted);
void print_depth_data(data_t *depth_total, data_t **depth_sorted);
void branching_data(turn_t *root, int depth);

/**==============================MAIN FUNCTION===============================**/

int main(int argc, char *argv[]) {
    /* Simulate a new game */
    turn_t *new_game = make_empty_turn();
    assert(new_game);
    int depth;
    printf("Input depth of generation (13 is ideal): ");
    while ((scanf("%d", &depth)) != 1);
    generate_children(new_game, depth);
    
    /* Obtain data */
    printf("Print data for generations (y), or continue (n)? >> ");
    int c;
    while ((c = getchar()) != EOF && !isalpha(c));
    if (c == Y_CHAR) branching_data(new_game, depth);
    
    /* Introduction */
    printf("\n%s", BANNER);
    printf("This is the turn simulator for the game of Odds & Evens.\n");
    printf("This uses turns generated by code to play the game.\n");
    printf("This program facilitates player vs computer gameplay and\n");
    printf("two-player games, and you can enable hints if desired to\n");
    printf("to label moves as \"path to victory\" (you will win), or\n");
    printf("\"avoid this move\" (avoid, otherwise smart opponents win).\n");
    printf("Enter the lowercase letter code as indicated in the prompt\n");
    printf("To use the simulator, make moves, backtrack, etc. Enjoy!\n");
    printf("%s\n", BANNER);
    
    /* Choice of player number */
    printf("Player vs PC (1) or two-player game (2) ? >> ");
    while ((c = getchar()) != EOF && c != ONE_C && c != TWO_C);
    int players = c;
    /* Choice of hints */
    printf("Would you like hints (y) or none? >> ");
    while ((c = getchar()) != EOF && !isalpha(c));
    int hints = (c == Y_CHAR) ? TRUE : FALSE;
    
    /* Start actual game navigator */
    if (players == ONE_C) { /* One player AI functionality */
        printf("Would you like to go first (y) or not? >> ");
        while ((c = getchar()) != EOF && !isalpha(c));
        printf("\nLET THE GAME BEGIN....\n");
        if (c == Y_CHAR) {
            simulator(new_game, hints, TRUE, TRUE, FALSE);
        } else {
            simulator(new_game, hints, TRUE, TRUE, TRUE);
        }
    } else {    /* 2 player functionality */
        printf("\nLET THE GAME BEGIN....\n");
        simulator(new_game, hints, TRUE, FALSE, FALSE);
    }
    
    free_tree(new_game, TRUE);
    return 0;
}

/**===========================FUNCTION DEFINITIONS===========================**/

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

/**=============================GAME SIMULATION==============================**/

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
    s
/**============================GAME EXPLORATION==============================**/

/* Turn navigation */
int simulator(turn_t *root, int hints, int board_print, int one_player, 
        int comp_turn) {
    assert(root);
    turn_t *curr = root;
    printf("%s", BANNER);
    /* Computer moves */
    if (one_player && comp_turn && root->num_children) {
        printf("COMPUTER MAKES A MOVE...\n");
        generate_children(curr, 1);
        curr = best_child(curr).best;
        return simulator(curr, hints, TRUE, one_player, !comp_turn);
    }
    /* Handling finished games */
    if (!root->num_children && root->win_state) {
        printf("GAME OVER... ");
        if (root->move.entry % BASE) {
            printf("ODD WINS!");
        } else if (!one_player) {
            printf("EVEN WINS!");
        }
        if (one_player && comp_turn) {
            printf("... AND HUMANITY WON! AI CANNOT USURP US!\n");
        }
        return EXIT_SUCCESS;
    }
    print_turn(curr, (hints && board_print), board_print, hints);
    
    /* User input handler and resolver */
    printf("Move (m) back (b) print (p) help (h) quit (q) automatic (o) >> ");
    int c;
    while((c = getchar()) != EOF) {
        if (!isalpha(c)) continue;
        if (c == 'p') return simulator(curr, hints, TRUE, one_player, comp_turn);
        if (c == 'b') {
            if (curr->parent != NULL) curr = curr->parent;
            if (one_player && curr->parent != NULL) curr = curr->parent;
            return simulator(curr, hints, TRUE, one_player, comp_turn);
        } else if (c == 'q') {
            printf("Thank you for playing :)\n");
            return EXIT_SUCCESS;
        } else if (c == 'h') {
            help_information();
            return simulator(curr, hints, FALSE, one_player, comp_turn);
        } else if (c == 'g') {
            int depth;
            printf("Enter depth of generation: ");
            scanf("%d", &depth);
            generate_children(curr, depth);
            return simulator(curr, hints, FALSE, one_player, comp_turn);
        } else if (c == 'o' && hints) {
            printf("Playing strongest move...\n");
            generate_children(curr, 1);
            curr = best_child(curr).best;
            return simulator(curr, hints, TRUE, one_player, !comp_turn);
        } else if (c == 'o') {
        	printf("Automatic is disabled when hints is disabled.\n");
        } else if (c == 'm') {
            generate_children(curr, 1);
            printf("Enter move (row x col): ");
            int row, col;
            row = col = BAD_ENTRY;
            while (scanf("%dx%d", &row, &col) != 2) {
            	printf("Invalid format, must be row# x col#...\n");
            };
            /* Look up user entry in children */
            int i;
            for (i = 0; i < curr->num_children; i++) {
                turn_t *tmp = curr->children[i];
                if (tmp->move.entry == next_move(curr) &&
                        tmp->move.row == row && tmp->move.col == col) {
                    return simulator(tmp, hints, TRUE, one_player, !comp_turn);
                }
            }
            printf("Invalid move...\n");
            return simulator(curr, hints, FALSE, one_player, comp_turn);
        }
    }
    return EXIT_SUCCESS;
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

/**===============================PRINT INFO=================================**/

/* Prints information for a given turn */
void print_turn(turn_t *turn, int print_children, int board_print, int hints) {
    assert(turn);
    if (board_print) {
        print_board(turn);
    }
    if (print_children) {
        printf("# of children: %d\n", turn->num_children);
        int i;
        for (i = 0; i < turn->num_children; i++) {
            turn_t *child = turn->children[i];
            assert(child);
            printf("--> ");
            print_move(child, hints);
        }
    }
}

/* Prints board for a given turn with headings */
void print_board(turn_t *turn) {
    assert(turn);
    board_t curr_board;
    create_board(turn, curr_board);
    int row, col;
    printf("x | 0  1  2\n--|---------\n");
    for (row = 0; row < ROWS; row++) {
        printf("%d | ", row);
        for (col = 0; col < COLS; col++) {
            int entry = curr_board[row][col];
            printf("%-2d ", entry);
        }
        printf("\n");
    }
}

/* Prints move for a given turn */
void print_move(turn_t *turn, int hints) {
    assert(turn);
    printf("Move: %d @ %d x %d", turn->move.entry,  
        turn->move.row, turn->move.col);
    if (turn->win_state && hints) printf(" (PATH TO VICTORY)");
    if (turn->bad_state && hints) printf(" (AVOID THIS MOVE)");
    printf("\n");
}


/* Help information printer */
void help_information(void) {
    printf("- Move: use to enter next move to make\n");
    printf("--> Enter row #, x, & col #, e.g. 1 x 1 is centre\n");
    printf("- Back: returns to previous move\n");
    printf("- Quit: exits simulator\n");
    printf("- Generate: makes more moves in computer for play\n");
    printf("- Print: prints turn again\n");
    printf("- Automatic: makes strongest move IF hints enabled\n");
    return;
}

/**===============================ANALYTICAL=================================**/

/* Initialise data_t and return copy */
data_t *make_empty_data(int num_children) {
    data_t *new = (data_t*)malloc(sizeof(data_t));
    assert(new);
    new->num_children = num_children;   /* serves as an index basket */
    new->count_num = new->count_win = new->count_bad = 0;
    return new;
}

/* Traverse tree by depth and updating child_data structs */
void traverse_and_analyze(turn_t *parent, int depth, data_t *total, 
        data_t *depth_total, data_t **depth_sorted) {
    assert(parent);
    if (depth > 0) {
        /* Explore the children */
        int i;
        for (i = 0; i < parent->num_children; i++) {
            traverse_and_analyze(parent->children[i], depth - 1, total, 
                    depth_total, depth_sorted);
        }
    } else {
        /* You are at appropriate depth, add where necessary */
        int num_children = parent->num_children;
        total->count_num++;
        depth_total->count_num++;
        depth_sorted[num_children]->count_num++;
        if (parent->win_state) {
            total->count_win++;
            depth_total->count_win++;
            depth_sorted[num_children]->count_win++;
        }
        if (parent->bad_state) {
            total->count_bad++;
            depth_total->count_bad++;
            depth_sorted[num_children]->count_bad++;
        }
    }
}

/* Prints analytical data */
void print_depth_data(data_t *depth_total, data_t **depth_sorted) {
    assert(depth_total);
    assert(depth_sorted);
    int i;
    for (i = 0; i < NUM_SQUARES+1; i++) {
        if (depth_sorted[i]->count_num == 0) continue;
        printf("\t(%d)\t%d turns, %d wins, %d bads\n", 
            depth_sorted[i]->num_children,
            depth_sorted[i]->count_num,
            depth_sorted[i]->count_win,
            depth_sorted[i]->count_bad);
    }
    printf("\tTotals:\t%d turns, %d wins, %d bads\n", 
            depth_total->count_num,
            depth_total->count_win,
            depth_total->count_bad);
}

/* Diagnostics for branching information */
void branching_data(turn_t *root, int depth) {
    data_t *total, *depth_total;
    data_t *depth_sorted[NUM_SQUARES+1];
    total = make_empty_data(ANY_CHILD);
    
    /* Analyze & print layer by layer */
    int i, j;
    for (i = 0; i < depth + 1; i++) {
        printf("Depth: %d\n", i);
        depth_total = make_empty_data(ANY_CHILD);
        for (j = 0; j < NUM_SQUARES+1; j++) {
            depth_sorted[j] = make_empty_data(j);
        }
        traverse_and_analyze(root, i, total, depth_total, depth_sorted);
        print_depth_data(depth_total, depth_sorted);
        free(depth_total);
        for (j = 0; j < NUM_SQUARES+1; j++) {
            free(depth_sorted[j]);
        }
    }
    printf("Grand totals: %d turns, %d wins, %d bads\n", 
            total->count_num,
            total->count_win,
            total->count_bad);
    free(total);
}
