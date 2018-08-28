#include "user_interface.h"

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
            return simulator(curr, hints, FALSE, one_player, comp_turn);
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

/* Prints introduction text */
void print_intro(void) {
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
}
