#ifndef H_OMNIUM_SPEEDWAY_DEFINED
#define H_OMNIUM_SPEEDWAY_DEFINED

int **speedway_create(int distance, int max_cyclists);

void speedway_destroy(int **speedway, int distance);

int speedway_insert_cyclist(int **speedway, int cyclist, int position);

int speedway_remove_cyclist(int **speedway, int cyclist, int position);

#endif
