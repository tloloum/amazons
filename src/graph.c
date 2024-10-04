#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

int is_an_edge(struct graph_t *graph, int i, int j)
{
    if (gsl_spmatrix_uint_get(graph->t, i, j) > 0)
        return 1;
    return 0;
}

int is_void(struct graph_t *graph, int idx)
{
    unsigned int m = sqrt(graph->t->size1);
    for (unsigned int i = 0; i < m * m; i++)
    {
        if (gsl_spmatrix_uint_get(graph->t, idx, i) != NO_DIR)
            return 0;
    }

    return 1;
}

unsigned int get_neighbor_in_direction(struct graph_t *graph, unsigned int row /* index */, enum dir_t direction)
{

    unsigned int m = sqrt(graph->t->size1);
    if (!strcmp(gsl_spmatrix_uint_type(graph->t), "CSR"))
    {
        for (int k = graph->t->p[row]; k < graph->t->p[row + 1]; k++)
        {
            unsigned int col = graph->t->i[k];

            if (gsl_spmatrix_uint_get(graph->t, row, col) == direction)
            {

                return col;
            }
        }
    }

    else
    {
        for (unsigned int vertice = 0; vertice < m * m; vertice++)
        {
            if (gsl_spmatrix_uint_get(graph->t, row, vertice) == direction)
            {
                return vertice;
            }
        }
    }

    return UINT_MAX;
}

enum dir_t get_direction_between_edges(struct graph_t *graph, int i, int j)
{
    return gsl_spmatrix_uint_get(graph->t, i, j);
}

int get_num_vertices(struct graph_t *graph)
{
    return graph->num_vertices;
}


void print_graph(struct graph_t *g, unsigned int m)
{
    if (!g || !g->t)
    {
        printf("Invalid graph or matrix.\n");
        return;
    }

    unsigned int n = g->num_vertices;

    printf("Graph with %u vertices:\n\n", n);

    for (unsigned int row_block = 0; row_block < m; row_block++)
    {
        for (unsigned int i = 0; i < m; i++)
        {
            for (unsigned int col_block = 0; col_block < m; col_block++)
            {
                for (unsigned int j = 0; j < m; j++)
                {
                    unsigned int big_matrix_idx = row_block * m + col_block;
                    unsigned int small_matrix_idx = i * m + j;
                    unsigned int edge = gsl_spmatrix_uint_get(g->t, big_matrix_idx, small_matrix_idx);

                    switch (edge)
                    {
                    case NO_DIR:
                        printf("0 ");
                        break;
                    case DIR_NORTH:
                        printf("N ");
                        break;
                    case DIR_SOUTH:
                        printf("S ");
                        break;
                    case DIR_WEST:
                        printf("W ");
                        break;
                    case DIR_EAST:
                        printf("E ");
                        break;
                    default:
                        printf("? ");
                    }
                }
                printf("  ");
            }
            printf("\n");
        }
        printf("\n");
    }
}

/*----------CONSTRUCTION----------*/

/*
Make the square board
PARAM : m (int), the width of the scare (which is given as a compilation parameter)

RETURN : a pointer of new struct graph_t, initialize as a square with all the relation
*/
struct graph_t *make_square_graph(unsigned int m)
{
    unsigned int n = m * m;

    // sparse matrix allocation
    gsl_spmatrix_uint *t = gsl_spmatrix_uint_alloc(n, n);

    // graph allocation
    struct graph_t *g = malloc(sizeof(struct graph_t));

    // linking of graph attributes
    g->num_vertices = n;
    g->t = t;

    // initialization of relations
    for (unsigned int i = 0; i < n; i++)
    {
        for (unsigned int j = 0; j < n; j++)
        {
            int dx = (i % m) - (j % m); // column of i - column of j
            int dy = (i / m) - (j / m); // line of i - line of j

            if (dx == 0 && (dy == 1 || -1 == dy))
            { // same column
                dy == -1 ? gsl_spmatrix_uint_set(t, i, j, DIR_SOUTH) : gsl_spmatrix_uint_set(t, i, j, DIR_NORTH);
            }
            else if (dy == 0 && (1 == dx || -1 == dx))
            { // same line TODO
                dx == 1 ? gsl_spmatrix_uint_set(t, i, j, DIR_WEST) : gsl_spmatrix_uint_set(t, i, j, DIR_EAST);
            }

            else if (dx == 0 && dy == 0) // i = j
                gsl_spmatrix_uint_set(t, i, j, NO_DIR);
        }
    }

    return g;
}

/*
PARAM :
    - a pointer of struct graph_t
    - a position (int) of a summit

ACTION :
    - change the graph and delete the summit "position"
    - adapt all the relation
*/
void make_void(struct graph_t *g, int position)
{
    int n = g->num_vertices;
    gsl_spmatrix_uint *t = g->t;

    for (int i = 0; i < n; i++)
    {
        gsl_spmatrix_uint_set(t, position, i, NO_DIR);
        gsl_spmatrix_uint_set(t, i, position, NO_DIR);
    }
}

/* Make the donut board
PARAM : m (int), the width of the scare (which is given as a compilation parameter)

PRECOND : m % 3 = 0

RETURN : a pointer of new struct graph_t, initialize as a donut -> the center of the graph is empty (an empty square with a side of m/3)
*/
struct graph_t *make_donut_graph(unsigned int m)
{
    assert(m % 3 == 0);
    struct graph_t *g = make_square_graph(m);

    unsigned int square_size = m / 3;

    for (unsigned int i = square_size; i < square_size * 2; i++)
    {
        for (unsigned int j = square_size; j < square_size * 2; j++)
        {
            make_void(g, i * m + j);
        }
    }

    g->num_vertices = (8 * m * m) / 9;

    return g;
}

/*
make clover
Remove four squares of vertices from the graph, positioned at 1/5 and 2/5, and 3/5 and 4/5 on both width and height.

PARAM: g (struct graph_t*) : a pointer to the graph to modify

PRECOND: m % 5 = 0

RETURN: struct graph_t
*/
struct graph_t *make_clover_graph(unsigned int m)
{

    assert(m % 5 == 0);
    struct graph_t *g = make_square_graph(m);

    unsigned int s = m / 5;    // size of the square to remove
    unsigned int pos1 = s;     // position of first square
    unsigned int pos2 = 3 * s; // position of second square

    // Remove the first square
    for (unsigned int i = pos1; i < 2 * s; i++)
    {
        for (unsigned int j = pos1; j < 2 * s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    // Remove the second square
    for (unsigned int i = pos1; i < 2 * s; i++)
    {
        for (unsigned int j = pos2; j < pos2 + s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    // Remove the third square
    for (unsigned int i = pos2; i < pos2 + s; i++)
    {
        for (unsigned int j = pos1; j < 2 * s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    // Remove the fourth square
    for (unsigned int i = pos2; i < pos2 + s; i++)
    {
        for (unsigned int j = pos2; j < pos2 + s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    g->num_vertices = (21 * m * m) / 25;
    return g;
}

/*
make eigth
Remove 2 squares of vertices from the graph. 8-shaped in the middle

PARAM: g (struct graph_t*) : a pointer to the graph to modify

PRECOND: m % 4 = 0

RETURN: struct graph_t
*/
struct graph_t *make_eight_graph(unsigned int m)
{

    assert(m % 4 == 0);
    struct graph_t *g = make_square_graph(m);

    unsigned int s = m / 4;

    // First square
    for (unsigned int i = s; i < 2 * s; i++)
    {
        for (unsigned int j = 2 * s; j < 3 * s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    // Second square
    for (unsigned int i = 2 * s; i < 3 * s; i++)
    {
        for (unsigned int j = s; j < 2 * s; j++)
        {
            make_void(g, i * m + j);
        }
    }

    // diagonal
    unsigned int diag_one = (2 * s - 1) * m + (2 * s - 1);
    unsigned int diag_two = 2 * s * m + 2 * s;

    gsl_spmatrix_uint_set(g->t, diag_one, diag_two, DIR_SE);
    gsl_spmatrix_uint_set(g->t, diag_two, diag_one, DIR_NW);

    g->num_vertices = (7 * m * m) / 8;
    return g;
}

struct graph_t *init_graph(enum graph_shape shape, unsigned int width)
{

    // Preparation of the returned graph
    struct graph_t *board_graph = NULL;

    switch (shape)
    {
    case SQUARE:
        board_graph = make_square_graph(width);
        break;
    case DONUT:
        board_graph = make_donut_graph(width);
        break;
    case CLOVER:
        board_graph = make_clover_graph(width);
        break;
    case EIGHT:
        board_graph = make_eight_graph(width);
        break;
    default:
        board_graph = make_square_graph(width);
        printf("FAILED TO INIT GRAPH : DEFAULT MODE");
        break;
    }

    return board_graph;
}

/*
Operates a deep_copy of a graph, from source to destination
*/
void deep_copy_graph(struct graph_t *destination, struct graph_t *source)
{

    memcpy(destination, source, sizeof(*source));

    destination->t = gsl_spmatrix_uint_alloc(source->t->size1, source->t->size2);

    if(!strcmp(gsl_spmatrix_uint_type(source->t), "CSR")){
        gsl_spmatrix_uint * tmp = destination->t;
        destination->t = gsl_spmatrix_uint_compress(destination->t, GSL_SPMATRIX_CSR);
        gsl_spmatrix_uint_free(tmp);
    }

    gsl_spmatrix_uint_memcpy(destination->t, source->t);
}

void free_graph(struct graph_t *graph)
{

    gsl_spmatrix_uint_free(graph->t);
    free(graph);
}