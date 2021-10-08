#ifndef __RPCH_THREADPOOL_H_
#define __RPCH_THREADPOOL_H_

typedef struct threadpool threadpool_t;

threadpool_t* pool_init(int cnt);
void pool_destory(threadpool_t*);
void pool_put(threadpool_t*, void (*)(void* arg), void* arg);

#endif
