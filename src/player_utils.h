#ifndef _PLAYER_UTILS_H_
#define _PLAYER_UTILS_H_

#include "board.h"
#include <dlfcn.h>

struct player {
    char const* (*get_player_name)();
    void (*initialize_player)(unsigned int, struct graph_t*, unsigned int, unsigned int**);
    struct move_t (*play_player)(struct move_t);
    void (*finalize_player)();
};

struct player include_player_library(char * player_path, void** player_library, unsigned int player_id);

unsigned int*** initialize_all_players(struct player* all_players, struct board_t* server_board);

#endif // _PLAYER_UTILS_H_
