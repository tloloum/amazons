#ifndef _TEST_TOOLS_H_
#define _TEST_TOOLS_H_

void execute_test_function(int* (*f)(), char* f_name);

int * make_result(int nb_tests, int passed);

#endif // _TEST_TOOLS_H_