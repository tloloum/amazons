#include <stdio.h>
#include <stdlib.h>

void execute_test_function(int* (*f)(), char * f_name){

    int* results = (*f)();

    char* code_color = (results[0] == results[1] ? "\033[0;32m" : "\033[31m");

    printf("%s : %s%d/%d\033[0m tests passed\n", f_name, code_color, results[0], results[1]);

    free(results);

    return;

}

int* make_result(int nb_tests, int passed){

    int* result = malloc(2*sizeof(int));

    result[0]=passed;
    result[1]=nb_tests;

    return result;

}