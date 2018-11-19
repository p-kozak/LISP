#ifndef eval_h
#define eval_h

#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long eval(mpc_ast_t* sentence);
long eval_operator(long current_sum, char* op, long next_number);
int number_of_nodes(mpc_ast_t*sentence);

#endif
