#ifndef _BOARD_H_
#define _BOARD_H_

#include "player.h"
#include "utils.h"

#include <gsl/gsl_matrix.h>
#include <limits.h>


/** Enum defining the possible sorts of pieces in the world */
enum piece_kind_t
{
    VOID = 0, // Default sort (i.e. nothing)
    EMPTY = 1,
    QUEEN = 2,
    ARROW = 3
};

struct piece_t
{
    enum piece_kind_t kind;
    unsigned int id_player;
};

#define PIECE_EMPTY \
    (struct piece_t) { EMPTY, UINT_MAX }
    

struct board_t
{
    struct graph_t *graph;
    // world is an array of piece_t
    struct piece_t *world;
    unsigned int **queens;
    unsigned int num_queens;
    unsigned int m;
};

void board_place_players(struct board_t *board, unsigned int **queens, unsigned int num_queens);

/*
Takes a board type.
Returns a struct board_t pointer (heap-allocated outside function).
*/

int get_m(struct board_t *board);

struct board_t *init_board(struct graph_t *graph, unsigned int **queens_array, unsigned int num_queens);

void deep_copy_board(struct board_t *dst, struct board_t *src);

struct piece_t board_get_piece(struct board_t *board, unsigned int idx);

void board_set_new_piece(struct board_t *board, unsigned int idx, enum piece_kind_t kind, unsigned int id_player);

void board_set_piece(struct board_t *board, unsigned int idx, struct piece_t new_piece);

struct board_t *init_board(struct graph_t *graph, unsigned int **queens_array, unsigned int num_queens);

void board_execute_move(struct board_t *board, struct move_t *move);

void print_board(struct board_t *board);

void free_board(struct board_t *board);

void hard_free_board(struct board_t *board);

void free_sub_array_queens(unsigned int **queens);

void free_queens(unsigned int **queens);

int is_legal_pos(struct board_t *board, unsigned int idx);

int is_attainable_pos(struct board_t *board, unsigned int src, unsigned int dst);

void list_attainable_pos(struct board_t *board, unsigned int src, unsigned int list[]);

struct board_t *create_board_and_attributes(unsigned int m, enum graph_shape shape);

unsigned int random_attainable_position_from(struct board_t *board, unsigned int src);

int *compute_reachable_positions(unsigned int nb_queens, unsigned int *queens, struct board_t *player_board);

void reachable_positions_from_queens(unsigned int nb_queens, unsigned int *queens, int *reachable, struct board_t *player_board);

void reachable_positions_from(unsigned int pos, int *reachable, struct board_t *player_board);

int *compute_reachable_vertices(struct board_t *player_board);

int *compute_linked_pos(struct board_t *board, unsigned int player_id);

float linked_heuristic(struct board_t *board, unsigned int player_id);

struct move_t *possible_moves(struct board_t *board, unsigned int player_id);

float *compute_heuristic_on_possible_moves(struct board_t *board, struct move_t *possible_moves, unsigned int player_id, float (*heuristic)(struct board_t *, unsigned int));

struct move_t compute_best_move(struct board_t *board, unsigned int player_id, float (*heuristic)(struct board_t *, unsigned int));

int game_over(struct board_t *board, int curr_player_id);

#endif // _BOARD_H_