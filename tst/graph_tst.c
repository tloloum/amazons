#include "graph_tst.h"
#include "test_tools.h"
#include <string.h>

int alltests_graph()
{

    execute_test_function(&test_get_direction_between_edges, "get_direction_between_edges");
    execute_test_function(&test_get_neighbor_in_direction, "test_get_neighbor_in_direction");
    execute_test_function(&test_board_set_new_piece, "test_board_set_new_place");
    execute_test_function(&test_board_get_piece, "test_board_get_piece");
    return 0;
}

int *test_get_direction_between_edges()
{

    struct graph_t *graph = make_square_graph(5); // make_square_graph is supposed to work correctly

    int nb_tests = 23;
    int i[6] = {0, 2, 5, 7, 13, 20};
    int j[6] = {5, 1, 11, 2, 3, 19};
    enum dir_t dirs[6] = {DIR_SOUTH, DIR_WEST, NO_DIR, DIR_NORTH, NO_DIR, NO_DIR};

    int passed = 0;

    for (int k = 0; k < 6; k++)
    {
        passed += get_direction_between_edges(graph, i[k], j[k]) == dirs[k];
    }

    free_graph(graph);

    struct graph_t *donut = make_donut_graph(3);

    int a[17] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 1, 2, 3, 5, 6, 7, 8};
    int b[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 4, 4, 4, 4, 4, 4, 4, 4};
    enum dir_t dirs_donut[17] = {NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR, NO_DIR};

    for (int k = 0; k < 17; k++)
    {
        passed += get_direction_between_edges(donut, a[k], b[k]) == dirs_donut[k];
    }

    free_graph(donut);

    struct graph_t *clover = make_clover_graph(10);

    int nb_clover_tests = 4;
    int c_i[4] = {0, 0, 5 * 10, 5 * 10};
    int c_j[4] = {2 * 10 + 1, 3 * 10, 2 * 10 + 1, 3 * 10};
    enum dir_t clover_dirs[4] = {NO_DIR, NO_DIR, NO_DIR, NO_DIR};

    for (int k = 0; k < nb_clover_tests; k++)
    {
        passed += get_direction_between_edges(clover, c_i[k], c_j[k]) == clover_dirs[k];
    }

    free_graph(clover);

    struct graph_t *eight = make_eight_graph(12);

    int nb_eight_tests = 6;
    int e_i[6] = {3 * 12 + 5, 5 * 12 + 3, 5 * 12 + 6, 5 * 12 + 5, 0, 6 * 12 + 6};
    int e_j[6] = {3 * 12 + 6, 6 * 12 + 3, 5 * 12 + 5, 6 * 12 + 6, 1, 5 * 12 + 5};
    enum dir_t eight_dirs[6] = {NO_DIR, NO_DIR, NO_DIR, DIR_SE, DIR_EAST, DIR_NW};

    for (int k = 0; k < nb_eight_tests; k++)
    {
        passed += get_direction_between_edges(eight, e_i[k], e_j[k]) == eight_dirs[k];
        if ((get_direction_between_edges(eight, e_i[k], e_j[k]) == eight_dirs[k]) == 0)
            printf("les troubleurs : %d\n", eight_dirs[k]);
    }

    free_graph(eight);

    return make_result(nb_tests + nb_clover_tests + nb_eight_tests, passed);
}

/*
For each test, there is a version on both COO and CSR matrix
*/
int *test_get_neighbor_in_direction()
{

    struct graph_t *graph = make_square_graph(5); // make_square_graph is supposed to work correctly
    struct graph_t *graph_csr = make_square_graph(5);
    gsl_spmatrix_uint *tmp = graph_csr->t;
    graph_csr->t = gsl_spmatrix_uint_compress(graph_csr->t, GSL_SPMATRIX_CSR);
    gsl_spmatrix_uint_free(tmp);

    int vertices[10] = {0, 3, 5, 7, 9, 11, 13, 15, 17, 20};
    enum dir_t directions[10] = {DIR_NORTH, DIR_SOUTH, DIR_NE, DIR_WEST, DIR_SW, DIR_NW, DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_EAST};

    // Expected output for each test case
    unsigned int expected[10] = {UINT_MAX, 8, UINT_MAX, 6, UINT_MAX, UINT_MAX, 8, 16, 22, 21};

    int nb_tests = 20;
    int passed = 0;

    for (int i = 0; i < nb_tests / 2; i++)
    {
        unsigned int result = get_neighbor_in_direction(graph, vertices[i], directions[i]);
        if (result == expected[i])
        {
            passed++;
        }
        else
        {
            printf("Expected : %d , Result : %d ", expected[i], result);
        }
    }

    free_graph(graph);

    for (int i = 0; i < nb_tests / 2; i++)
    {
        unsigned int result = get_neighbor_in_direction(graph_csr, vertices[i], directions[i]);
        if (result == expected[i])
        {
            passed++;
        }
        else
        {
            printf("Expected : %d , Result : %d ", expected[i], result);
        }
    }

    free_graph(graph_csr);

    return make_result(nb_tests, passed);
}

int *test_board_set_new_piece()
{

    struct board_t *board = create_board_and_attributes(5, SQUARE);

    int nb_tests = 1;

    board_set_new_piece(board, 2, QUEEN, 1);

    // Check that the correct piece was added
    int passed = 0;
    if (board->world[2].kind == 2 && board->world[2].id_player == 1)
    {
        passed++;
    }

    hard_free_board(board);

    return make_result(nb_tests, passed);
}

int *test_board_get_piece()
{

    struct board_t *board = create_board_and_attributes(5, SQUARE);

    int nb_tests = 2;
    int passed = 0;

    int idx1 = 12;
    int idx2 = 10;

    board_set_new_piece(board, idx1, QUEEN, 1);
    board_set_new_piece(board, idx2, ARROW, 0);

    struct piece_t piece1 = board_get_piece(board, idx1);
    struct piece_t piece2 = board_get_piece(board, idx2);

    if (piece1.kind == QUEEN && piece1.id_player == 1)
    {
        passed++;
    }

    if (piece2.kind == ARROW && piece2.id_player == 0)
    {
        passed++;
    }

    hard_free_board(board);

    return make_result(nb_tests, passed);
}

/* int* test_get_neighbors(){
    struct graph_t* graph = make_square_graph(5);

    unsigned int t1_exp[NUM_DIRS] = {2, UINT_MAX, 8, UINT_MAX, 12, UINT_MAX, 6, UINT_MAX};
    unsigned int t1[NUM_DIRS] = {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};

    get_neighbors(graph, 7, t1);

    int nb_tests=1;
    int passed=0;

    if(!memcmp(t1_exp, t1, NUM_DIRS*sizeof(t1[0]))){
        passed++;
    }

    return make_result(nb_tests, passed);

} */