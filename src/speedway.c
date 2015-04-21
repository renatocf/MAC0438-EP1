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

/*---------------------------------------------------------------------------*/
/*                              DEFINITIONS                                  */
/*---------------------------------------------------------------------------*/

struct _speedway_t {
  int **positions;
  sem_array_t mutexes;
  unsigned int distance;
  unsigned int max_cyclists;
};

static int speedway_insert_cyclist_impl(speedway_t speedway,
                                        unsigned int cyclist,
                                        unsigned int pos);
static int speedway_remove_cyclist_impl(speedway_t speedway,
                                        unsigned int cyclist,
                                        unsigned int pos);

/*---------------------------------------------------------------------------*/
/*                            PUBLIC FUNCTIONS                               */
/*---------------------------------------------------------------------------*/

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

int speedway_insert_cyclist(speedway_t speedway,
                            unsigned int cyclist,
                            unsigned int pos) {
  int result_code = -1;
  sem_wait(sem_array_get(speedway->mutexes, pos));
  result_code = speedway_insert_cyclist_impl(speedway, cyclist, pos);
  sem_post(sem_array_get(speedway->mutexes, pos));
  return result_code;
}

int speedway_remove_cyclist(speedway_t speedway,
                            unsigned int cyclist,
                            unsigned int pos) {
  int result_code = -1;
  sem_wait(sem_array_get(speedway->mutexes, pos));
  result_code = speedway_remove_cyclist_impl(speedway, cyclist, pos);
  sem_post(sem_array_get(speedway->mutexes, pos));
  return result_code;
}

int speedway_move_cyclist(speedway_t speedway,
                          unsigned int cyclist,
                          unsigned int old_pos,
                          unsigned int new_pos) {
  int result_code = -1;
  sem_wait(sem_array_get(speedway->mutexes, old_pos));
  sem_wait(sem_array_get(speedway->mutexes, new_pos));

  result_code = speedway_insert_cyclist_impl(speedway, cyclist, new_pos);
  if (result_code != -1)
    result_code = speedway_remove_cyclist_impl(speedway, cyclist, old_pos);

  sem_wait(sem_array_get(speedway->mutexes, new_pos));
  sem_post(sem_array_get(speedway->mutexes, old_pos));
  return result_code;
}

/*---------------------------------------------------------------------------*/
/*                            PRIVATE FUNCTIONS                              */
/*---------------------------------------------------------------------------*/

static int speedway_insert_cyclist_impl(speedway_t speedway,
                                        unsigned int cyclist,
                                        unsigned int pos) {
  unsigned int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < speedway->max_cyclists; i++) {
    if (speedway->positions[pos][i] == -1) {
      speedway->positions[pos][i] = cyclist;
      return (int) i;
    }
  }
  return -1;
}

static int speedway_remove_cyclist_impl(speedway_t speedway,
                                        unsigned int cyclist,
                                        unsigned int pos) {
  unsigned int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < speedway->max_cyclists; i++) {
    if (speedway->positions[pos][i] == cyclist)
      return (int) i;
  }
  return -1;
}
