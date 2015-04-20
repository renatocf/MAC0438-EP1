/* Standard libraries */
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args) {

  pthread_t *threads = malloc(n * sizeof(*threads));
  int i = 0, result_code;

  for (i = 0; i < n; i++) {
    result_code = pthread_create(
      &threads[i], NULL, action, (void *) args);
    assert(result_code == 0);
  }

  return threads;
}
