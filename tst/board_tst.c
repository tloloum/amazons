#include "board_tst.h"
#include "test_tools.h"
#include "../src/utils.h"
#include "../src/board.h"


#include <string.h>
#include <stdio.h>

int alltests_board(){

    execute_test_function(&test_list_attainable_pos, "list_attainaible_pos");
    execute_test_function(&test_compute_linked_pos, "compute_linked_pos");
    execute_test_function(&test_possible_moves, "possible_moves");
    execute_test_function(&test_compute_hlinked_possible_moves, "compute_hlinked_possible_moves");
    execute_test_function(&test_linked_heuristic, "linked_heuristic");
    execute_test_function(&test_compute_best_move, "compute_best_move");

    return 0;
}

int * test_list_attainable_pos(){

    struct board_t* board = create_board_and_attributes(5, SQUARE);
    
    for(int i=0; i<25; i++){
        board_set_new_piece(board, i, EMPTY, UINT_MAX);
    }

    int nb_test = 1;
    int passed = 0;

    unsigned int t1_exp[] = {2, 5, 6, 8, 9, 12, 17, 22, UINT_MAX,
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};

    unsigned int t1[] = 
    {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, 
    UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};

    list_attainable_pos(board, 7, t1);

    if(!memcmp(t1, t1_exp, board->graph->num_vertices*sizeof(t1[0]))){
        passed++;
    }

    hard_free_board(board);

    return make_result(nb_test, passed);
}

int* test_compute_linked_pos(){

    int nb_tests = 2;
    int passed = 0;

    struct board_t* board = create_board_and_attributes(5, SQUARE);
    
    int* linked_pos = compute_linked_pos(board, 0);

    //print_int_array(linked_pos, board->graph->num_vertices);

    //print_board(board);

    for(unsigned int i=0; i<board->graph->num_vertices; i++){

        if(linked_pos[i]){
            board_set_new_piece(board, i, ARROW, 0);
        }
        else{
            board_set_new_piece(board, i, ARROW, 1);
        }

    }

    //print_board(board);

    //1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 1 | 0 | 0 |

    int linked_pos_exp[] = {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 1 , 1 , 1 , 0 , 0 , 0 , 1 , 0 , 0 };

    if(!memcmp(linked_pos, linked_pos_exp, board->graph->num_vertices)){
        passed++;
    }


    free(linked_pos);
    hard_free_board(board);

    board = create_board_and_attributes(5, SQUARE);

    board_set_new_piece(board, 1, EMPTY, -1);
    board_set_new_piece(board, 3, EMPTY, -1);
    board_set_new_piece(board, 5, EMPTY, -1);
    board_set_new_piece(board, 9, EMPTY, -1);

    board_set_new_piece(board, 15, EMPTY, -1);
    board_set_new_piece(board, 19, EMPTY, -1);
    board_set_new_piece(board, 21, EMPTY, -1);
    board_set_new_piece(board, 23, EMPTY, -1);

    board_set_new_piece(board, 0, QUEEN, 1);
    board_set_new_piece(board, 1, QUEEN, 1);
    board_set_new_piece(board, 11, QUEEN, 1);
    board_set_new_piece(board, 16, QUEEN, 1);

    board_set_new_piece(board, 20, VOID, UINT_MAX);


    board_set_new_piece(board, 12, QUEEN, 0);
    board_set_new_piece(board, 19, QUEEN, 0);
    board_set_new_piece(board, 23, QUEEN, 0);
    board_set_new_piece(board, 24, QUEEN, 0);
    board_set_new_piece(board, 7, ARROW, 0);

    board->queens[0][0]=12;
    board->queens[0][1]=19;
    board->queens[0][2]=23;
    board->queens[0][3]=24;

    board->queens[1][0]=0;
    board->queens[1][1]=1;
    board->queens[1][2]=11;
    board->queens[1][3]=16;


    linked_pos = compute_linked_pos(board, 0);

    //print_int_array(linked_pos, board->graph->num_vertices);

    //print_board(board);

    for(unsigned int i=0; i<board->graph->num_vertices; i++){

        if(linked_pos[i]){
            board_set_new_piece(board, i, ARROW, 0);
        }
        else{
            board_set_new_piece(board, i, ARROW, 1);
        }

    }

    //print_board(board);

    //0 | 0 | 1 | 1 | 1 | 0 | 0 | 0 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | 0 | 1 | 1 | 1 | 1 |

    int linked_pos_exp_bis[]= {0 , 0 , 1 , 1 , 1 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 1 , 1 , 1 , 0 , 0 , 1 , 1 , 1 , 0 , 1 , 1 , 1 , 1 };

    if(!memcmp(linked_pos, linked_pos_exp_bis, board->graph->num_vertices)){
        passed++;
    }

    free(linked_pos);
    hard_free_board(board);

    return make_result(nb_tests, passed);

}

int* test_linked_heuristic(){

    int nb_tests = 1;
    int passed = 0;

    struct board_t* board = create_board_and_attributes(5, SQUARE);

    float score = linked_heuristic(board, 0);

    printf("%f\n", score);

    hard_free_board(board);

    passed++;
    return make_result(nb_tests, passed);

}

int* test_possible_moves(){
    
    struct board_t* board = create_board_and_attributes(5, SQUARE);
    
    struct move_t* pos_moves = possible_moves(board, 0);
    int max_nb_moves = board->graph->num_vertices*board->graph->num_vertices*board->num_queens;
    
    for(int i=0; i<max_nb_moves; i++){
        if(is_move_null(pos_moves[i])){
            break;
        }
        //printf("queen src : %d, queen dst : %d, arrow dst : %d\n", pos_moves[i].queen_src, pos_moves[i].queen_dst, pos_moves[i].arrow_dst);

    }

    free(pos_moves);
    hard_free_board(board);

    return make_result(1,1);

}

int* test_compute_hlinked_possible_moves(){

    int nb_test = 1;
    int passed = 0;

    struct board_t* board = create_board_and_attributes(5, SQUARE);
    struct move_t* pos_moves = possible_moves(board, 0);

    float* hlinked_pos = compute_heuristic_on_possible_moves(board, pos_moves, 0, &linked_heuristic);

    //print_float_array(hlinked_pos, board->graph->num_vertices*board->graph->num_vertices*board->num_queens);

    free(hlinked_pos);
    free(pos_moves);
    hard_free_board(board);

    passed++;

    return make_result(nb_test, passed);

}

int* test_compute_best_move(){

    int nb_tests = 1;
    int passed = 0;

    struct board_t* board = create_board_and_attributes(5, SQUARE);

    struct move_t better_move = compute_best_move(board, 0, &linked_heuristic);

    printf("queen_src : %d | queen_dst : %d | arrow_dst : %d \n", better_move.queen_src, better_move.queen_dst, better_move.arrow_dst);

    hard_free_board(board);

    passed++;
    return make_result(nb_tests, passed);

}