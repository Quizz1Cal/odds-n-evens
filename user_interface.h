#ifndef _USER_INTERFACE
#define _USER_INTERFACE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "game_struct.h"

#define BAD_ENTRY 11
#define BANNER "=============================================================\n"

/* Game simulation */
int simulator(turn_t *new_game, int hints, int board_print, int one_player, 
        int comp_turn);
/* Printing functions */
void print_turn(turn_t *turn, int print_children, int board_print, int hints);
void print_board(turn_t *turn);
void print_move(turn_t *turn, int hints);
void help_information(void);
void print_intro(void);

#endif