/* Standard libraries */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* System libraries */
#include <unistd.h>

/* Libraries */
#include "thread.h"

/* Constraints */
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

int **g_speedway;

sem_array_t g_speedway_mutexes;

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

int **speedway_create(int distance, int max_cyclists) {
  int i = 0, j = 0;
  int **speedway = NULL;

  /* Pre-conditions */
  assert(distance > 0);
  assert(max_cyclists > 0);

  speedway = (int **) malloc (distance * sizeof(*speedway));
  for (i = 0; i < distance; i++) {
      speedway[i] = malloc(max_cyclists * sizeof(*speedway[i]));
      for (j = 0; j < max_cyclists; j++)
          speedway[i][j] = -1;
  }

  /* Post-conditions */
  assert(speedway != NULL);
  return speedway;
}

void speedway_destroy(int **speedway) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < g_distance; i++) free(speedway[i]);
  free(speedway);
}

int speedway_insert_cyclist(int **speedway, int cyclist, int position) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);
  assert(cyclist >= 0 && cyclist <= g_num_cyclists);
  assert(position >= 0 && position <= g_distance);

  for (i = 0; i < CYCLISTS_PER_POSITION; i++) {
    if (speedway[position][i] == -1) {
      speedway[position][i] = cyclist;
      return i;
    }
  }
  return -1;
}

int speedway_remove_cyclist(int **speedway, int cyclist, int position) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);
  assert(cyclist >= 0 && cyclist <= g_num_cyclists);
  assert(position >= 0 && position <= g_distance);

  for (i = 0; i < CYCLISTS_PER_POSITION; i++) {
    if (speedway[position][i] == cyclist)
      return i;
  }
  return -1;
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
    if (g_turn == 5) break;

    /* Cyclist processment */
    printf("thread[%d]: position = %d!\n", id, position);
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
  g_speedway_mutexes = sem_array_create(g_distance, 0, TRUE);

  /* Threads and barriers */
  threads  = pthread_array_create(g_num_cyclists, perform_work, NULL);

  /** Process *****************************************************************/
  pthread_barrier_init (&barrier, NULL, g_num_cyclists + 1);

  /* Start run! */
  printf(YELLOW "race control:" BLUE " starting race!" RES "\n");
  pthread_barrier_wait(&barrier);

  while (TRUE) {
    /* Simulator processment */
    g_turn++;
    printf(YELLOW "race control:" RES " turn %d\n", g_turn);
    pthread_barrier_wait(&barrier);

    /* Exit conditions */
    if (g_turn == 5) {
      printf(YELLOW "race control:" BLUE " end race!" RES "\n");
      break;
    }

    /* Cyclist processment */
    pthread_barrier_wait(&barrier);
  }

  pthread_array_join(g_num_cyclists, threads);

  /** End *********************************************************************/

  /* Threads and barriers */
  pthread_array_destroy(threads);

  /* Speedway */
  sem_array_destroy(g_speedway_mutexes);
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

  srand(time(NULL));

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

  return EXIT_SUCCESS;
}
