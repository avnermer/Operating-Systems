//Avner Merhavi 203090436
// 29/05/18

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threadPool.h"

#define TRUE 1
#define ERROR (-1)
#define SUCCESS 0

/**
 * handling systemCall errors.
 * @param shouldHandle int "boolean" represents does handle needed
 */
void handleErrors(int shouldHandle)
{
    if(shouldHandle)
    {
        char sysCallError[] = {"Error in system call"};

        write(STDERR_FILENO, sysCallError, sizeof(sysCallError));
        exit(EXIT_FAILURE);
    }
}
/**
 * the function executes the tasks. is sent to the threads in their creation.
 * @param args args
 * @return void
 */
static void* executeTasks(void* args)
{
    ThreadPool* threadPool = (ThreadPool*)args;
    OSQueue* queue = threadPool->queue;
    while(TRUE)
    {
        /*if not needs to fulfil all queue
         * or queue is empty, and need to fulfil till it empty
         * return
         * */
        if(threadPool->status == NOT_WAITING_QUEUE)
        {
            return NULL;
        }
        if(osIsQueueEmpty(threadPool->queue) && threadPool->status != FULLY_ACTIVE)
            return NULL;

        /*commit a CONDITIONAL WAIT for a new task, if queue is empty and needs to wait*/
        /*This is NOT a busy waiting, the loop is a just in case "spurious wakeup" will occur,
         * it normally won't loop more than 1 iteration.
         * for more info see: https://stackoverflow.com/questions/7766057
         * */

        //lock cond before

        handleErrors(pthread_mutex_lock(&threadPool->lock) != 0);
        while(osIsQueueEmpty(queue) && threadPool->status == FULLY_ACTIVE)
        {
            handleErrors(pthread_cond_wait(&threadPool->notify, &threadPool->lock) != 0);
        }
        //when a new task available , will reach here and will fulfil it

        Task* task = osDequeue(queue);
        handleErrors(pthread_mutex_unlock(&threadPool->lock) != 0);

        //in case other threads caught the tasks and emptied the queue
        if(!task)
        {
            continue;
        }
        //fulfill task
        task->func(task->args);
        free(task);
    }
}
/**
 * creating the thread pool.
 * @param numOfThreads number of threads in the pool
 * @return thread pool
 */
ThreadPool* tpCreate(int numOfThreads)
{
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    handleErrors(pool == NULL);
    pool->status = FULLY_ACTIVE;
    pool->numOfThreads = numOfThreads;
    pool->threads = (pthread_t*)malloc(sizeof(pthread_t)*numOfThreads);
    handleErrors(pool->threads == NULL);
    pool->queue =  osCreateQueue();

    /* Initialize mutex and conditional var */
    handleErrors(
            (pthread_cond_init(&(pool->notify), NULL) != 0) ||
            (pthread_mutex_init(&(pool->lock), NULL) != 0) ||
            (pool->threads == NULL) ||
            (pool->queue == NULL));

    /* start threads */
    int i;
    for(i = 0; i < numOfThreads; i++)
    {
        if(pthread_create(&(pool->threads[i]), NULL,
                          executeTasks, (void*)pool) != 0)
        {
            tpDestroy(pool, 0);
            handleErrors(TRUE);
        }
    }

    return pool;
}
/**
 * inserting task to the queue
 * @param threadPool tp
 * @param computeFunc  cf
 * @param param parameters to the task
 * @return
 */
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param)
{
    //if was destroyed or in the middle of destroy
    if(!threadPool ||threadPool->status != FULLY_ACTIVE)
    {
        return ERROR;
    }

    Task* task = (Task*)(malloc(sizeof(Task)));
    task->func = computeFunc;
    task->args = param;
    handleErrors(pthread_mutex_lock(&threadPool->lock) != 0);
    osEnqueue(threadPool->queue, task);


    //notify thread waiting for a new task

    handleErrors(pthread_cond_signal(&threadPool->notify) != 0);
    handleErrors(pthread_mutex_unlock(&threadPool->lock) != 0);
    return SUCCESS;
}
/**
 * destroying the thread pool.
 * @param threadPool
 * @param shouldWaitForTasks
 */
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks)
{
    threadPool->status = (shouldWaitForTasks == 1) ? WAITING_QUEUE : NOT_WAITING_QUEUE;
    //printf("updated status\n");

    // signal all threads if they still wait to cond
    handleErrors(pthread_cond_broadcast(&threadPool->notify) != 0);
    //wait for all threads to be finished
    int i;
    for(i = 0; i < threadPool->numOfThreads; i++)
    {
        handleErrors(pthread_join(threadPool->threads[i], NULL) != 0);
    }

    //free data (tasks) in queue nodes, then destroy queue
    OSNode* current = threadPool->queue->head;
    while(current !=NULL)
    {
        if(current->data == NULL)
            continue;
        free(current->data);
        current = current->next;
    }

    osDestroyQueue(threadPool->queue);


    //free thread pool stuff
    free(threadPool->threads);
    pthread_cond_destroy(&threadPool->notify);
    pthread_mutex_destroy(&threadPool->lock);
    free(threadPool);
}