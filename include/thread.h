#ifndef H_OMNIUM_THREAD_DEFINED
#define H_OMNIUM_THREAD_DEFINED

/* Thread libraries */
#include <pthread.h>
#include <semaphore.h>

/* Symbols */
typedef void *(pthread_action_t)(void *);
typedef void *pthread_arg_t;

/*---------------------------------------------------------------------------*/
/*                            SEMAPHORE ARRAY                                */
/*---------------------------------------------------------------------------*/

/* Types */
typedef struct _sem_array_t *sem_array_t;

/* Functios */
sem_array_t sem_array_create(
  unsigned int n, int pshared, unsigned int value);

void sem_array_destroy(sem_array_t sem_array);

sem_t *sem_array_get(sem_array_t sem_array, unsigned int i);

/*---------------------------------------------------------------------------*/
/*                             THREAD ARRAY                                  */
/*---------------------------------------------------------------------------*/

/* Types */
typedef struct _pthread_array_t *pthread_array_t;

/* Functios */
pthread_array_t pthread_array_create(
  unsigned int n, pthread_action_t action, pthread_arg_t args);

void pthread_array_destroy(pthread_array_t pthread_array);

void pthread_array_join(pthread_array_t pthread_array);

int pthread_array_remove(pthread_array_t pthread_array,
                         unsigned int i);

int pthread_array_is_joinable(pthread_array_t pthread_array, unsigned int i);

/*---------------------------------------------------------------------------*/
/*                                BARRIER                                    */
/*---------------------------------------------------------------------------*/

int pthread_barrier_reset(pthread_barrier_t *barrier,
                          pthread_barrierattr_t *attr,
                          unsigned int count);

/*---------------------------------------------------------------------------*/
/*                               SEMAPHORES                                  */
/*---------------------------------------------------------------------------*/

int sem_reset(sem_t *sem, int pshared, unsigned int value);

int sem_mult_wait(sem_t *sem, unsigned int n);

int sem_mult_post(sem_t *sem, unsigned int n);

#endif
