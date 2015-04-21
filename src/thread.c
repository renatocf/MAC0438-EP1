/* Standard libraries */
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

static int *ns; /* TODO: delete debug array */

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args) {
  int i = 0, result_code = -1;
  pthread_t *threads = malloc(n * sizeof(*threads));

  ns = malloc(n * sizeof(*ns)); /* TODO: delete debug array */

  for (i = 0; i < n; i++) {
    ns[i] = i;
    result_code = pthread_create(
      &threads[i], NULL, action, (void *) &ns[i]); /* TODO: delete resource */
    assert(result_code == 0);
  }

  return threads;
}

void pthread_array_destroy(pthread_array_t threads) {
  free(ns);
  free(threads);
}

void pthread_array_join(int n, pthread_array_t threads) {

  int i = 0, result_code = -1;

  for (i = 0; i < n; i++) {
    result_code = pthread_join(threads[i], NULL);
    assert(result_code == 0);
  }
}
