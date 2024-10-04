#include "player.h"
#include "board.h"

#include <time.h>
#include <assert.h>

// /* -------------- Player -------------- */

char const *player_name = "Côme quand la forge est rouge";

unsigned int id_player = 0;

unsigned int players_nb_queens = 0;

unsigned int **player_queens = NULL;

// /* -------------- Terrain -------------- */

struct board_t *player_board = NULL;

// /* -------------- Function -------------- */

char const *get_player_name()
{
    return player_name;
}

/*
Initialize useful global variable about players : nb_queens, id, player1_queens, player_graph, num_vertices
*/
void initialize(unsigned int player_id, struct graph_t *graph, unsigned int num_queens, unsigned int *queens[NUM_PLAYERS])
{

    // Storage of important variables
    id_player = player_id;
    players_nb_queens = num_queens;
    player_queens = malloc(NUM_PLAYERS*sizeof(*queens));

    for(unsigned int i = 0; i<NUM_PLAYERS; i++){
        player_queens[i]=queens[i];
    }

    // Initialize player's board
    player_board = init_board(graph, player_queens, players_nb_queens);

    /* // conversion into CSR format
    gsl_spmatrix_uint *tmp = player_board->graph->t;
    player_board->graph->t = gsl_spmatrix_uint_compress(player_board->graph->t, GSL_SPMATRIX_CSR);
    gsl_spmatrix_uint_free(tmp); */

    printf("Player %d initialized.\n", id_player);
}

/*
Update board and player1_queens with previous_move
*/

void update_board(struct move_t *previous_move)
{

    board_execute_move(player_board, previous_move);
}

/*
Framework :
    - choose a random queen using player_queens
    - compute all possible moves (queens + arrow) and chose a random one
    - update the player_queens and the player_board
    - return the chosen move
*/
struct move_t play(struct move_t previous_move)
{

    // update the player's board
    if (!move_is_equal(previous_move, MOVE_NULL))
        update_board(&previous_move);

    // find the player's queens
    unsigned int *queens = malloc(players_nb_queens * sizeof(unsigned int));
    unsigned int ind = 0;
    for (unsigned int i = 0; i < player_board->m * player_board->m; i++)
    {
        struct piece_t curr = player_board->world[i];
        if (curr.id_player == id_player && curr.kind == QUEEN)
        {
            assert(ind < players_nb_queens); // size problem
            queens[ind] = i;
            ind++;
        }
    }

    // get random move
    struct move_t res;
    for (unsigned int i = 0; i < 100; i++)
    {
        if (i > 0)
        {
            printf("player %d: queen %d can't move, retrying (%d)...\n", id_player, res.queen_src, i);
            if (i >= 99)
            {
                res = MOVE_NULL;
                break;
            }
        }
        res.queen_src = queens[randint(0, ind - 1)];
        if (!is_legal_pos(player_board, res.queen_src))
            continue;
        res.queen_dst = random_attainable_position_from(player_board, res.queen_src);
        if (!is_legal_pos(player_board, res.queen_dst))
            continue;
        struct piece_t tmp = board_get_piece(player_board, res.queen_src);
        board_set_new_piece(player_board, res.queen_src, EMPTY, UINT_MAX);
        res.arrow_dst = random_attainable_position_from(player_board, res.queen_dst);
        board_set_piece(player_board, res.queen_src, tmp);

        if (!is_legal_pos(player_board, res.arrow_dst))
            continue;
        break;
    }

    if (!move_is_equal(res, MOVE_NULL))
        update_board(&res);

    free(queens);
    queens = NULL;

    return res; 
}

void finalize()
{

    hard_free_board(player_board);
}
