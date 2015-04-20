/* Standard libraries */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Thread libraries */
#include <pthread.h>
#include <semaphore.h>

int distance, n_cyclists, uniforme;

int generate_aleatory(int min, int max) {
  return (int) (min + ((rand() * 1.0)/RAND_MAX) * max + 0.5);
}

int sort_initial_position() {
  return generate_aleatory(0, distance);
}

void *perform_work(void *argument) {
  /* int lap = 0; */
  int position = sort_initial_position();

  /* int passed_in_value; */
  /* passed_in_value = *((int *) argument); */

  printf("Hello World! It's me, thread with initial position %d!\n",
      position);

  return NULL;
}

void simulate_race() {
    /* threads */
    pthread_t *threads;
    int *thread_args;

    int result_code, index;

    printf("d:%d n:%d uniforme:%d\n", distance, n_cyclists, uniforme);

    /* Initialize threads */
    threads = (pthread_t *) malloc(n_cyclists * sizeof(*threads));
    thread_args = (int *) malloc(n_cyclists * sizeof(*thread_args));

    /* create all threads one by one */
    for (index = 0; index < n_cyclists; index++) {
        thread_args[index] = index;
        printf("In main: creating thread %d\n", index);
        result_code = pthread_create(
            &threads[index], NULL, perform_work,
            (void *) &thread_args[index]);
        assert(result_code == 0);
    }

    /* wait for each thread to complete */
    for (index = 0; index < n_cyclists; index++) {
        /* block until thread 'index' completes */
        result_code = pthread_join(threads[index], NULL);
        printf("In main: thread %d has completed\n", index);
        assert(result_code == 0);
    }

    free(threads);
    free(thread_args);

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
