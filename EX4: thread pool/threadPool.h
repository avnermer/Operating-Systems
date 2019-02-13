//Avner Merhavi 203090436
// 29/05/18.

#ifndef UNTITLED9_THREADPOOL_H
#define UNTITLED9_THREADPOOL_H

#include <pthread.h>
#include "osqueue.h"

typedef enum status {FULLY_ACTIVE, WAITING_QUEUE, NOT_WAITING_QUEUE} Status;
typedef struct thread_pool
{
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    OSQueue *queue;
    int numOfThreads;
    Status status;

}ThreadPool;

typedef struct task
{
    void (*func)(void*);
    void* args;
}Task;

ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
