/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Thread libraries */
#include <pthread.h>
#define NUM_THREADS 5

void *perform_work(void *argument)
{
    int passed_in_value;

    passed_in_value = *((int *) argument);
    printf("Hello World! It's me, thread with argument %d!\n",
        passed_in_value);

    return NULL;
}

int main(int argc, char **argv)
{
    int distance, n_cyclists, uniforme;

    /* threads */
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code, index;

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

    printf("d:%d n:%d uniforme:%d\n", distance, n_cyclists, uniforme);

    /* create all threads one by one */
    for (index = 0; index < NUM_THREADS; index++) {
        thread_args[index] = index;
        printf("In main: creating thread %d\n", index);
        result_code = pthread_create(
            &threads[index], NULL, perform_work,
            (void *) &thread_args[index]);
        assert(result_code == 0);
    }

    /* wait for each thread to complete */
    for (index = 0; index < NUM_THREADS; index++) {
        /* block until thread 'index' completes */
        result_code = pthread_join(threads[index], NULL);
        printf("In main: thread %d has completed\n", index);
        assert(result_code == 0);
    }

    printf("In main: All threads completed successfully\n");

    return EXIT_SUCCESS;
}
