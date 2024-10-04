#ifndef _AMAZON_GRAPH_H_
#define _AMAZON_GRAPH_H_

#include <stddef.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_spmatrix_uint.h>
#include <gsl/gsl_spblas.h>

#include "dir.h"

enum graph_shape
{
  SQUARE = 0,
  DONUT = 1,
  CLOVER = 2,
  EIGHT = 3
};

struct graph_t
{
  unsigned int num_vertices; // Number of vertices in the graph
  gsl_spmatrix_uint *t;      // Sparse matrix of size n*n,
                             // t[i][j] > 0 means there is an edge from i to j
                             // t[i][j] == DIR_NORTH means that j is NORTH of i
                             // t[i][j] == DIR_SOUTH means that j is SOUTH of i
                             // and so on
};

int is_an_edge(struct graph_t *graph, int i, int j);

int is_void(struct graph_t *graph, int idx);

enum dir_t get_direction_between_edges(struct graph_t *graph, int i, int j);

unsigned int get_neighbor_in_direction(struct graph_t *graph, unsigned int index, enum dir_t direction);

int get_num_vertices(struct graph_t *graph);

// modify the neighbors array (firstly filled with DIR_ERROR), link every direction with the related neighbors
// DIR_ERROR if no neighbors in the direction
void get_neighbors(struct graph_t *graph, unsigned int idx, unsigned int neighbors[]);

struct graph_t *init_graph(enum graph_shape shape, unsigned int width);

struct graph_t *make_square_graph(unsigned int m);

struct graph_t *make_eight_graph(unsigned int m);

void available_square_directions(enum dir_t directions[NUM_DIRS], unsigned int vertice, unsigned int m);

void free_graph(struct graph_t *graph);

void print_graph(struct graph_t *, unsigned int);

void deep_copy_graph(struct graph_t *destination, struct graph_t *source);

struct graph_t *make_donut_graph(unsigned int m);

struct graph_t *make_clover_graph(unsigned int m);


#endif // _AMAZON_GRAPH_H_