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
#define MAX_STEPS    251 /* TODO: remove constraint */
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

pthread_barrier_t g_barrier;

unsigned int g_step = 0;
unsigned int g_end = FALSE, g_break = FALSE;

speedway_t g_speedway;

unsigned int g_missing = 0;
unsigned int g_last[3] = { 0, 0, 0 };
sem_t g_missing_mutex;

unsigned int g_to_remove = FALSE, g_removed_id = 0;
sem_t g_deletion;


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

void new_turn(int cyclist) {
  sem_wait(&g_missing_mutex);
  g_missing--;
  g_last[0] = g_last[1];
  g_last[1] = g_last[2];
  g_last[2] = cyclist;
  sem_post(&g_missing_mutex);
}

void race_statistics(unsigned int cyclists_remaining, unsigned int turn) {
  printf(YELLOW "race control (turn %d): " RES, turn);
  if (cyclists_remaining != 3)
    printf("[ 3rd last: %d, 2nd last: %d, " RED "last: %d" RES " ]",
      g_last[0], g_last[1], g_last[2]);
  else
    printf("[ Cupper: %d, Silver: %d, " YELLOW "Gold: %d" RES " ]",
      g_last[2], g_last[1], g_last[0]);
  printf("\n");
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
  int position = -1, place = -1;
  int half = 0;

  do {
    position = sort_initial_position();
  } while ((place = speedway_insert_cyclist(g_speedway, id, position)) == -1);

  d_start(id); /* TODO: remove */

  /** Process *****************************************************************/
  printf("thread[%d]: initial position = [%d,%d]!\n", id, position, place);

  /* Start run! */
  pthread_barrier_wait(&g_barrier);

  do {
    while (TRUE) {
      /* Simulator processment */
      pthread_barrier_wait(&g_barrier);
      printf("thread[%d]: on barrier!\n", id);

      /* Exit conditions */
      if (g_break) break;

      /* Cyclist processment */
      /* printf("thread[%d]: old position = %d!\n", id, position); */
      position = speedway_advance_cyclist(g_speedway, id, position);
      /* printf("thread[%d]: new position = %d!\n", id, position); */

      if (position == 0 && g_step != 0) {
        new_turn(id);
      }

      pthread_barrier_wait(&g_barrier);
    }

    if (g_end) break;

    if (id == g_removed_id) break;
    else sem_wait(&g_deletion);

  } while (!g_end);

  /** End *********************************************************************/
  d_end(id); /* TODO: remove */
  return NULL;
}

void simulate_race() {

  /** Variables ***************************************************************/
  pthread_array_t threads;
  unsigned int turns = 0;
  unsigned int cyclists_remaining = g_num_cyclists;

  /** Initialize **************************************************************/

  /* Speedway */
  g_speedway = speedway_create(g_distance, CYCLISTS_PER_POSITION);

  g_missing = cyclists_remaining;
  sem_init(&g_missing_mutex, 0, 1);

  pthread_barrier_init (&g_barrier, NULL, cyclists_remaining + 1);

  /* Threads and barriers */
  threads  = pthread_array_create(g_num_cyclists, perform_work, NULL);

  /** Process *****************************************************************/

  /* Start run! */
  printf(YELLOW "race control:" BLUE " starting race!" RES "\n");
  pthread_barrier_wait(&g_barrier);

  printf("\n=============================================================\n\n");

  do {

    g_break = FALSE;
    g_to_remove = FALSE;

    while (TRUE) {
      /* Simulator processment */
      if (g_missing == 0 && g_step != 0) {
        turns++;

        race_statistics(cyclists_remaining, turns);

        if (cyclists_remaining == 3) {
          g_end = TRUE;
        } else {
          cyclists_remaining--;
          g_missing = cyclists_remaining;

          g_removed_id = g_last[2];
          g_to_remove = TRUE;

          sem_destroy(&g_deletion);
          sem_init(&g_deletion, 0, 0);
        }
        g_break = TRUE;
      }

      g_step++;
      printf(YELLOW "race control:" RES " step %d\n", g_step);

      pthread_barrier_wait(&g_barrier);

      /* Exit conditions */
      if (g_break) break;

      /* Cyclist processment */
      pthread_barrier_wait(&g_barrier);
    }

    if (g_to_remove) {
      unsigned int i = 0;
      printf(YELLOW "race control:" RES " removing %d\n", g_removed_id);
      pthread_array_remove(threads, g_removed_id);

      pthread_barrier_destroy (&g_barrier);
      pthread_barrier_init (&g_barrier, NULL, cyclists_remaining + 1);
      g_to_remove = TRUE;
      for (i = 0; i < cyclists_remaining; i++) sem_post(&g_deletion);
    }

  } while (!g_end);

  printf("\n=============================================================\n\n");

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

  printf(YELLOW "Using seed: %ld" RES "\n", (long) now);

  simulate_race();

  printf(YELLOW "Using seed: %ld" RES "\n", (long) now);

  return EXIT_SUCCESS;
}
