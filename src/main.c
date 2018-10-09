#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "map.h"
#include "path.h"
#include "ordered_set.h"

void read_map(char *filename, struct map *map) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Unable to open map file");
        exit(EXIT_FAILURE);
        return;
    }

    int error = map_parse(file, map);

    if (error) {
        fprintf(stderr, "Error parsing map: %s\n", map_error_str(error));
        exit(EXIT_FAILURE);
        return;
    }
}

void write_map(char *filename, struct map *map) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Unable to save map file");
        exit(EXIT_FAILURE);
        return;
    }

    fprintf(file, "width = %d\n", map->header.width);
    fprintf(file, "height = %d\n", map->header.height);
    fprintf(file, "start = %c\n", map->header.start_ch);
    fprintf(file, "end = %c\n", map->header.end_ch);

    for (int y = 0; y < map->header.height; ++y) {
        for (int x = 0; x < map->header.width; ++x) {
            fputc(map->tiles[y][x], file);
        }
        fputc('\n', file);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("missing parameter file\n");
        printf("usage: %s <in_map_file> <out_map_file>\n\n", argv[0]);
        return 0;
    }

    struct map map;
    memset(&map, 0, sizeof(struct map));

    read_map(argv[1], &map);

    printf("calculating path...");
    clock_t start, end;
    double time_duration;

    struct path *path = NULL;

    start = clock();

    int error = path_find(&map, &path);

    end = clock();

    if (error) {
        printf(" error calculating path!\n");
        return EXIT_FAILURE;
    }
    
    time_duration = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf(" done! took %.02f seconds.\n", time_duration);

    struct path *p = path->next;
    while (p != NULL) {
        map.tiles[p->point.y][p->point.x] = '.';
        p = p->next;
    }
    map.tiles[map.path_end.y][map.path_end.x] = map.header.end_ch;
    write_map(argv[2], &map);

    map_free(&map);
    path_free(path);
    return 0;
}
