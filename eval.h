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
lispValue* lispValueBuiltInOperator(lispEnvironment* environment, lispValue * value, char* op);
lispValue* lispValueBuiltInEval(lispEnvironment* environment, lispValue* value);
lispValue* lispValueBuiltIn(lispEnvironment* environment, lispValue* value, char* function);
lispValue* builtInAdd(lispEnvironment* environment, lispValue* value);
lispValue* builtInSub(lispEnvironment* environment, lispValue* value);
lispValue* builtInMul(lispEnvironment* environment, lispValue* value);
lispValue* builtInDiv(lispEnvironment* environment, lispValue* value);
lispValue* builtInDef(lispEnvironment* environment, lispValue* value);


void lispEnvironmentAddBuiltIn(lispEnvironment* environment, char* name, lispBuiltIn function);
void lispEnvironmentAddBuiltIns(lispEnvironment* environment);


#endif
