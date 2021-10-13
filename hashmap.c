#include "hashmap.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUCKETS_LEN 20

typedef struct node {
    void* key;
    void* value;
    struct node* next;
} node_t;

struct hashmap {
    node_t* buckets;
    int buckets_len;
    int len;
    void (*del_value)(void*);
    int (*equal)(void*, void*);
    uint32_t (*hash)(void*);
    void (*set_key)(node_t* n, void* key);
    void (*del_key)(void* key);
    enum key_type t;
};

static uint32_t HASH_STRING(void* value) {
    char* str = (char*)value;
    uint32_t hash = 5381;
    char c;
    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}
static uint32_t HASH_INT(void* value) {
    int num = (int)value;
    return num;
}
static int EQUAL_INT(void* a, void* b) { return (int)a == (int)b; }
static int EQUAL_STRING(void* a, void* b) {
    return strcmp((char*)a, (char*)b) == 0;
}
static void SETKEY_INT(node_t* n, void* key) { n->key = key; }
static void SETKEY_STRING(node_t* n, void* key) {
    n->key = malloc(sizeof((char*)key) + 1);
    strcpy((char*)n->key, (char*)key);
}
static void DEL_KEY_INT(void* key) {}
static void DEL_KEY_STRING(void* key) { free(key); }

struct hashmap* hashmap_init(void (*del_value)(void*), enum key_type t) {
    int i;
    hashmap_t* map = malloc(sizeof(hashmap_t));
    node_t* buckets = malloc(sizeof(node_t) * DEFAULT_BUCKETS_LEN);
    for (i = 0; i < DEFAULT_BUCKETS_LEN; i++) {
        buckets[i].next = NULL;
    }
    map->buckets = buckets;
    map->buckets_len = DEFAULT_BUCKETS_LEN;
    map->len = 0;
    map->del_value = del_value;
    map->t = t;
    if (t == Type_String) {
        map->hash = HASH_STRING;
        map->equal = EQUAL_STRING;
        map->set_key = SETKEY_STRING;
        map->del_key = DEL_KEY_STRING;
    } else {
        map->hash = HASH_INT;
        map->equal = EQUAL_INT;
        map->set_key = SETKEY_INT;
        map->del_key = DEL_KEY_INT;
    }
    return map;
}

void hashmap_destroy(struct hashmap* map) {
    int i;
    node_t* node;
    node_t* tmp;
    for (i = 0; i < map->buckets_len; i++) {
        node = map->buckets[i].next;
        while (node != NULL) {
            tmp = node;
            node = node->next;
            map->del_key(tmp->key);
            if (map->del_value) map->del_value(tmp->value);
            free(tmp);
        }
    }
    free(map->buckets);
    free(map);
}

static inline float loadfactor(hashmap_t* map) {
    return (float)map->len / (float)map->buckets_len;
}

static inline node_t* hashmap_get_bucket(hashmap_t* map, void* key) {
    uint32_t hash = map->hash(key);
    return &map->buckets[hash % map->buckets_len];
}

static void grow_and_rehash(hashmap_t* map) {
    int i;
    node_t* tmp;
    node_t* old_buckets = map->buckets;
    int old_buckets_len = map->buckets_len;
    map->buckets_len *= 2;
    node_t* buckets = malloc(sizeof(node_t) * map->buckets_len);
    for (i = 0; i < map->buckets_len; i++) {
        buckets[i].next = NULL;
    }
    map->buckets = buckets;
    for (i = 0; i < old_buckets_len; i++) {
        node_t* node = old_buckets[i].next;
        while (node != NULL) {
            tmp = node->next;
            node_t* bucket = hashmap_get_bucket(map, node->key);
            node->next = bucket->next;
            bucket->next = node;
            node = tmp;
        }
    }
    free(old_buckets);
}

void hashmap_set(hashmap_t* map, void* key, void* value) {
    if (loadfactor(map) >= 0.75) {
        grow_and_rehash(map);
    }
    node_t* bucket = hashmap_get_bucket(map, key);
    struct node* node = bucket->next;
    while (node) {
        if (map->equal(node->key, key)) {
            if (map->del_value) map->del_value(node->value);
            node->value = value;
            return;
        }
        node = node->next;
    }
    node = malloc(sizeof(node_t));
    map->set_key(node, key);
    node->value = value;
    node->next = bucket->next;
    bucket->next = node;
    map->len++;
}

void* hashmap_get(hashmap_t* map, const char* key) {
    node_t* bucket = hashmap_get_bucket(map, (void*)key);
    struct node* node = bucket->next;
    while (node) {
        if (map->equal((void*)key, node->key)) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void hashmap_del(hashmap_t* map, const char* key) {
    node_t* bucket = hashmap_get_bucket(map, (void*)key);
    node_t* node = bucket;
    node_t* tmp;
    while (node->next) {
        tmp = node->next;
        if (map->equal(tmp->key, (void*)key)) {
            if (map->del_value) map->del_value(tmp->value);
            map->del_key(tmp->key);
            node->next = tmp->next;
            free(tmp);
            return;
        }
        node = tmp;
    }
}

int hashmap_len(hashmap_t* map) { return map->len; }

int hashmap_has(hashmap_t* map, const char* key) {
    void* value = hashmap_get(map, key);
    return value != NULL;
}
