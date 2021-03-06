#ifndef H_OMNIUM_SPEEDWAY_DEFINED
#define H_OMNIUM_SPEEDWAY_DEFINED

/*---------------------------------------------------------------------------*/
/*                               SPEEDWAY                                    */
/*---------------------------------------------------------------------------*/

/* Types */
typedef struct _speedway_t *speedway_t;

/* Functios */
speedway_t speedway_create(unsigned int distance, unsigned int max_cyclists);

void speedway_destroy(speedway_t speedway);

int speedway_insert_cyclist(speedway_t speedway,
                            unsigned int cyclist,
                            unsigned int pos);

int speedway_remove_cyclist(speedway_t speedway,
                            unsigned int cyclist,
                            unsigned int pos);

int speedway_move_cyclist(speedway_t speedway,
                          unsigned int cyclist,
                          unsigned int old_pos,
                          unsigned int new_pos);

int speedway_advance_cyclist(speedway_t speedway,
                             unsigned int cyclist,
                             unsigned int pos);

#endif
