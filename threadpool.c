#include "threadpool.h"

#include <pthread.h>
#include <stdlib.h>

#define DEFAULT_QUEUE_CAP 100

typedef struct task {
    void (*cb)(void* arg);
    void* arg;
} task_t;

struct queue {
    task_t* taskq;
    int front;
    int rear;
    int len;
    int cap;
};

void queue_init(struct queue* q, int cap) {
    q->front = q->rear = q->len = q->cap = 0;
    q->cap = cap;
    q->taskq = malloc(sizeof(task_t) * cap);
}

task_t queue_pop(struct queue* q) {
    task_t rtn;
    rtn = q->taskq[q->front];
    q->front = (q->front + 1) % q->cap;
    q->len--;
    return rtn;
}

void queue_push(struct queue* q, struct task t) {
    q->taskq[q->rear] = t;
    q->rear = (q->rear + 1) % q->cap;
    q->len++;
}

struct threadpool {
    struct queue taskq;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    pthread_t* tids;
    int destroyed;
    int thread_cnt;
};

static void* worker(void* arg) {
    threadpool_t* pool = (threadpool_t*)arg;
    task_t t;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        while (pool->taskq.len == 0 && !pool->destroyed) {
            pthread_cond_wait(&pool->not_empty, &pool->lock);
        }
        if (pool->destroyed) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }
        t = queue_pop(&pool->taskq);
        pthread_mutex_unlock(&pool->lock);
        pthread_cond_signal(&pool->not_full);
        t.cb(t.arg);
        free(t.arg);
    }
    pthread_exit(NULL);
    return NULL;
}

threadpool_t* pool_init(int cnt) {
    threadpool_t* pool = malloc(sizeof(threadpool_t));
    pool->destroyed = 0;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);
    pool->tids = malloc(sizeof(pthread_t) * cnt);
    queue_init(&pool->taskq, DEFAULT_QUEUE_CAP);
    for (int i = 0; i < cnt; i++) {
        pthread_create(pool->tids + i, NULL, worker, (void*)pool);
    }
    pool->thread_cnt = cnt;
    return pool;
}

void pool_destory(threadpool_t* pool) {
    pool->destroyed = 1;
    pthread_cond_broadcast(&pool->not_full);
    pthread_cond_broadcast(&pool->not_empty);
    for (int i = 0; i < pool->thread_cnt; i++) {
        pthread_join(pool->tids[i], NULL);
    }
    free(pool->taskq.taskq);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->not_full);
    pthread_cond_destroy(&pool->not_empty);
    free(pool->tids);
    free(pool);
}

void pool_put(threadpool_t* pool, void (*cb)(void* arg), void* arg) {
    pthread_mutex_lock(&pool->lock);
    while (pool->taskq.cap == pool->taskq.len && !pool->destroyed) {
        pthread_cond_wait(&pool->not_full, &pool->lock);
    }
    if (pool->destroyed) {
        pthread_mutex_unlock(&pool->lock);
        return;
    }
    struct task t;
    t.arg = arg;
    t.cb = cb;
    queue_push(&pool->taskq, t);
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->not_empty);
}
