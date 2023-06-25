#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"



//备用歌词实现功能，通过map实现，现在已用单链表实现歌词功能，故先不用此方法

map_t* map_create() {
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        return NULL;
    }
    map->size = 0;
    return map;
}

void map_put(map_t* map, const char* key, const char* value) {
    
    if (map->size == MAX_MAP_SIZE) {
        return;
    }
    if (map == NULL || key == NULL || value == NULL) {
        return;
    }
    map_entry_t* entry = malloc(sizeof(map_entry_t));
    if (entry == NULL) {
        return;
    }
    strncpy(entry->key, key, MAX_KEY_LENGTH);
    strncpy(entry->value, value, MAX_VALUE_LENGTH);
    map->entries[map->size] = entry;
    map->size++;
    
}

const char* map_get(map_t* map, const char* key) {
    if (map == NULL || key == NULL) {
        return NULL;
    }
    int i;
    for (i = 0; i < map->size; i++) {
        if (strcmp(map->entries[i]->key, key) == 0) {
            return map->entries[i]->value;
        }
    }
    return NULL;
}

void map_destroy(map_t* map) {
    if (map != NULL) {
        int i;
        for (i = 0; i < map->size; i++) {
            free(map->entries[i]);
        }
        free(map);
    }
}
