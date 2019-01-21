#ifndef eval_h
#define eval_h

#include "mpc.h"
#include "general.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//lispValue eval(mpc_ast_t* sentence);
//lispValue evalOperator(lispValue current_sum, char* op, lispValue next_number);
lispValue* lispValueEvalSymbolicExpression(lispEnvironment* environment, lispValue* value);
lispValue* lispValueEval(lispEnvironment* environment, lispValue* value);
int numberOfNodes(mpc_ast_t*sentence);
lispValue* lispValueBuiltInOperator(lispValue * value, char* op);
lispValue* lispValueBuiltInEval(lispValue* value);
lispValue* lispValueBuiltIn(lispValue* value, char* function);

#endif
