#ifndef _UTILS_H_
#define _UTILS_H_

#include "move.h"
#include "player.h"

#include <getopt.h>
#include <stdio.h>
#include <limits.h>

#define MAX_TURNS 10000
#define MOVE_NULL \
    (struct move_t) { UINT_MAX, UINT_MAX, UINT_MAX}


/*
Returns an integer in [inf, sup]
*/
int randint(int inf, int sup);

int move_is_equal(struct move_t a, struct move_t b);

int cmpfunc_uint(const void *a, const void *b);

void print_int_array(int *array, int size);

void print_uint_array(unsigned int *array, unsigned int size);

void command_line_reader(int argc, char* argv[], const char* optstring, unsigned int* m, char* t);

int check_lib_load();

void deep_copy_queens(unsigned int **destination, unsigned int **source, unsigned int num_queens);

void free_sub_array_queens(unsigned int **queens);

void free_queens(unsigned int** queens);

unsigned int** queens_initialization(unsigned int num_queens, unsigned int m);

unsigned int *init_pos_array(unsigned int size);

void print_float_array(float* array, unsigned int size);

int is_move_null(struct move_t move);

#endif // _UTILS_H_