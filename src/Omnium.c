/* Standard libraries */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Libraries */
#include "thread.h"

int distance, n_cyclists, uniforme;

int generate_aleatory(int min, int max) {
  return (int) (min + ((rand() * 1.0)/RAND_MAX) * max + 0.5);
}

int sort_initial_position() {
  return generate_aleatory(0, distance);
}

void *perform_work(void *argument) {

  /* while (TRUE) { */
    int position = sort_initial_position();

    printf("Hello World! It's me, thread with initial position %d!\n",
        position);
  /* } */

  return NULL;
}

void simulate_race() {

    pthread_array_t threads;

    printf("d:%d n:%d uniforme:%d\n", distance, n_cyclists, uniforme);

    threads = pthread_array_create(n_cyclists, perform_work, NULL);

    pthread_array_join(n_cyclists, threads);

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

    simulate_race();

    return EXIT_SUCCESS;
}
