#ifndef __MAP_H_
#define __MAP_H_ 

#define MAX_KEY_LENGTH 50
#define MAX_VALUE_LENGTH 50
#define MAX_MAP_SIZE 100

//备用歌词实现功能，通过map实现，现在已用单链表实现歌词功能，故先不用此方法


typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} map_entry_t;

typedef struct {
    map_entry_t* entries[MAX_MAP_SIZE];
    int size;
} map_t;

map_t* map_create();
void map_put(map_t* map, const char* key, const char* value);
const char* map_get(map_t* map, const char* key);
void map_destroy(map_t* map);

#endif