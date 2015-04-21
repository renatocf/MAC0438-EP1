/* Standard libraries */
#include <assert.h>
#include <stdlib.h>

/* Libraries */
#include "speedway.h"
#include "thread.h"

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                                 SPEEDWAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

struct _speedway_t {
  int **positions;
  sem_array_t mutexes;
  unsigned int distance;
  unsigned int max_cyclists;
};

speedway_t speedway_create(unsigned int distance, unsigned int max_cyclists) {
  unsigned int i = 0, j = 0;
  speedway_t speedway = NULL;

  speedway               = malloc(sizeof(*speedway));
  speedway->distance     = distance;
  speedway->max_cyclists = max_cyclists;
  speedway->mutexes      = sem_array_create(distance, 0, 1);

  speedway->positions = malloc(distance * sizeof(*(speedway->positions)));
  for (i = 0; i < distance; i++) {
      sem_wait(sem_array_get(speedway->mutexes, i));

      speedway->positions[i]
        = malloc(max_cyclists * sizeof(*(speedway->positions[i])));
      for (j = 0; j < max_cyclists; j++)
        speedway->positions[i][j] = -1;

      sem_post(sem_array_get(speedway->mutexes, i));
  }

  /* Post-conditions */
  assert(speedway != NULL);

  return speedway;
}

void speedway_destroy(speedway_t speedway) {
  unsigned int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < speedway->distance; i++) {
    sem_wait(sem_array_get(speedway->mutexes, i));
    free(speedway->positions[i]);
    sem_post(sem_array_get(speedway->mutexes, i));
  }
  free(speedway->positions);
  sem_array_destroy(speedway->mutexes);
  free(speedway);
}

int speedway_insert_cyclist(speedway_t speedway, int cyclist, int pos) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  sem_wait(sem_array_get(speedway->mutexes, pos));
  for (i = 0; (unsigned int) i < speedway->max_cyclists; i++) {
    if (speedway->positions[pos][i] == -1) {
      speedway->positions[pos][i] = cyclist;
      sem_post(sem_array_get(speedway->mutexes, pos));
      return i;
    }
  }
  sem_post(sem_array_get(speedway->mutexes, pos));
  return -1;
}

int speedway_remove_cyclist(speedway_t speedway, int cyclist, int pos) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  sem_wait(sem_array_get(speedway->mutexes, pos));
  for (i = 0; (unsigned int) i < speedway->max_cyclists; i++) {
    if (speedway->positions[pos][i] == cyclist)
      sem_post(sem_array_get(speedway->mutexes, pos));
      return i;
  }
  sem_post(sem_array_get(speedway->mutexes, pos));
  return -1;
}
