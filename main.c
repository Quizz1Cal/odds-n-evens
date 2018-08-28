#include "main.h"

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
    
    /* Main menu */
    print_intro();
    printf("Player vs PC (1) or two-player game (2) ? >> ");
    while ((c = getchar()) != EOF && c != ONE_C && c != TWO_C);
    int players = c;
    /* Choice of hints */
    printf("Would you like hints (y) or none? >> ");
    while ((c = getchar()) != EOF && !isalpha(c));
    int hints = (c == Y_CHAR) ? TRUE : FALSE;
    
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
