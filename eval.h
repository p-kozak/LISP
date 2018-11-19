#ifndef eval_h
#define eval_h

#include "mpc.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

lisp_value eval(mpc_ast_t* sentence);
lisp_value eval_operator(lisp_value current_sum, char* op, lisp_value next_number);
int number_of_nodes(mpc_ast_t*sentence);

#endif
