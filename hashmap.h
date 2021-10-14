#ifndef __RPCH_HASHMAP_H_
#define __RPCH_HASHMAP_H_

typedef struct hashmap hashmap_t;

enum key_type { Type_String, Type_Int };

struct hashmap* hashmap_init(void (*del_value)(void*), enum key_type);
void hashmap_destroy(struct hashmap* map);
void hashmap_set(hashmap_t* map, void* key, void* value);
void* hashmap_get(hashmap_t* map, void* key);
void hashmap_del(hashmap_t* map, void* key);
int hashmap_len(hashmap_t* map);
int hashmap_has(hashmap_t* map, void* key);

#endif
