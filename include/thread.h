#ifndef H_OMNIUM_THREAD_DEFINED
#define H_OMNIUM_THREAD_DEFINED

/* Thread libraries */
#include <pthread.h>
#include <semaphore.h>

/* Symbols */
typedef void *(pthread_action_t)(void *);
typedef void *pthread_arg_t;
typedef pthread_t *pthread_array_t;

/* Functions */
pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args);

#endif
