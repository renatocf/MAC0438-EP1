/* Standard libraries */
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

pthread_array_t pthread_array_create(
    int n, pthread_action_t action, pthread_arg_t args) {

  int i = 0, result_code = -1;
  pthread_t *threads = malloc(n * sizeof(*threads));

  for (i = 0; i < n; i++) {
    result_code = pthread_create(
      &threads[i], NULL, action, (void *) args);
    assert(result_code == 0);
  }

  return threads;
}

void pthread_array_join(int n, pthread_array_t threads) {

  int i = 0, result_code = -1;

  for (i = 0; i < n; i++) {
    result_code = pthread_join(threads[i], NULL);
    assert(result_code == 0);
  }
}
