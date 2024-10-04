#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <limits.h>

#include "board.h"

/*

    The "board.c" file contains functions that handle the game board.
    The functions perform tasks such as creating a new board with initial settings,
    setting up the pieces on the board, getting and setting the pieces on the board,
    and executing moves on the board.

*/

int get_m(struct board_t *board)
{
    unsigned int m = sqrt(board->graph->t->size1);
    return m;
}

/*
   This function takes two pointers to structs of type "board_t".
   It creates a deep copy of the source board to the destination board,
   which means that it creates new copies of all the dynamically allocated memory
   used by the source board, so that the destination board is a completely separate copy with its own memory.
*/

void deep_copy_board(struct board_t *dst, struct board_t *src)
{

    memcpy(dst, src, sizeof(struct board_t));

    dst->graph = malloc(sizeof(struct graph_t));
    deep_copy_graph(dst->graph, src->graph);

    dst->queens = malloc(NUM_PLAYERS * sizeof(*src->queens));
    deep_copy_queens(dst->queens, src->queens, src->num_queens);

    unsigned int m = src->m;

    dst->world = malloc(m * m * sizeof(*src->world));
    memcpy(dst->world, src->world, m * m * sizeof(*src->world));
}

/*
This function sets up the pieces on a board based on a given array of queens' positions.
It first fills the board with void or empty pieces, and then places the queens on the board based on their positions in the array.
*/
void set_board_pieces(struct board_t *board, unsigned int **queens_array, unsigned int num_queens)
{
    unsigned int m = board->m;
    for (unsigned int idx = 0; idx < m * m; idx++)
    {

        if (is_void(board->graph, idx))
        {
            board_set_new_piece(board, idx, VOID, UINT_MAX);
        }
        else
        {
            board_set_new_piece(board, idx, EMPTY, UINT_MAX);
        }
    }

    // Pieces are set up :
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        // placing pieces
        for (unsigned int j = 0; j < num_queens; j++)
        {
            board_set_new_piece(board, queens_array[i][j], QUEEN, i);
        }
    }
}

struct board_t *init_board(struct graph_t *graph, unsigned int **queens, unsigned int num_queens)
{
    // allocation for the board and its attributes
    struct board_t *board = malloc(sizeof(*board));

    board->graph = graph;
    unsigned m = get_m(board);
    board->m = m;
    struct piece_t *board_world = malloc(m * m * sizeof(*board_world));
    board->world = board_world;

    set_board_pieces(board, queens, num_queens);
    board->queens = queens;
    board->num_queens = num_queens;

    return board;
}

struct piece_t board_get_piece(struct board_t *board, unsigned int idx)
{
    return board->world[idx];
}

/*
CAUTION :  these two functions DO NOT modify queens array in struct board
*/
void board_set_new_piece(struct board_t *board, unsigned int idx, enum piece_kind_t kind, unsigned int id_player)
{

    struct piece_t new_piece = {.kind = kind, .id_player = id_player};
    board_set_piece(board, idx, new_piece);
}

void board_set_piece(struct board_t *board, unsigned int idx, struct piece_t new_piece)
{
    board->world[idx] = new_piece;
}

void board_execute_move(struct board_t *board, struct move_t *move)
{
    if (is_move_null(*move))
    {
        assert("Tried to update board with MOVE NULL");
        return;
    }

    struct piece_t tmp;
    tmp = board_get_piece(board, move->queen_src);
    board_set_new_piece(board, move->queen_src, EMPTY, UINT_MAX);
    board_set_piece(board, move->queen_dst, tmp);

    // modification of queens array
    for (unsigned int i = 0; i < board->num_queens; i++)
    {
        if (board->queens[tmp.id_player][i] == move->queen_src)
        {
            board->queens[tmp.id_player][i] = move->queen_dst;
        }
    }

    board_set_new_piece(board, move->arrow_dst, ARROW, tmp.id_player);
}

void print_piece(struct piece_t a_piece)
{

    char piece_form[16] = " "; // U+2002

    switch (a_piece.kind)
    {
    case EMPTY:
        strcpy(piece_form, "□");
        break;
    case QUEEN:
        strcpy(piece_form, "♛");
        break;
    case ARROW:
        strcpy(piece_form, "■");
        break;
    default:
        break;
    }

    char color[16] = "";

    switch (a_piece.id_player)
    {
    case 0:
        strcpy(color, "\033[0;31m");
        break;
    case 1:
        strcpy(color, "\033[0;34m");
    default:
        break;
    }

    printf("%s%s\033[0m", color, piece_form);
}

void print_board(struct board_t *board)
{
    printf("\n\n");
    unsigned int m = board->m;

    for (unsigned int i = 0; i < m; i++)
    {
        printf("\t");
        for (unsigned int j = 0; j < m; j++)
        {
            print_piece(board->world[i * m + j]);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n\n");
}

/*
The first function, free_board(), frees the memory allocated for a given board structure,
including its world array, graph structure, and queens array.
The function does not free the memory allocated for the queens array itself.

The second function, hard_free_board(), calls free_board() to free the memory allocated for the board structure,
but also frees the memory allocated for the queens array using free().
This function is useful when the queens array was allocated separately and needs to be freed separately from the board structure.
*/

void free_board(struct board_t *board)
{
    free(board->world);
    free_graph(board->graph);
    free_sub_array_queens(board->queens);
    free(board);
}

void hard_free_board(struct board_t *board)
{

    unsigned int **queens = board->queens;
    free_board(board);
    free(queens);
}

int is_legal_pos(struct board_t *board, unsigned int idx)
{
    if (idx > board->m * board->m)
        return 0;
    else if (is_void(board->graph, idx))
        return 0;
    return 1;
}

/*
returns if a position dst is attainable from src
CAN BE OPTIMIZED
*/
int is_attainable_pos(struct board_t *board, unsigned int src, unsigned int dst)
{
    unsigned int *list = malloc(board->m * board->m * sizeof(unsigned int));
    list_attainable_pos(board, src, list);

    int found = 0;
    for (unsigned int i = 0; i < board->m * board->m; i++)
    {
        if (list[i] == UINT_MAX)
            break;
        if (list[i] == dst)
            found = 1;
    }
    free(list);
    list = NULL;
    return found;
}

/*
fill list[] with attainable positions from src
list[] always end with UINT_MAX
list size is board->m * board->m
*/
void list_attainable_pos(struct board_t *board, unsigned int src, unsigned int list[])
{
    struct piece_t tmp = board_get_piece(board, src);
    board_set_new_piece(board, src, EMPTY, UINT_MAX);

    for (unsigned int i = 0; i < board->m * board->m; i++)
    {
        list[i] = UINT_MAX;
    }

    unsigned int ind = 0;
    for (enum dir_t d = FIRST_DIR; d != LAST_DIR; d++)
    {
        unsigned int neighbor = get_neighbor_in_direction(board->graph, src, d);
        if (is_legal_pos(board, neighbor))
        {
            // printf("neighbor : kind:%d id_player:%d\n", board->world[neighbor].kind, board->world[neighbor].id_player);
            // printf("neighbor of %d in dir %d: %d\n", src, d, get_neighbor_in_direction(board->graph, src, d));
        }
        while (is_legal_pos(board, neighbor))
        {
            if (board->world[neighbor].kind != EMPTY)
                break;
            assert(ind < board->m * board->m);
            list[ind] = neighbor;
            // printf("neighbor of %d in dir %d: %d and ind = %d\n", neighbor, d, get_neighbor_in_direction(board->graph, neighbor, d), ind);
            ind++;
            neighbor = get_neighbor_in_direction(board->graph, neighbor, d);
        }
    }
    // printf("fin de list_attainable, ind=%d\n", ind);
    assert(ind < board->m * board->m);
    list[ind] = UINT_MAX;

    qsort(list, board->m * board->m, sizeof(unsigned int), cmpfunc_uint);
    board_set_piece(board, src, tmp);
}

/*
This fct returns a ptr towards a fully inited board
*/

struct board_t *create_board_and_attributes(unsigned int m, enum graph_shape shape)
{

    unsigned int num_queens = 4 * (m / 10 + 1);

    unsigned int **queens = queens_initialization(num_queens, m);
    struct graph_t *graph = init_graph(shape, m);

    return init_board(graph, queens, num_queens);
}

unsigned int random_attainable_position_from(struct board_t *board, unsigned int src)
{
    size_t possible_pos_length = board->m * board->m + 1;
    unsigned int *possible_pos = malloc(possible_pos_length * sizeof(unsigned int));
    for (unsigned int i = 0; i < possible_pos_length; i++)
    {
        possible_pos[i] = UINT_MAX;
    }

    list_attainable_pos(board, src, possible_pos);
    unsigned int real_size = possible_pos_length;
    for (unsigned int i = 0; i < possible_pos_length; i++)
    {
        if (possible_pos[i] == UINT_MAX)
        {
            real_size = i;
            break;
        }
    }
    unsigned int ind = randint(0, real_size - 1);
    unsigned int res = possible_pos[ind];
    // printf("random pos: %d generated at index %d\n", res, ind);
    free(possible_pos);
    possible_pos = NULL;
    return res;
}

/*
Computes all linked positions to src and modifies the bool arrray accordingly.
*/

void compute_linked_position_from_source(struct board_t *board, unsigned int src, int *linked_pos_bool_array)
{

    linked_pos_bool_array[src] = 1;

    unsigned int *attainable_pos = init_pos_array(board->m * board->m);
    list_attainable_pos(board, src, attainable_pos);

    for (unsigned int i = 0; i < board->m * board->m; i++)
    {

        unsigned int next = attainable_pos[i];

        if (next == UINT_MAX)
        {
            break;
        }

        if (!linked_pos_bool_array[attainable_pos[i]])
        {
            compute_linked_position_from_source(board, attainable_pos[i], linked_pos_bool_array);
        }
    }

    free(attainable_pos);
}

/*
Takes a board and a player_id
Launch the search a linked area for this player, starting by the queens positions
Result is stored in boolean array "linked_pos_bool_array"
*/

int *compute_linked_pos(struct board_t *board, unsigned int player_id)
{

    int *linked_pos_bool_array = calloc(board->m * board->m, sizeof(*linked_pos_bool_array));

    for (unsigned int i = 0; i < board->num_queens; i++)
    {

        compute_linked_position_from_source(board, board->queens[player_id][i], linked_pos_bool_array);
    }

    return linked_pos_bool_array;
}

/*
An heuristic
Given a board and a player_id gives a score in [0,1]
This score is computed thanks to the principle of connexity
*/

float linked_heuristic(struct board_t *board, unsigned int player_id)
{

    int *linked_pos_bool_array_ally = compute_linked_pos(board, player_id);
    int *linked_pos_bool_array_ennemy = compute_linked_pos(board, abs((int)(player_id)-1));

    float total_ally = 0;
    float total_ennemy = 0;

    for (unsigned int i = 0; i < board->m * board->m; i++)
    {
        if (linked_pos_bool_array_ally[i])
        {
            total_ally++;
        }
        if (!linked_pos_bool_array_ennemy[i])
        {
            total_ennemy++;
        }
    }

    free(linked_pos_bool_array_ally);
    free(linked_pos_bool_array_ennemy);

    return (total_ally + total_ennemy) / (float)(2 * board->graph->num_vertices);
}

struct move_t *possible_moves(struct board_t *board, unsigned int player_id)
{
    struct move_t *possible_moves =
        malloc(board->m * board->m * board->m * board->m * board->num_queens * sizeof(struct move_t));
    int nb_elem = 0;

    struct move_t tmp_move;

    // For each queens
    for (unsigned int i = 0; i < board->num_queens; i++)
    {

        // printf("\n queen moved : %d \n", i);

        tmp_move.queen_src = board->queens[player_id][i];

        tmp_move.queen_src = board->queens[player_id][i];

        // Compute possible move for the queen
        unsigned int *possible_queen_moves = init_pos_array(board->m * board->m);
        list_attainable_pos(board, board->queens[player_id][i], possible_queen_moves);

        // printf("\n possible_queen_moves : ");
        // print_uint_array(possible_queen_moves, board->m * board->m);

        // For each queen moves
        for (unsigned int j = 0; j < board->m * board->m; j++)
        {
            if (possible_queen_moves[j] == UINT_MAX)
            {
                break;
            }

            // printf("\n queen dst : %d\n", possible_queen_moves[j]);

            tmp_move.queen_dst = possible_queen_moves[j];

            // tmp move of the queen in order to have arrow moves
            struct piece_t tmp_piece = board_get_piece(board, tmp_move.queen_src);
            board_set_piece(board, tmp_move.queen_dst, tmp_piece);
            board_set_piece(board, tmp_move.queen_src, PIECE_EMPTY);
            board->queens[player_id][i] = tmp_move.queen_dst;

            unsigned int *possible_arrow_moves = init_pos_array(board->m * board->m);

            list_attainable_pos(board, board->queens[player_id][i], possible_arrow_moves);

            // printf("\n possible_arrow_moves : ");
            // print_uint_array(possible_arrow_moves, board->m * board->m);

            for (unsigned int k = 0; k < board->m * board->m; k++)
            {
                if (possible_arrow_moves[k] == UINT_MAX)
                {
                    break;
                }
                tmp_move.arrow_dst = possible_arrow_moves[k];
                possible_moves[nb_elem] = tmp_move;

                // printf("queen src : %d, queen dst : %d, arrow dst : %d\n", possible_moves[nb_elem].queen_src, possible_moves[nb_elem].queen_dst, possible_moves[nb_elem].arrow_dst);

                nb_elem++;
            }

            free(possible_arrow_moves);

            // undo tmp move of the queen
            board_set_piece(board, tmp_move.queen_src, tmp_piece);
            board_set_new_piece(board, tmp_move.queen_dst, EMPTY, UINT_MAX);
            board->queens[player_id][i] = tmp_move.queen_src;
        }

        free(possible_queen_moves);
    }

    possible_moves[nb_elem] = MOVE_NULL; // SENTINEL
    return possible_moves;
}

/*
It then creates a temporary copy of the board and iterates through each possible move,
executing the move and computing a score using the given heuristic function.
*/

float *compute_heuristic_on_possible_moves(struct board_t *board, struct move_t *possible_moves, unsigned int player_id, float (*heuristic)(struct board_t *, unsigned int))
{

    int size = board->m * board->m * board->m * board->m * board->num_queens;

    struct board_t *tmp_board = malloc(sizeof(*board));
    deep_copy_board(tmp_board, board);

    float *hlinked_score_possible_moves = malloc(size * sizeof(float));
    int nb_elem = 0;

    for (int i = 0; i < size; i++)
    {
        if (is_move_null(possible_moves[i]))
        {
            break;
        }

        // do the move
        struct move_t move = possible_moves[i];
        board_execute_move(tmp_board, &(move));

        // compute the score
        float score = (*heuristic)(tmp_board, player_id);
        // printf("%f\n", score);
        hlinked_score_possible_moves[nb_elem] = score;
        nb_elem++;

        // undo the move
        board_set_piece(tmp_board, move.arrow_dst, PIECE_EMPTY);
        struct move_t rmove = {.queen_dst = move.queen_src, .queen_src = move.queen_dst, .arrow_dst = move.arrow_dst};
        board_execute_move(tmp_board, &rmove);
    }

    hard_free_board(tmp_board);

    hlinked_score_possible_moves[nb_elem] = -1; // SENTINEL

    return hlinked_score_possible_moves;
}

struct move_t compute_best_move(struct board_t *board, unsigned int player_id, float (*heuristic)(struct board_t *, unsigned int))
{

    int size = board->graph->num_vertices * board->graph->num_vertices * board->num_queens;
    struct move_t *pos_moves = possible_moves(board, player_id);

    if (is_move_null(pos_moves[0]))
    {
        return MOVE_NULL;
    }

    float *scores = compute_heuristic_on_possible_moves(board, pos_moves, player_id, heuristic);

    float max_score = scores[0];
    int max_score_idx = 0;

    for (int i = 0; i < size; i++)
    {

        float score = scores[i];

        if (score + 1 < 0.0001)
        {
            break;
        }

        if (max_score < score)
        {
            max_score = score;
            max_score_idx = i;
        }
    }

    free(scores);
    struct move_t best_move = pos_moves[max_score_idx];
    free(pos_moves);

    return best_move;
}

/*
returns 1 if the game should end.
(this function tests if the player who should move can move)
*/
int game_over(struct board_t *board, int curr_player_id)
{
    unsigned int *queens = board->queens[curr_player_id];
    for (unsigned int i = 0; i < board->num_queens; i++)
    {
        unsigned int *attainable_positions = malloc(board->graph->num_vertices * sizeof(int));
        list_attainable_pos(board, queens[i], attainable_positions);
        if (attainable_positions[0] != UINT_MAX)
        {
            free(attainable_positions);
            return 0;
        }
        free(attainable_positions);
        attainable_positions = NULL;
    }
    return 1;
}