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

struct _pthread_array_t {
  pthread_t *threads;
  unsigned int *ids;
  unsigned int size;
};

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args) {

  unsigned int i = 0;
  int result_code = -1;
  pthread_array_t pthread_array;
 
  pthread_array          = malloc(sizeof(*pthread_array));
  pthread_array->threads = malloc(n * sizeof(*(pthread_array->threads)));
  pthread_array->ids     = malloc(n * sizeof(*(pthread_array->ids)));
  pthread_array->size    = n;

  for (i = 0; i < pthread_array->size; i++) {
    pthread_array->ids[i] = i;
    result_code = pthread_create(
      &(pthread_array->threads[i]), NULL, action,
      (void *) &(pthread_array->ids[i]));
    assert(result_code == 0);
  }

  return pthread_array;
}

void pthread_array_destroy(pthread_array_t pthread_array) {
  free(pthread_array->ids);
  free(pthread_array->threads);
  free(pthread_array);
}

void pthread_array_join(pthread_array_t pthread_array) {

  unsigned int i = 0;
  int result_code = -1;

  for (i = 0; i < pthread_array->size; i++) {
    result_code = pthread_join(pthread_array->threads[i], NULL);
    assert(result_code == 0);
  }
}
