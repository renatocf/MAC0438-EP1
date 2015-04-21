/* Standard libraries */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* System libraries */
#include <unistd.h>

/* Libraries */
#include "thread.h"
#include "speedway.h"

/* Constraints */
#define MAX_TURNS    5 /* TODO: remove constraint */
#define MIN_CYCLISTS 3
#define MIN_DISTANCE 249
#define CYCLISTS_PER_POSITION 4

/* Constants */
#define TRUE (1 == 1)
#define FALSE (!(TRUE))

#define RES    "\033[0m"
#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN   "\033[1;36m"
#define WHITE  "\033[1;37m"

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                              SHARED VARIABLES
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

int g_distance, g_num_cyclists, g_uniform;

pthread_barrier_t barrier;

unsigned int g_turn = 0;

speedway_t g_speedway;

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                             AUXILIAR FUNCTIONS
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

int generate_aleatory(int min, int max) {
  return (int) (min + ((rand() * 1.0)/RAND_MAX) * max + 0.5);
}

int sort_initial_position() {
  return generate_aleatory(0, g_num_cyclists/CYCLISTS_PER_POSITION-1);
}

void d_start(int i) {
  time_t now;
  time(&now);
  printf(GREEN "thread[%d]: barrier:" RES " starting at %s", i, ctime (&now));
}

void d_end(int i) {
  time_t now;
  time(&now);
  printf(GREEN "thread[%d]: barrier:" RES " done at %s", i, ctime(&now));
}

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                                 SIMULATION
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

void *perform_work(void *argument) {

  /** Initialize **************************************************************/
  int id = *((int *) argument);
  int position = -1;
  int half = 0;
  int place = -1;

  do {
    position = sort_initial_position();
  } while ((place = speedway_insert_cyclist(g_speedway, id, position)) == -1);

  d_start(id); /* TODO: remove */

  /** Process *****************************************************************/
  printf("thread[%d]: initial position = [%d,%d]!\n", id, position, place);

  /* Start run! */
  pthread_barrier_wait(&barrier);

  while (TRUE) {
    /* Simulator processment */
    pthread_barrier_wait(&barrier);

    /* Exit conditions */
    if (g_turn == MAX_TURNS) break;

    /* Cyclist processment */
    printf("thread[%d]: old position = %d!\n", id, position);
    position = speedway_advance_cyclist(g_speedway, id, position);
    printf("thread[%d]: new position = %d!\n", id, position);
    pthread_barrier_wait(&barrier);
  }

  /** End *********************************************************************/
  d_end(id); /* TODO: remove */
  return NULL;
}

void simulate_race() {

  /** Variables ***************************************************************/
  pthread_array_t threads;

  /** Initialize **************************************************************/

  /* Speedway */
  g_speedway = speedway_create(g_distance, CYCLISTS_PER_POSITION);

  pthread_barrier_init (&barrier, NULL, g_num_cyclists + 1);

  /* Threads and barriers */
  threads  = pthread_array_create(g_num_cyclists, perform_work, NULL);

  /** Process *****************************************************************/

  /* Start run! */
  printf(YELLOW "race control:" BLUE " starting race!" RES "\n");
  pthread_barrier_wait(&barrier);

  while (TRUE) {
    /* Simulator processment */
    g_turn++;
    printf(YELLOW "race control:" RES " turn %d\n", g_turn);
    pthread_barrier_wait(&barrier);

    /* Exit conditions */
    if (g_turn == MAX_TURNS) {
      printf(YELLOW "race control:" BLUE " end race!" RES "\n");
      break;
    }

    /* Cyclist processment */
    pthread_barrier_wait(&barrier);
  }

  pthread_array_join(threads);

  /** End *********************************************************************/

  /* Threads and barriers */
  pthread_array_destroy(threads);

  /* Speedway */
  speedway_destroy(g_speedway);

  printf("In main: All threads completed successfully\n");
}

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                                    MAIN
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

int main(int argc, char **argv) {

  time_t now;

  /* Read arguments */
  if (argc != 4) {
      fprintf(stderr, "USAGE: %s d n [v|u]\n", argv[0]);
      return EXIT_FAILURE;
  }

  g_distance     = atoi(argv[1]);
  g_num_cyclists = atoi(argv[2]);

  if (argv[3][1] != '\0') {
      fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
      return EXIT_FAILURE;
  }

  switch (argv[3][0]) {
      case 'u': g_uniform = 1; break;
      case 'v': g_uniform = 0; break;
      default:
          fprintf(stderr, "3rd argument must be 'u' or 'v'\n");
          return EXIT_FAILURE;
  }

  time(&now);
  srand(now);

  if (g_num_cyclists <= MIN_CYCLISTS) {
      fprintf(stderr, "d (distance) must be > %d\n", MIN_CYCLISTS);
      return EXIT_FAILURE;
  }

  if (g_distance <= MIN_DISTANCE) {
      fprintf(stderr, "n (number of cyclists) must be > %d\n", MIN_DISTANCE);
      return EXIT_FAILURE;
  }

  printf("d:%d n:%d uniform:%d\n", g_distance, g_num_cyclists, g_uniform);

  simulate_race();

  printf(YELLOW "Using seed: %ld" RES "\n", (long) now);

  return EXIT_SUCCESS;
}
