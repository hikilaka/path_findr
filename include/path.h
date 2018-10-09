#ifndef SYSD_PATH_H
#define SYSD_PATH_H

#include <stddef.h>

struct map;

struct point {
    int x;
    int y;
};

struct path {
    struct point point;
    struct path *next;
};

void path_init(struct path **path, struct point *start);

void path_free(struct path *path);

void path_append(struct path *path, struct point *point);

void path_reverse(struct path **path);

void path_print(struct path *path);

int path_find(struct map *map, struct path **path);

#endif
