#include "board.h"
#include "player_utils.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

static int is_legal_move(struct board_t *board, unsigned int player_id, struct move_t move)
{
    if (!is_legal_pos(board, move.queen_src) || !is_legal_pos(board, move.queen_dst) || !is_legal_pos(board, move.arrow_dst))
    {
        printf("illegal move : illegal position\n");
        return 0;
    }
    struct piece_t queen = board_get_piece(board, move.queen_src);
    if (queen.id_player != player_id || queen.kind != QUEEN)
    {
        printf("illegal move : wrong piece in src position : kind=%d expected %d, id_player=%d expected %d\n", queen.kind, QUEEN, queen.id_player, player_id);
        return 0;
    }

    struct piece_t tmp = board_get_piece(board, move.queen_src);
    board_set_new_piece(board, move.queen_src, EMPTY, UINT_MAX);

    if (!is_attainable_pos(board, move.queen_src, move.queen_dst) || !is_attainable_pos(board, move.queen_dst, move.arrow_dst))
    {
        printf("illegal move : unreachable position\n");
        board_set_piece(board, move.queen_src, tmp);
        return 0;
    }

    board_set_piece(board, move.queen_src, tmp);

    return 1;
}


void finalize_server(struct board_t *board, void **players_libraries, struct player *all_players, unsigned int ***queens_all_player)
{

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        dlclose(players_libraries[i]);
        free_queens(queens_all_player[i]);
    }

    hard_free_board(board);
    free(queens_all_player);
    free(all_players);
}

enum graph_shape get_init_shape(char t)
{
    switch (t)
    {
    case 'c':
        return SQUARE;
        break;
    case 'd':
        return DONUT;
        break;
    case 't':
        return CLOVER;
        break;
    case '8':
        return EIGHT;
        break;
    default:
        return SQUARE;
        break;
    }
}

void print_move(struct move_t move) {
    printf("src : %d\n", move.queen_src);
    printf("dst : %d\n", move.queen_dst);
    printf("arrow dst : %d\n", move.arrow_dst);
}

/*
The information of the world is given throught the queens array (unsigned int **)
But client and server use an easier representation of the world, using a struct board
*/
int main(int argc, char **argv)
{

    srand(time(NULL));

    unsigned int m = 8; // width of the board
    char t = 'c';       // shape of the board

    command_line_reader(argc, argv, ":m:t:", &m, &t);

    printf("%c, %d\n", t, m);

    enum graph_shape shape = get_init_shape(t);

    // Server board initialization => spmatrix, graph, world, queens placements
    struct board_t *server_board = create_board_and_attributes(m, shape);

    // players init
    void *players_libraries[NUM_PLAYERS];

    struct player *all_players = malloc(NUM_PLAYERS * sizeof(struct player));

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        printf("Loading player %d...\n", i);
        all_players[i] = include_player_library(argv[optind + i], players_libraries, i);
        printf("Load Successful\n");
    }

    unsigned int ***queen_all_players = initialize_all_players(all_players, server_board);

    gsl_spmatrix_uint* tmp = server_board->graph->t;
    server_board->graph->t = gsl_spmatrix_uint_compress(server_board->graph->t, GSL_SPMATRIX_CSR);
    gsl_spmatrix_uint_free(tmp);

    print_board(server_board);

    // Game loop
    struct move_t move = MOVE_NULL;
    int curr = 0;
    for (unsigned int i = 0; i < MAX_TURNS; i++)
    {
        struct player *curr_player = &all_players[curr];

        // test if game is over
        if (game_over(server_board, curr))
            break;

        // call the play function of the player
        printf("player %d playing...\n", curr);
        move = (*curr_player->play_player)(move);
        print_move(move);

        // test if the move chosen by the player is legal
        if (is_legal_move(server_board, curr, move))
            board_execute_move(server_board, &move);
        else
            break; // if not, the player loses

        print_board(server_board);

        curr = 1 - curr;
    }

    printf("END OF THE GAME, player %d won the game\n", 1-curr);

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        (*(all_players[i].finalize_player))();
    }

    finalize_server(server_board, players_libraries, all_players, queen_all_players);

    return 0;
}