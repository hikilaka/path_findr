#ifndef SYSD_ORDERED_SET
#define SYSD_ORDERED_SET

#include <stdio.h>
#include <stddef.h>

#include "path.h"

#define ORDERED_SET_DEFAULT_NODES 16

struct ordered_set_node {
    struct point point;
    double priority;
};

struct ordered_set {
    size_t size;
    size_t node_cnt;
    struct ordered_set_node *nodes;
};

enum ordered_set_result {
    ordered_set_ok,
    ordered_set_error
};

// initializes an ordered set
int ordered_set_init(struct ordered_set *set);

// releases resources allocated
int ordered_set_release(struct ordered_set *set);

// inserts a point with a specific priority, if the point is already in the set,
// this function updates that point's priority
int ordered_set_insert(struct ordered_set *set, struct point point, double priority);

int ordered_set_dec_priorty(struct ordered_set *set, struct point point, double priority);

// removes the head of the set, that is, the point with least priority,
// if there are multiple points with the same priority, no order is guaranteed
int ordered_set_remove_min(struct ordered_set *set, struct point *out);

// prints an ordered set to a file ptr
void ordered_set_print(struct ordered_set *set, FILE *file);

#endif
