/* Standard libraries */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* System libraries */
#include <unistd.h>

/* Libraries */
#include "thread.h"

int distance, n_cyclists, uniforme;

pthread_barrier_t barrier;

int generate_aleatory(int min, int max) {
  return (int) (min + ((rand() * 1.0)/RAND_MAX) * max + 0.5);
}

int sort_initial_position() {
  return generate_aleatory(0, distance);
}

void d_start(int i) {
  time_t now;
  time(&now);
  printf("thread%d starting at %s", i, ctime (&now));
}

void d_end(int i) {
  time_t now;
  time(&now);
  printf ("barrier in thread%d() done at %s", i, ctime(&now));
}

void *perform_work(void *argument) {

  /* Initialize *******************************************************/
  int i = *((int *) argument);
  int position = sort_initial_position();
  d_start(i); /* TODO: remove */

  /** Process *********************************************************/
  printf("thread[%d]: initial position = %d!\n", i, position);
  sleep(i);

  pthread_barrier_wait(&barrier);

  /* End **************************************************************/
  d_end(i); /* TODO: remove */
  return NULL;
}

void simulate_race() {

  /* Initialize *******************************************************/
  pthread_array_t threads = pthread_array_create(n_cyclists, perform_work, NULL);
  pthread_barrier_init (&barrier, NULL, n_cyclists + 1);

  /** Process *********************************************************/
  pthread_barrier_wait(&barrier);
  pthread_array_join(n_cyclists, threads);

  /* End **************************************************************/
  pthread_array_destroy(threads);
  printf("In main: All threads completed successfully\n");
}

int main(int argc, char **argv) {

  /* Read arguments */
  if (argc != 4) {
      fprintf(stderr, "USAGE: %s d n [v|u]\n", argv[0]);
      return EXIT_FAILURE;
  }

  distance   = atoi(argv[1]);
  n_cyclists = atoi(argv[2]);

  if (argv[3][1] != '\0') {
      fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
      return EXIT_FAILURE;
  }

  switch (argv[3][0]) {
      case 'u': uniforme = 1; break;
      case 'v': uniforme = 0; break;
      default:
          fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
          return EXIT_FAILURE;
  }

  srand(time(NULL));

  printf("d:%d n:%d uniforme:%d\n", distance, n_cyclists, uniforme);

  simulate_race();

  return EXIT_SUCCESS;
}
