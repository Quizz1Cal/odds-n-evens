/*	Description 
	Attempted proof of the forceability of "odds & evens". 
	
 	IF ODD PLAYER CAN MAKE A WIN: Kill all siblings, take this path to win.
 	IF EVEN PLAYER CAN MAKE A WIN: Kill parent, that is bad move.
 	Ensure my code allows for odd and even wins
*/

/* 
gcc -g -o run odds_evens.c
gdb --annotate=3 run.exe
*/

/** Libraries and constants  **/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define ROWS 3
#define COLS 3
#define NUM_SQUARES 9
#define MAX_MOVES 6
#define EMPTY_STATE -1
#define BAD_STATE -2
#define PLAYER_ODD 1
#define PLAYER_EVEN 0
#define BASE 2
#define TRUE 1
#define FALSE 0
#define BANNER "==========================================================\n"

/** Typedefs and structs **/

typedef int row_t[ROWS];
typedef row_t board_t[COLS];	/* Hence board_t[ROW#][COL#] */
typedef struct {
	int row, col, entry;
} move_t;

/* Represents a turn; stores move made and points to parent and any children */		
typedef struct turn_s turn_t;	
/* Inefficient but apt, small memory cost */
struct turn_s {
	move_t move;
	turn_t *parent;
	int num_children;
	int win_state;		/* Flags if a turn wins and for who else EMPTY_STATE */
	int dec_state;		/* Flags BAD_STATE, meaning smart player won't pick */
	turn_t **children;
};

/** Function prototypes **/
turn_t *make_empty_turn(void);
int create_board(turn_t *turn, board_t stor);
int next_move(turn_t *parent);
void create_children(turn_t *parent);
void assess_children(turn_t *parent);
void generate_children(turn_t *parent, int last_turn);
int is_game_over(turn_t *turn);
int three_in_row(int val_stor[]);
void print_turn(turn_t *turn, int print_children, int board_print);
void print_board(turn_t *turn);
void print_move(turn_t *turn);
void print_history(turn_t *turn);
turn_t *random_turn(turn_t *root, int depth);
void free_tree(turn_t *root);
void reduce_siblings(turn_t *parent, turn_t *choice);
int turn_navigator(turn_t *new_game);
void traverse_and_create(turn_t *parent);

/** Main function **/

int kill_count = 0;
int total = 0;

int main(int argc, char *argv[]) {
	srand(time(0));
	/* Start a new game */
	turn_t *new_game = make_empty_turn();
	assert(new_game);
	
	/* Generate rounds for testing */
	int depth;
	printf("Input depth of generation: ");
	while ((scanf("%d", &depth)) != 1);
	generate_children(new_game, depth);
	printf("\nTotal is %d, kill count %d\n", total, kill_count);
	printf("%s", BANNER);
	turn_navigator(new_game);
	return 0;
}

/** Function definitions **/

/* Generating children */
void generate_children(turn_t *parent, int depth) {
	assert(parent);
	/* Find the children on ith layer at an increasing depth recursively */
	int i;
	for (i = 0; i < depth; i++) {
		traverse_and_create(parent);
	}
}

/* Recursively goes down to find endpoints and expands from there */
void traverse_and_create(turn_t *parent) {
	assert(parent);
	/* If a win scenario, don't need to traverse more */
	if (parent->win_state != EMPTY_STATE) {
		return;
	}
	/* If no children, make them, else, continue traversing */
	if (parent->num_children == EMPTY_STATE) {
		create_children(parent);
		assess_children(parent);
		return;
	} else {
		int i;
		for (i = 0; i < parent->num_children; i++) {
			traverse_and_create(parent->children[i]);
		}
	}
}

/* Kill all siblings and their ancestors excl choice, update parent */
void reduce_siblings(turn_t *parent, turn_t *choice) {
	assert(parent);
	assert(choice);
	assert(parent->num_children != EMPTY_STATE);
	if (parent->num_children == 1) {
		return;
	}

	print_turn(parent, TRUE, TRUE);
	int i;
	turn_t *tmp;
	for (i = 0; i < parent->num_children; i++) {
		tmp = parent->children[i];
		if (tmp != choice) {
			printf("Freeing %p...\n", tmp);
			free_tree(tmp);
		}
	}
	
	/* Substitute array of children with just winning child */
	free(parent->children);
	*parent->children = choice;
	kill_count += (parent->num_children - 1);
	parent->num_children = 1;
}

/* Check to see if a child wins, reduce child array to single turn_t if so,
	and return pointer for updating the children array */
void assess_children(turn_t *parent) {
	assert(parent);
	int i;
	turn_t *child = NULL;
	for (i = 0; i < parent->num_children; i++) {
		child = parent->children[i];
		if (child->move.entry >= 5 && child->win_state != EMPTY_STATE) {
			/* printf("\nWinning child\n");
			print_board(child);
			turn_t *tmp = child;
			while (tmp != NULL) {
				printf("(%d,%d)", tmp->win_state, tmp->dec_state);
				tmp = tmp->parent;
			}
			printf("\n"); */
			/* No siblings needed; free all excl. current (winning) child */
			reduce_siblings(parent, child);
						
			/* Go up a level, update state, check if all are BAD_STATE */
			parent->dec_state = BAD_STATE;
			turn_t *grand_parent = parent->parent;
			int j;
			for (j = 0; j < grand_parent->num_children; j++) {
				if (grand_parent->children[j]->dec_state != BAD_STATE) {
					return;	/* Not all bad yet */
				}	
			}
			turn_t *g_grand_parent = grand_parent->parent;
			grand_parent->win_state = parent->children[0]->win_state;
			g_grand_parent->dec_state = BAD_STATE;	/* g_parent is avoided */
			
			printf("ALL_BAD_STATE\n%s%s\n", BANNER, BANNER);
			printf("Great_Grand_parent:\n");
			print_turn(parent->parent->parent, TRUE, TRUE);
			printf("\nGrand_parent:\n");
			print_turn(parent->parent, TRUE, TRUE);
			printf("\nParent (input):\n");
			print_turn(parent, TRUE, TRUE);
			printf("\nChild:\n");
			print_turn(parent->children[0], FALSE, TRUE);
			
			/* First, reduce BAD_STATES to one */
			reduce_siblings(grand_parent, parent);	/*NB: could be any child */
			
			/* Now re_assess the g_grand_parent layer */
			assess_children(g_grand_parent);
			return;
		}
	}
}

/* Allocates turn_t and returns pointer */
turn_t *make_empty_turn(void) {
	turn_t *turn = (turn_t*)malloc(sizeof(turn_t));
	if (turn == NULL) {
		printf("%d, killed %d\n", total, kill_count);
	}
	assert(turn);
	total++;
	turn->move.entry = turn->move.row = turn->move.col = EMPTY_STATE;
	turn->parent = NULL;
	turn->children = NULL;
	turn->num_children = EMPTY_STATE;
	turn->win_state = EMPTY_STATE;
	turn->dec_state = EMPTY_STATE;
	return turn;
}

/* Finds all children turns for a given parent and links parent to children */
void create_children(turn_t *parent) {
	assert(parent);
	/* Create board, get number of children, get next entry to be put in */
	board_t curr_board;
	int curr_num_moves = create_board(parent, curr_board);
	int num_possible_moves = NUM_SQUARES - curr_num_moves;
	
	/* Create nodes for all potential children */
	turn_t **child_arr = (turn_t**)malloc(num_possible_moves*sizeof(turn_t*));
	turn_t *new_turn = NULL;
	int row, col, i = 0;
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			if (curr_board[row][col] == EMPTY_STATE) {
				/* A new move! Create the child and add ptr to children */
				new_turn = make_empty_turn();
				assert(new_turn);
				new_turn->move = (move_t){
					.row = row, 
					.col = col, 
					.entry = next_move(parent)
				};
				new_turn->parent = parent;
				new_turn->win_state = is_game_over(new_turn);
				*(child_arr+i) = new_turn;
				i++;
			}
		}
	}
	assert(num_possible_moves == i);
	parent->children = child_arr;
	parent->num_children = num_possible_moves;
	return;
}

/* Determines next entry */
int next_move(turn_t *parent) {
	assert(parent);
	if (parent->move.entry == EMPTY_STATE) {
		return 1;
	} else {
		return (parent->move.entry + 1);
	}
}

/* Initialises a board_t, adds in max moves (or less) & returns num_moves */
int create_board(turn_t *turn, board_t stor) {
	assert(turn);
	
	/* Initialise empty board */
	int row, col;
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			stor[row][col] = EMPTY_STATE;
		}
	}
	
	/* Backtracks to update board */
	turn_t *curr = turn;
	int num_moves = 0;
	while (curr->move.entry != EMPTY_STATE && num_moves < MAX_MOVES) {
		stor[curr->move.row][curr->move.col] = curr->move.entry;
		assert(curr->parent);
		curr = curr->parent;
		num_moves++;
	}
	return num_moves;
}

/**==========================================================================**/
/**====================PRETTY CONFIDENT IT WORKS BELOW ======================**/
/**==========================================================================**/

/* Identifies a winning turn & returns -1 if none, 0 if even and 1 if odd.
	It is assumed that only one win condition can be present by game nature. */
int is_game_over(turn_t *turn) {
	board_t curr_board;
	int curr_num_moves = create_board(turn, curr_board);
	if (curr_num_moves < 3) {
		return EMPTY_STATE;
	}
	
	int result, i, val_stor[ROWS];
	int move_row = turn->move.row, move_col = turn->move.col;
	
	/* Check row of the move made */
	for (i = 0; i < COLS; i++) {
		val_stor[i] = curr_board[move_row][i];
	}
	result = three_in_row(val_stor);
	if (result != EMPTY_STATE) {
		return result;
	}
	
	/* Check col of the move made */
	for (i = 0; i < COLS; i++) {
		val_stor[i] = curr_board[i][move_col];
	}
	result = three_in_row(val_stor);
	if (result != EMPTY_STATE) {
		return result;
	}
	
	/* Check \ diag if move in it */
	if (move_row == move_col) {
		for (i = 0; i < COLS; i++) {
			val_stor[i] = curr_board[i][i];
		}
		result = three_in_row(val_stor);
		if (result != EMPTY_STATE) {
			return result;
		}	
	}
	
	/* Check / diag if move in it */
	if (move_row == ROWS - move_col) {
		for (i = 0; i < COLS; i++) {
			val_stor[i] = curr_board[i][i];
		}
		result = three_in_row(val_stor);
		if (result != EMPTY_STATE) {
			return result;
		}	
	}

	return EMPTY_STATE;
}

/* Checks if passed int array gives win condition and returns the state */
int three_in_row(int val_stor[]) {
	int i;
	for (i = 1; i < ROWS; i++) {
		if (val_stor[i-1] == EMPTY_STATE || val_stor[i] == EMPTY_STATE) {
			return EMPTY_STATE;
		}
		if (val_stor[i-1] % BASE != val_stor[i] % BASE) {
			return EMPTY_STATE;
		}                            
	}
	return (val_stor[0] % BASE == PLAYER_EVEN) ? PLAYER_EVEN : PLAYER_ODD;
}

/* Prints information for a given turn */
void print_turn(turn_t *turn, int print_children, int board_print) {
	assert(turn);
	printf("Me: %p \t Parent: %p\n", turn, turn->parent);
	print_move(turn);
	if (board_print) {
		print_board(turn);
	}
	printf("# of children: %d\n", turn->num_children);
	if (print_children) {
		int i;
		for (i = 0; i < turn->num_children; i++) {
			turn_t *child = turn->children[i];
			assert(child);
			printf("--> One @ %p, ", child);
			print_move(child);
		}
	}
}

/* Prints board for a given turn */
void print_board(turn_t *turn) {
	assert(turn);
	board_t curr_board;
	create_board(turn, curr_board);
	int row, col;
	for (row = 0; row < ROWS; row++) {
		for (col = 0; col < COLS; col++) {
			int entry = curr_board[row][col];
			if (entry == EMPTY_STATE) {
				entry = 0;
			}
			printf("%d ", entry);
		}
		printf("\n");
	}
}

/* Prints move for a given turn */
void print_move(turn_t *turn) {
	assert(turn);
	printf("Move: %d @ %d x %d (%d) (%d)\n", turn->move.entry, 
			turn->move.row, turn->move.col, turn->win_state, turn->dec_state);
}

/* Gets a random path aiming for depth */
turn_t *random_turn(turn_t *root, int depth) {
	/* Go through up to depth layers taking random child as you go */
	int num_children, depth_count = 0, rand_child;
	turn_t *curr = root;
	num_children = curr->num_children;
	while (num_children > 0 && depth_count <= depth) {
		rand_child = rand() % num_children;
		curr = curr->children[rand_child];
		assert(curr);
		num_children = curr->num_children;
		depth_count++;
	}
	return curr;
}

/* Prints the game */
void print_history(turn_t *turn) {
	assert(turn);
	if (turn->parent != NULL) {
		printf("Cool");
		print_history(turn->parent);
	}
	print_move(turn);
	print_board(turn);
	return;
}

/* Free turn_t and all below it recursively */
void free_tree(turn_t *root) {
	assert(root);
	int i;
	for (i = 0; i < root->num_children; i++) {
		free_tree(root->children[i]);
	}
	free(root);
	kill_count += 1;
}

/* Turn navigation */
int turn_navigator(turn_t *new_game) {
	if(new_game == NULL) {
		printf("End of depth!\n");
		return FALSE;
	}
	turn_t *curr = new_game;
	printf("Move (m), back (b), quit (q) >> ");
	int c;
	while((c = getchar()) != EOF) {
		if (isalpha(c)) {
			if (c == 'q' || c == 'b' || c == 'm') {
				break;
			}
		}
	}
	if (c == 'q') {
		return FALSE;
	} else if (c == 'b') {
		curr = curr->parent;
	} else if (c == 'm') {
		printf("Enter move (row x col): ");
		int row, col;
		while ((scanf("%d x %d", &row, &col)) != 2);
		/* Look that up in the children */
		int i;
		turn_t *tmp;
		int bad_move = TRUE;
		for (i = 0; i < new_game->num_children; i++) {
			tmp = new_game->children[i];
			if (tmp->move.entry == next_move(new_game) &&
				tmp->move.row == row && tmp->move.col == col) {
				curr = tmp;
				bad_move = FALSE;
				break;
			}
		}
		if (bad_move) {
			printf("Invalid move...\n");
			return turn_navigator(curr);
		}
	}
	print_turn(curr, FALSE, TRUE);
	return turn_navigator(curr);
}

/** References **/
