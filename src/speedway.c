/* Standard libraries */
#include <assert.h>
#include <stdlib.h>

/* Libraries */
#include "speedway.h"

#define CYCLISTS_PER_POSITION 4

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

void speedway_destroy(int **speedway, int distance) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

  for (i = 0; i < distance; i++) free(speedway[i]);
  free(speedway);
}

int speedway_insert_cyclist(int **speedway, int cyclist, int position) {
  int i = 0;

  /* Pre-conditions */
  assert(speedway != NULL);

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

  for (i = 0; i < CYCLISTS_PER_POSITION; i++) {
    if (speedway[position][i] == cyclist)
      return i;
  }
  return -1;
}
