#include "ordered_set.h"

#include <stdlib.h>
#include <string.h>

int ordered_set_init(struct ordered_set *set) {
    if (set == NULL) {
        return ordered_set_error;
    }
    set->size = 0;
    set->node_cnt = ORDERED_SET_DEFAULT_NODES;
    set->nodes = malloc(sizeof(struct ordered_set_node) * ORDERED_SET_DEFAULT_NODES);
    return ordered_set_ok;
}

int ordered_set_release(struct ordered_set *set) {
    if (set == NULL) {
        return ordered_set_error;
    }
    set->node_cnt = 0;
    if (set->nodes) {
        free(set->nodes);
        set->nodes = NULL;
    }
    return ordered_set_ok;
}

static int compare_nodes(const void *s1, const void *s2) {
    struct ordered_set_node *a = (struct ordered_set_node *)s1;
    struct ordered_set_node *b = (struct ordered_set_node *)s2;

    return a->priority - b->priority;
}

int ordered_set_insert(struct ordered_set *set, struct point point, double priority) {
    if (set == NULL) {
        return ordered_set_error;
    }
    while (set->size >= set->node_cnt) {
        set->node_cnt *= 2;
        set->nodes = realloc(set->nodes, sizeof(struct ordered_set_node) * set->node_cnt);
    }

    for (size_t i = 0; i < set->size; ++i) {
        struct point node = set->nodes[i].point;

        // if node already exists, update priorty, sort, and return
        if (node.x == point.x && node.y == point.y) {
            set->nodes[i].priority = priority;
            qsort(set->nodes, set->size, sizeof(struct ordered_set_node), compare_nodes);
            return ordered_set_ok;
        }
    }

    // node does not exist, set to last element and sort
    memcpy(&set->nodes[set->size].point, &point, sizeof(struct point));
    set->nodes[set->size].priority = priority;
    set->size += 1;
    qsort(set->nodes, set->size, sizeof(struct ordered_set_node), compare_nodes);
    return ordered_set_ok;
}

int ordered_set_dec_priorty(struct ordered_set *set, struct point point, double priority) {
    if (set == NULL) {
        return ordered_set_error;
    }

    for (size_t i = 0; i < set->size; ++i) {
        struct point node = set->nodes[i].point;

        // if node already exists, update priorty, sort, and return
        if (node.x == point.x && node.y == point.y) {
            set->nodes[i].priority = priority;
            qsort(set->nodes, set->size, sizeof(struct ordered_set_node), compare_nodes);
            return ordered_set_ok;
        }
    }

    return ordered_set_ok;
}

int ordered_set_remove_min(struct ordered_set *set, struct point *out) {
    if (set == NULL) {
        return ordered_set_error;
    }
    if (set->size == 0) {
        return ordered_set_error;
    }
    if (out != NULL) {
        memcpy(out, &set->nodes[0].point, sizeof(struct point));
    }
    set->size -= 1;
    memmove(&set->nodes[0], &set->nodes[1], sizeof(struct ordered_set_node) * set->size);
    return ordered_set_ok;
}

void ordered_set_print(struct ordered_set *set, FILE *file) {
    fprintf(file, "ordered_set { size=%zu, node_cnt=%zu, nodes=[ ",
        set->size, set->node_cnt);

    for (size_t i = 0; i < set->size; i++) {
        struct point node = set->nodes[i].point;
        fprintf(file, "{(%d, %d), %f}, ", node.x, node.y, set->nodes[i].priority);
    }
    fprintf(file, "] }\n");
}
