#ifndef H_OMNIUM_THREAD_DEFINED
#define H_OMNIUM_THREAD_DEFINED

/* Thread libraries */
#include <pthread.h>
#include <semaphore.h>

/* Symbols */
typedef void *(pthread_action_t)(void *);
typedef void *pthread_arg_t;

/* Semaphore array */
typedef sem_t *sem_array_t;

sem_array_t sem_array_create(int n, int pshared, unsigned int value);

void sem_array_destroy(sem_array_t semaphores);

/* Thread array */
typedef struct _pthread_array_t *pthread_array_t;

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args);

void pthread_array_destroy(pthread_array_t pthread_array);

void pthread_array_join(pthread_array_t pthread_array);

#endif
