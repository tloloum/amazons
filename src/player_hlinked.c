#include "player.h"
#include "board.h"
#include "utils.h"

#include <time.h>
#include <assert.h>


// /* -------------- Player -------------- */

char const* player_name = "QUoicouBot3000";

unsigned int id_player = 0;

unsigned int players_nb_queens = 0;

unsigned int ** player_queens = NULL;

// /* -------------- Terrain -------------- */

struct board_t *player_board = NULL;

// /* -------------- Function -------------- */

char const* get_player_name(){
    return player_name;
}

/*
Initialize useful global variable about players : nb_queens, id, player1_queens, player_graph, num_vertices
*/
void initialize(unsigned int player_id, struct graph_t* graph, unsigned int num_queens, unsigned int* queens[NUM_PLAYERS]){ 
    
    //Storage of important variables
    id_player=player_id;
    players_nb_queens=num_queens;
    player_queens=queens;

    //Initialize player's board
    player_board=init_board(graph, player_queens, players_nb_queens);
    
    //conversion into CSR format
    gsl_spmatrix_uint* tmp = player_board->graph->t;
    player_board->graph->t = gsl_spmatrix_uint_compress(player_board->graph->t, GSL_SPMATRIX_CSR);
    gsl_spmatrix_uint_free(tmp);

    printf("Player %d initialized.\n", id_player);
}

/*
Update board and player1_queens with previous_move 
*/

void update_board(struct move_t* previous_move){

    board_execute_move(player_board, previous_move);

}

struct move_t play(struct move_t previous_move){

    update_board(&previous_move);

    struct move_t move = compute_best_move(player_board, id_player, &linked_heuristic);

    update_board(&move);

    return move;

}

 
void finalize(){

    free_board(player_board);

}