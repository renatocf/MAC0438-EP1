/* Standard libraries */
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

/* Constants */
#define TRUE (1 == 1)
#define FALSE !(TRUE)

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                              SEMAPHORE ARRAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

struct _sem_array_t {
  sem_t *sems;
  unsigned int size;
};

sem_array_t sem_array_create(
    unsigned int n, int pshared, unsigned int value) {

  unsigned int i = 0;
  int result_code = -1;
  sem_array_t sem_array = NULL;

  sem_array       = malloc(sizeof(*sem_array));
  sem_array->sems = malloc(n * sizeof(*(sem_array->sems)));
  sem_array->size = n;

  for (i = 0; i < n; i++) {
    result_code = sem_init(&sem_array->sems[i], pshared, value);
    assert(result_code == 0);
  }
  return sem_array;
}

void sem_array_destroy(sem_array_t sem_array) {
  /* Pre-conditions */
  assert(sem_array != NULL);

  free(sem_array->sems);
  free(sem_array);
}

sem_t *sem_array_get(sem_array_t sem_array, unsigned int i) {
  /* Pre-conditions */
  assert(sem_array != NULL);
  assert(i < sem_array->size);

  return &(sem_array->sems[i]);
}

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                               THREAD ARRAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

struct _pthread_array_t {
  pthread_t *threads;
  unsigned int size;
  unsigned int *ids;
  unsigned int *joined;
};

pthread_array_t pthread_array_create(
    unsigned int n, pthread_action_t action, pthread_arg_t args) {

  unsigned int i = 0;
  int result_code = -1;
  pthread_array_t pthread_array = NULL;

  pthread_array          = malloc(sizeof(*pthread_array));
  pthread_array->threads = malloc(n * sizeof(*(pthread_array->threads)));
  pthread_array->ids     = malloc(n * sizeof(*(pthread_array->ids)));
  pthread_array->joined  = malloc(n * sizeof(*(pthread_array->joined)));
  pthread_array->size    = n;

  for (i = 0; i < pthread_array->size; i++) {
    pthread_array->ids[i] = i;
    pthread_array->joined[i] = FALSE;
    result_code = pthread_create(
      &(pthread_array->threads[i]), NULL, action,
      (void *) &(pthread_array->ids[i]));
    assert(result_code == 0);
  }

  return pthread_array;
}

int pthread_array_remove(pthread_array_t pthread_array,
                         unsigned int i) {
  int result_code = 0;
  if (!pthread_array->joined[i]) {
    result_code = pthread_join(pthread_array->threads[i], NULL);
    assert(result_code == 0);
    pthread_array->joined[i] = TRUE;
    pthread_array->size--;
  }
  return result_code;
}

void pthread_array_destroy(pthread_array_t pthread_array) {
  /* Pre-conditions */
  assert(pthread_array != NULL);

  free(pthread_array->ids);
  free(pthread_array->threads);
  free(pthread_array);
}

void pthread_array_join(pthread_array_t pthread_array) {

  unsigned int i = 0;
  for (i = 0; i < pthread_array->size; i++)
    pthread_array_remove(pthread_array, i);
}

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                                  BARRIER
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

int pthread_barrier_reset(pthread_barrier_t *barrier,
                          pthread_barrierattr_t *attr,
                          unsigned int count) {
  int result_code = -1;

  result_code = pthread_barrier_destroy(barrier);
  assert(result_code == 0);

  pthread_barrier_init(barrier, attr, count);
  assert(result_code == 0);

  return result_code;
}
