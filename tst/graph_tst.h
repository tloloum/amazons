#ifndef _GRAPH_TST_H_
#define _GRAPH_TST_H_

#include "../src/graph.h"
#include "../src/board.h"

/*
This function is called in order to execute all tests
*/
int alltests_graph();

int* test_availaible_square_directions();

int* test_get_direction_between_edges();

int* test_get_neighbor_in_direction();

int* test_board_set_new_piece();

int* test_board_get_piece();

int test_get_neighbor();

int* test_get_neighbors();

int test_init_graph();


#endif // _GRAPH_TST_H_