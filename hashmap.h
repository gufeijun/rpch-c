#ifndef __RPCH_HASHMAP_H_
#define __RPCH_HASHMAP_H_

typedef struct hashmap hashmap_t;

struct hashmap* hashmap_init(void (*del_value)(void*));
void hashmap_destroy(struct hashmap* map);
void hashmap_set(hashmap_t* map, const char* key, void* value);
void* hashmap_get(hashmap_t* map, const char* key);
void hashmap_del(hashmap_t* map, const char* key);
int hashmap_len(hashmap_t* map);
int hashmap_has(hashmap_t* map, const char* key);

#endif
