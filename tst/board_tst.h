#ifndef _BOARD_TST_H_
#define _BOARD_TST_H_

#include "../src/graph.h"

int alltests_board();

int* test_list_attainable_pos();
int* test_compute_linked_pos();
int* test_possible_moves();
int* test_compute_hlinked_possible_moves();
int* test_linked_heuristic();
int* test_compute_best_move();



#endif // _BOARD_TST_H_