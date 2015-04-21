/* Standard libraries */
#include <assert.h>
#include <stdlib.h>

/* Libraries */
#include "speedway.h"

/*
////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------
                                 SPEEDWAY
-------------------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

struct _speedway_t {
  int **positions;
  unsigned int distance;
  unsigned int max_cyclists;
};

speedway_t speedway_create(unsigned int distance, unsigned int max_cyclists) {
  unsigned int i = 0, j = 0;
  speedway_t speedway = NULL;

  speedway               = malloc(sizeof(*speedway));
  speedway->distance     = distance;
  speedway->max_cyclists = max_cyclists;

  speedway->positions = malloc(distance * sizeof(*(speedway->positions)));
  for (i = 0; i < distance; i++) {
      speedway->positions[i]
        = malloc(max_cyclists * sizeof(*(speedway->positions[i])));
      for (j = 0; j < max_cyclists; j++)
        speedway->positions[i][j] = -1;
  }

  /* Post-conditions */
  assert(speedway != NULL);

  return speedway;
}

void speedway_destroy(speedway_t speedway) {
  unsigned int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < speedway->distance; i++) free(speedway->positions[i]);
  free(speedway->positions);
  free(speedway);
}

int speedway_insert_cyclist(speedway_t speedway, int cyclist, int place) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; (unsigned int) i < speedway->max_cyclists; i++) {
    if (speedway->positions[place][i] == -1) {
      speedway->positions[place][i] = cyclist;
      return i;
    }
  }
  return -1;
}

int speedway_remove_cyclist(speedway_t speedway, int cyclist, int place) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; (unsigned int) i < speedway->max_cyclists; i++) {
    if (speedway->positions[place][i] == cyclist)
      return i;
  }
  return -1;
}
