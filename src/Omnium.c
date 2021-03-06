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

unsigned g_removed_id = 0;
sem_t g_deletion;

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                             AUXILIAR FUNCTIONS
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

int generate_aleatory(int min, int max) {
  return (int) (min + ((rand() * 1.0)/RAND_MAX) * (max-min+1) + 0.5);
}

int sort_initial_position() {
  return generate_aleatory(0, g_num_cyclists/CYCLISTS_PER_POSITION-1);
}

int sort_broken_cyclist(pthread_array_t threads) {
  int random = generate_aleatory(1, 100);
  if (random == 1) {
    do {
      random = generate_aleatory(0, g_num_cyclists);
    } while (!pthread_array_is_joinable(threads, random));
    return random;
  }
  return -1;
}

int sort_speedy() {
  return (generate_aleatory(0, 1) == 1) ? 50 : 25;
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
  int speedy = (g_uniform) ? 50 : 25;
  int half = TRUE;

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

      /* Exit conditions */
      if (g_break) break;

      /* Cyclist processment */
      if (speedy == 50 || (speedy == 25 && half == TRUE)) {
        position = speedway_advance_cyclist(g_speedway, id, position);
        if (position == 0 && g_step != 0) new_turn(id);
        half = FALSE;
      } else {
        half = TRUE;
      }

      if (position == 0 && g_step != 0)
        speedy = sort_speedy();

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
  unsigned int turn = 0;
  unsigned int can_break = FALSE;
  unsigned int cyclists_remaining = g_num_cyclists;

  /** Initialize **************************************************************/

  /* Speedway */
  g_speedway = speedway_create(g_distance, CYCLISTS_PER_POSITION);

  g_missing = cyclists_remaining;
  sem_init(&g_missing_mutex, 0, 1);

  pthread_barrier_init (&g_barrier, NULL, cyclists_remaining + 1);

  /* Threads and barriers */
  threads = pthread_array_create(g_num_cyclists, perform_work, NULL);

  /** Process *****************************************************************/

  /* Start run! */
  printf(YELLOW "race control:" BLUE " starting race!" RES "\n");
  pthread_barrier_wait(&g_barrier);

  printf("\n=============================================================\n\n");

  do {
    int to_remove = FALSE;

    g_break = FALSE;

    while (TRUE) {
      /* Simulator processment */

      /* special case: cyclist breaks */
      if (can_break) {
        if (turn % 4 == 0) {
          int cyclist = sort_broken_cyclist(threads);
          if (cyclist != -1) {
            cyclists_remaining--;
            g_removed_id = cyclist;
            g_break = TRUE;
            to_remove = TRUE;
          }
        }
        can_break = FALSE;
      }

      /* special case: turn finishes */
      if (g_missing == 0 && g_step != 0) {
        turn++;

        race_statistics(cyclists_remaining, turn);

        if (cyclists_remaining == 3) {
          g_end = TRUE;
          g_break = TRUE;
        } else if (turn % 2 == 0) {
          cyclists_remaining--;
          g_missing = cyclists_remaining;

          g_removed_id = g_last[2];
          sem_reset(&g_deletion, 0, 0);

          to_remove = TRUE;
          g_break = TRUE;
        }

        if (cyclists_remaining != 3)
          can_break = TRUE;
      }

      g_step++;
      /* printf(YELLOW "race control:" RES " step %d\n", g_step); */

      pthread_barrier_wait(&g_barrier);

      /* Exit conditions */
      if (g_break) break;

      /* Cyclist processment */
      pthread_barrier_wait(&g_barrier);
    }

    if (to_remove) {
      pthread_array_remove(threads, g_removed_id);
      pthread_barrier_reset(&g_barrier, NULL, cyclists_remaining + 1);
      sem_mult_post(&g_deletion, cyclists_remaining);
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
