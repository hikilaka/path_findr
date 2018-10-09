#ifndef SYSDEVS_MAP_H
#define SYSDEVS_MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "path.h"

struct tile_weight {
    char ch;
    int weight;
};

struct map_header {
    int width;
    int height;
    char start_ch;
    char end_ch;
    struct tile_weight *weights;
    size_t weight_cnt;
};

struct map {
    struct map_header header;
    struct point path_start;
    struct point path_end;
    char **tiles;
};

char *map_error_str(int error);

int map_parse(FILE *file, struct map *map);

void map_free(struct map *map);

int map_cost(struct map *map, struct point point);

void map_print(struct map *map);

#endif
