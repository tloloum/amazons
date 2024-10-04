#include "player_utils.h"
#include "utils.h"

struct player include_player_library(char * player_path, void** player_library, unsigned int player_id) {

    struct player my_player;

    dlerror();
    player_library[player_id] = dlopen(player_path, RTLD_LAZY);
    if(!player_library[player_id]) {
        printf("ERROR : IMPOSSIBLE TO LOAD SHARED LIB\n");
    }

    dlerror();
    my_player.get_player_name =  dlsym(player_library[player_id], "get_player_name");
    check_lib_load();

    dlerror();
    my_player.initialize_player = dlsym(player_library[player_id], "initialize");
    check_lib_load();
    
    dlerror();
    my_player.play_player = dlsym(player_library[player_id], "play");
    check_lib_load();

    dlerror();
    my_player.finalize_player = dlsym(player_library[player_id], "finalize");
    check_lib_load();

    return my_player;
}

/*
This function initialize all players, giving them a copy of the initial world state (queens and graph)
The array returned contains element thats are to be freed by the server at the end of the game
*/
unsigned int*** initialize_all_players(struct player* all_players, struct board_t* server_board){

    unsigned int*** queens_all_players = malloc(NUM_PLAYERS* sizeof(void *));

    for(int i=0; i<NUM_PLAYERS; i++){  
        struct graph_t* graph_for_player = malloc(sizeof(*graph_for_player));
        deep_copy_graph(graph_for_player, server_board->graph);

        unsigned int** queens_for_player = malloc(NUM_PLAYERS*sizeof(*queens_for_player));

        deep_copy_queens(queens_for_player, server_board->queens, server_board->num_queens);
        queens_all_players[i]=queens_for_player;

        
        gsl_spmatrix_uint* tmp = graph_for_player->t;
        graph_for_player->t = gsl_spmatrix_uint_compress(graph_for_player->t, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(tmp);
    
        (*all_players[i].initialize_player)(i, graph_for_player, server_board->num_queens, queens_for_player);
    }

    return queens_all_players;
}
