#include "path.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "ordered_set.h"

void path_init(struct path **path, struct point *start) {
    *path = calloc(1, sizeof(struct path));
    (*path)->point.x = start->x;
    (*path)->point.y = start->y;
}

void path_free(struct path *path) {
    struct path *next;

    do {
        next = path->next;
        free(path);
        path = next;
    } while (next != NULL);
}

void path_append(struct path *path, struct point *point) {
    struct path *prev = path;
    path = path->next;

    while (path != NULL) {
        prev = path;
        path = path->next;
    }

    path = calloc(1, sizeof(struct path));
    path->point.x = point->x;
    path->point.y = point->y;
    prev->next = path;
}

void path_reverse(struct path **path) {
    struct path *prev = NULL;
    struct path *cur = *path;
    struct path *next;

    while (cur != NULL) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    *path = prev;
}

void path_print(struct path *path) {
    struct path *next;

    printf("[");
    do {
        next = path->next;
        printf("(%d, %d)", path->point.x, path->point.y);
        path = next;
        if (next != NULL) {
            printf(", ");
        }
    } while (next != NULL);
    printf("]\n");
}

double point_dist(struct point a, struct point b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

int path_find(struct map *map, struct path **path) {
    // error checking macro
    #define call(f, ...) do { \
        int error = f(__VA_ARGS__); \
        if (error != 0) return error; \
    } while (0);

    size_t node_cnt = map->header.width * map->header.height;
    double dist[node_cnt];
    struct point prev[node_cnt];
    struct ordered_set unvisited;

    call(ordered_set_init, &unvisited);

    dist[map->path_start.x + (map->path_start.y * map->header.width)] = 0;

    for (int y = 0; y < map->header.height; ++y) {
        for (int x = 0; x < map->header.width; ++x) {
            struct point p = { x, y };
            
            if ((p.x != map->path_start.x) || (p.y != map->path_start.y)) {
                dist[p.x + (p.y * map->header.width)] = INFINITY;
            }

            call(ordered_set_insert, &unvisited, p, dist[p.x + (p.y * map->header.width)]);
        }
    }

    struct point p;
    struct point neighbors[8];
    size_t neighbor_cnt = 0;

    #define add_neigh(nx, ny) \
        do { \
            neighbors[neighbor_cnt].x = (nx); \
            neighbors[neighbor_cnt].y = (ny); \
            neighbor_cnt += 1; \
        } while (0);

    while (unvisited.size > 0) {
        call(ordered_set_remove_min, &unvisited, &p);
        neighbor_cnt = 0;

        if (p.x > 0) {
            add_neigh(p.x - 1, p.y);

            if (p.y > 0) {
                add_neigh(p.x - 1, p.y - 1);
            }
            if (p.y + 1 < map->header.height) {
                add_neigh(p.x - 1, p.y + 1);
            }
        }
        if (p.y > 0) {
            add_neigh(p.x, p.y - 1);
            if (p.x + 1 < map->header.width) {
                add_neigh(p.x + 1, p.y - 1);
            }
        }
        if (p.x + 1 < map->header.width) {
            add_neigh(p.x + 1, p.y);
        }
        if (p.y + 1 < map->header.height) {
            add_neigh(p.x, p.y + 1);
            if (p.x + 1 < map->header.width) {
                add_neigh(p.x + 1, p.y + 1);
            }
        }

        for (size_t i = 0; i < neighbor_cnt; ++i) {
            struct point n = neighbors[i];
            double alt = dist[p.x + (p.y * map->header.width)] + point_dist(p, n) + map_cost(map, n);

            if (alt < dist[n.x + (n.y * map->header.width)]) {
                dist[n.x + (n.y * map->header.width)] = alt;
                memcpy(&prev[n.x + (n.y * map->header.width)], &p, sizeof(struct point));
                call(ordered_set_insert, &unvisited, n, alt);
            }
        }
    }

    #undef add_neigh

    // now that the costs have been calculated, choose
    // the end node & step backwards to the start node
    path_init(path, &map->path_end);

    p = prev[map->path_end.x + (map->path_end.y * map->header.width)];
    while (p.x != map->path_start.x || p.y != map->path_start.y) {
        path_append(*path, &p);
        p = prev[p.x + (p.y * map->header.width)];
    }

    path_append(*path, &map->path_start);
    path_reverse(path);

    call(ordered_set_release, &unvisited);

    #undef call
    return 0;
}
