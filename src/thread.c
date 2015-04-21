/* Standard libraries */
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                              SEMAPHORE ARRAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

sem_array_t sem_array_create(int n, int pshared, unsigned int value) {
  int i = 0, result_code = -1;
  sem_t *sems = malloc(n * sizeof(*sems));

  for (i = 0; i < n; i++) {
    result_code = sem_init(&sems[i], pshared, value);
    assert(result_code == 0);
  }
  return sems;
}

void sem_array_destroy(sem_array_t sems) {
  free(sems);
}

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                               THREAD ARRAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

static int *pthread_array_n; /* TODO: delete debug array */

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args) {
  int i = 0, result_code = -1;
  pthread_t *threads = malloc(n * sizeof(*threads));

 /* TODO: delete debug array */
  pthread_array_n = malloc(n * sizeof(*pthread_array_n));

  for (i = 0; i < n; i++) {
    pthread_array_n[i] = i;
    result_code = pthread_create(
      /* TODO: delete resource */
      &threads[i], NULL, action, (void *) &pthread_array_n[i]);
    assert(result_code == 0);
  }

  return threads;
}

void pthread_array_destroy(pthread_array_t threads) {
  free(pthread_array_n);
  free(threads);
}

void pthread_array_join(int n, pthread_array_t threads) {

  int i = 0, result_code = -1;

  for (i = 0; i < n; i++) {
    result_code = pthread_join(threads[i], NULL);
    assert(result_code == 0);
  }
}
