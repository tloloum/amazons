#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "utils.h"

#include <stdlib.h>

int randint(int inf, int sup)
{
    return (inf + (int)(((int)rand() / ((double)(RAND_MAX) + 1)) * abs(sup - inf + 1)));
}

int move_is_equal(struct move_t a, struct move_t b)
{
    return a.queen_src == b.queen_src && a.queen_dst == b.queen_dst && a.arrow_dst == b.arrow_dst;
}

int cmpfunc_uint(const void *a, const void *b)
{
    if (*(unsigned int *)b == UINT_MAX)
        return -1;
    return (*(int *)a - *(int *)b);
}


void command_line_reader(int argc, char* argv[], const char* optstring, unsigned int* m, char* t){
    int opt;
    while((opt = getopt(argc, argv, optstring))!=-1){
        switch(opt){
            case ':' : 
                printf("option needs a value\n");
                break;
            case 'm' : //width
                *m=atoi(optarg);
                break;
            case 't' :
                *t=optarg[0];
                break;
            case '?' :
                printf("Unknown option : %c\n", opt);
                break;
        }
    }
}

int check_lib_load()
{

    if (dlerror() != NULL)
    {
        printf("ERROR : IMPOSSIBLE TO LOAD FUNCTION FROM SHARED LIB\n");
        return 0;
    }

    return 1;
}

/*
Operates a deep_copy of a queens array, from source to destination
*/
void deep_copy_queens(unsigned int **destination, unsigned int **source, unsigned int num_queens)
{

    memcpy(destination, source, NUM_PLAYERS * sizeof(*destination));

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        destination[i] = malloc(num_queens * sizeof(*destination[i]));
        memcpy(destination[i], source[i], num_queens * sizeof(*destination[i]));
    }
}

void free_sub_array_queens(unsigned int **queens)
{

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        free(queens[i]);
    }

}

void free_queens(unsigned int** queens){

    free(queens);

}

//Initialization of the queens array (placement of the queens)
//further, the board's initialization is based on this queens array
//m is the side length of the board
unsigned int** queens_initialization(unsigned int num_queens, unsigned int m) {
    unsigned int ** queens = malloc(NUM_PLAYERS*sizeof(*queens));
    for(int i=0; i<NUM_PLAYERS; i++){
        queens[i]=malloc(num_queens*sizeof(*queens[i]));
    }

    // initial placement of the queens
    /* WARNING : FOR THE MOMENT, THIS PLACEMENT IS EFFECTIVE FOR 2 PLAYERS ONLY */

    unsigned int i = (m <= 5 || m == 10) ? 1 : 2;
    unsigned int nb_queens_placed = 0;
    int world_size = m * m;

    while (nb_queens_placed < num_queens && i < m / 2)
    {

        queens[0][nb_queens_placed] = i;
        queens[1][nb_queens_placed] = world_size - i - 1;
        nb_queens_placed++;

        queens[0][nb_queens_placed] = m - i - 1;
        queens[1][nb_queens_placed] = world_size - (m - i - 1) - 1;
        nb_queens_placed++;

        queens[0][nb_queens_placed] = i * m;
        queens[1][nb_queens_placed] = world_size - i * m - 1;
        nb_queens_placed++;

        queens[0][nb_queens_placed] = i * m + m - 1;
        queens[1][nb_queens_placed] = world_size - (i * m + m - 1) - 1;
        nb_queens_placed++;

        i += 2;
    }

    return queens;
}

unsigned int *init_pos_array(unsigned int size)
{

    unsigned int *array = malloc(size * sizeof(unsigned int));

    for (unsigned int i = 0; i < size; i++)
    {
        array[i] = UINT_MAX;
    }

    return array;
}

void print_int_array(int *array, int size)
{

    for (int i = 0; i < size; i++)
    {

        printf("%d | ", array[i]);
    }

    printf("\n");
}

void print_uint_array(unsigned int *array, unsigned int size)
{

    for (unsigned int i = 0; i < size; i++)
    {

        printf("%d | ", array[i]);
    }

    printf("\n");
}

void print_float_array(float* array, unsigned int size){

    for(unsigned int i=0; i<size; i++){

        printf("%f | ", array[i]);
    
    }

    printf("\n");

}

int is_move_null(struct move_t move){

    return (move.queen_src == MOVE_NULL.queen_src)
            && (move.queen_dst == MOVE_NULL.queen_dst)
            && (move.arrow_dst == MOVE_NULL.arrow_dst);

}

