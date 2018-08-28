#include "analytic.h"

/* Initialise data_t and return copy */
data_t *make_empty_data(int num_children) {
    data_t *new_data = (data_t*)malloc(sizeof(data_t));
    assert(new_data);
    new_data->num_children = num_children;   /* serves as an index basket */
    new_data->count_num = new_data->count_win = new_data->count_bad = 0;
    return new_data;
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