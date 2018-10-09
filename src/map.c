#include "map.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

enum map_error {
    map_ok,
    map_read_error,
    map_unknown_key,
    map_value_type_mismatch,
    map_missing_weights,
};

char *map_error_str(int error) {
    switch (error) {
        case map_ok: return "no error";
        case map_read_error: return "error reading map";
        case map_unknown_key: return "unknown header key";
        case map_value_type_mismatch: return "unexpected value for key";
        case map_missing_weights: return "missing tile weights in header";
        default: return "unknown error";
    }
}

void print_header(struct map_header *header) {
    printf("map_header { width=%d, height=%d, start_ch='%c', end_ch='%c'",
        header->width, header->height, header->start_ch, header->end_ch);

    if (header->weight_cnt > 0) {
        printf(", weights=[ ");
        for (size_t i = 0; i < header->weight_cnt; ++i) {
            printf("'%c'=%d, ", header->weights[i].ch, header->weights[i].weight);
        }
        printf("]");
    }

    printf(" }\n");
}

int parse_kv_pair(struct map_header *header, char *key_buf, char *val_buf) {
    
    if (strncmp("width", key_buf, 5) == 0) {
        int read = sscanf(val_buf, "%d", &header->width);
        if (read != 1) {
            return map_value_type_mismatch;
        }

        return map_ok;
    }else if (strncmp("height", key_buf, 6) == 0) {
        int read = sscanf(val_buf, "%d", &header->height);
        if (read != 1) {
            return map_value_type_mismatch;
        }

        return map_ok;
    } else if (strncmp("start", key_buf, 5) == 0) {
        int read = sscanf(val_buf, "%c", &header->start_ch);
        if (read != 1) {
            return map_value_type_mismatch;
        }
        
        return map_ok;
    } else if (strncmp("end", key_buf, 3) == 0) {
        int read = sscanf(val_buf, "%c", &header->end_ch);
        if (read != 1) {
            return map_value_type_mismatch;
        }
        
        return map_ok;
    } else if (strncmp("weights", key_buf, 7) == 0) {
        header->weights = calloc(header->width * header->height,
                                 sizeof(struct tile_weight));
        header->weight_cnt = 0;
        size_t offset = 0;
        size_t val_len = strlen(val_buf);
        int read;
        
        while (true) {
            read = sscanf(val_buf + offset, "%c:%d", &header->weights[header->weight_cnt].ch,
                          &header->weights[header->weight_cnt].weight);

            if (read == 2) {
                header->weight_cnt += 1;
                size_t i = offset;
                for (; i < val_len; i++) {
                    if (val_buf[i] == ',') {
                        offset = i + 1;
                        break;
                    }
                }
                if (i >= val_len) {
                    // couldn't find next
                    break;
                }
            } else {
                break;
            }
        }

        return map_ok;
    } else {
        return map_unknown_key;
    }
}

int read_map_header(FILE *file, struct map_header *header) {
    char key_buf[81];
    char val_buf[81];

    int read;
    fpos_t last_pos;

    while (true) {
        fgetpos(file, &last_pos);

        read = fscanf(file, "%s = %s\n", key_buf, val_buf);

        if (read == 2) {
            int error = parse_kv_pair(header, key_buf, val_buf);

            if (error != map_ok) {
                print_header(header);
                return error;
            }
        } else {
            // unable to read KV pair 
            break;
        }
    }

    if (ferror(file)) {
        return map_read_error;
    }

    fsetpos(file, &last_pos);
    return map_ok;
}

int read_map_body(FILE *file, struct map *map) {
    char line_buf[256];
    size_t offset = 0;
    size_t row_cnt = 0;

    while (!feof(file)) {
        char *read = fgets(line_buf, 256, file);

        if (read == NULL) {
            return map_read_error;
        }

        int line_len = strlen(line_buf);
        if (line_buf[line_len-1] == '\n') {
            line_buf[line_len-1] = 0;
            line_len -= 1;
        }

        for (int i = 0; i < line_len; ++i) {
            if (line_buf[i] == map->header.start_ch) {
                map->path_start.x = offset % map->header.width;
                map->path_start.y = row_cnt;
            } else if (line_buf[i] == map->header.end_ch) {
                map->path_end.x = offset % map->header.width;
                map->path_end.y = row_cnt;
            }
            map->tiles[row_cnt][offset++] = line_buf[i];
        }
        for (int i = line_len; i < map->header.width; i++) {
            map->tiles[row_cnt][offset++] = ' ';
        }
        row_cnt += 1;
        offset = 0;
    }

    for (int i = row_cnt; i < map->header.height; ++i) {
        for (int j = 0; j < map->header.width; ++j) {
            map->tiles[i][offset++] = ' ';
        }
    }

    return map_ok;
}

int map_parse(FILE *file, struct map *map) {
    int error = read_map_header(file, &map->header);
    if (error == map_ok && map->header.weight_cnt == 0) {
        error = map_missing_weights;
    }
    if (error != map_ok) {
        if (map->header.weights) {
            free(map->header.weights);
            map->header.weights = NULL;
            map->header.weight_cnt = 0;
        }
        return error;
    }

    map->tiles = calloc(map->header.height, sizeof(char *));
    for (int y = 0; y < map->header.height; ++y) {
        map->tiles[y] = calloc(map->header.width, sizeof(char));
    }

    error = read_map_body(file, map);
    if (error != map_ok) {
        free(map->header.weights);
        map->header.weights = NULL;
        map->header.weight_cnt = 0;

        for (int y = 0; y < map->header.height; ++y) {
            free(map->tiles[y]);
        }
        free(map->tiles);
        map->tiles = NULL;
        return error;
    }

    return map_ok;
}

void map_free(struct map *map) {
    free(map->header.weights);
    map->header.weights = NULL;
    map->header.weight_cnt = 0;

    for (int y = 0; y < map->header.height; ++y) {
        free(map->tiles[y]);
    }

    free(map->tiles);
    map->tiles = NULL;
}

int map_cost(struct map *map, struct point point) {
    if (point.x >= map->header.width) {
        return -1;
    }
    if (point.y >= map->header.height) {
        return -1;
    }

    char ch = map->tiles[point.y][point.x];

    for (size_t i = 0; i < map->header.weight_cnt; ++i) {
        if (map->header.weights[i].ch == ch) {
            return map->header.weights[i].weight;
        }
    }
    return 1;
}

void map_print(struct map *map) {
    print_header(&map->header);
    for (int y = 0; y < map->header.height; ++y) {
        for (int x = 0; x < map->header.width; ++x) {
            putchar(map->tiles[y][x]);
        }
        putchar('\n');
    }
}
