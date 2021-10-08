#include "hashmap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUCKETS_LEN 20

typedef struct node {
    char* key;
    void* value;
    struct node* next;
} node_t;

struct hashmap {
    node_t* buckets;
    int buckets_len;
    int len;
    void (*del_value)(void*);
};

struct hashmap* hashmap_init(void (*del_value)(void*)) {
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
            free(tmp->key);
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

static uint32_t HASH(const char* str) {
    uint32_t hash = 5381;
    char c;
    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

static inline node_t* hashmap_get_bucket(hashmap_t* map, const char* key) {
    uint32_t hash = HASH(key);
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

void show(hashmap_t* map) {
    printf("len=%d,buckets_len=%d\n", map->len, map->buckets_len);
    for (int i = 0; i < map->buckets_len; i++) {
        node_t* node = map->buckets[i].next;
        while (node != NULL) {
            printf("[%s:%d] ", node->key, *(int*)node->value);
            node = node->next;
        }
        printf("NULL\n");
    }
}

void hashmap_set(hashmap_t* map, const char* key, void* value) {
    if (loadfactor(map) >= 0.75) {
        grow_and_rehash(map);
    }
    node_t* bucket = hashmap_get_bucket(map, key);
    struct node* node = bucket->next;
    while (node) {
        if (strcmp(key, node->key) == 0) {
            if (map->del_value) map->del_value(node->value);
            node->value = value;
            return;
        }
        node = node->next;
    }
    node = malloc(sizeof(node_t));
    node->key = malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = value;
    node->next = bucket->next;
    bucket->next = node;
    map->len++;
}

void* hashmap_get(hashmap_t* map, const char* key) {
    node_t* bucket = hashmap_get_bucket(map, key);
    struct node* node = bucket->next;
    while (node) {
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void hashmap_del(hashmap_t* map, const char* key) {
    node_t* bucket = hashmap_get_bucket(map, key);
    node_t* node = bucket;
    node_t* tmp;
    while (node->next) {
        tmp = node->next;
        if (strcmp(tmp->key, key) == 0) {
            free(tmp->key);
            if (map->del_value) map->del_value(tmp->value);
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
