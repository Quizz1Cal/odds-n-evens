#ifndef _ANALYTIC
#define _ANALYTIC

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "game_struct.h"

#define ANY_CHILD -1

/* Generation data storage struct */
typedef struct data_s data_t;
struct data_s {
    int num_children;
    int count_num;
    int count_win;
    int count_bad;
};

data_t *make_empty_data(int num_children);
void traverse_and_analyze(turn_t *parent, int depth, data_t *total, 
        data_t *depth_total, data_t **depth_sorted);
void print_depth_data(data_t *depth_total, data_t **depth_sorted);
void branching_data(turn_t *root, int depth);

#endif