#ifndef general_h
#define general_h

#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct lispValue{
	int type;
	double number;
	//Error and symbol types can have some string data
	char* error;
	char* symbol;
	//Counter a pointer to list of another symbolic expressions
	int count;
	struct lispValue** cell;
} lispValue;

/* Create Enumeration of Possible lispValue Types */
enum {LISP_VALUE_NUMBER, LISP_VALUE_ERROR, LISP_VALUE_SYMBOL, LISP_VALUE_SYMBOLIC_EXPRESSION, LISP_VALUE_QUOTED_EXPRESSION};

//Create enumeration of possible error types now redundant as errors will soon be strings - gives more flexibility
//enum{ LISP_VALUE_DIVIDE_ZERO, LISP_VALUE_BAD_OPERATOR, LISP_VALUE_BAD_NUMBER};

lispValue* lispValueNumber(double x);
lispValue* lispValueError(char* x);
lispValue* lispValueSymbol(char* s);
lispValue* lispValueSymbolicExpression(void);
lispValue* lispValueQuotedExpression(void);
void lispValueDelete(lispValue* value);
void lispValuePrint(lispValue* value);
void lispValuePrintNewline(lispValue* value);
lispValue* lispValueReadNumber(mpc_ast_t* sentence);
lispValue* lispValueRead(mpc_ast_t* sentence);
lispValue* lispValueAddToCell(lispValue* value, lispValue* x);
lispValue* lispValueTake(lispValue* value, int i);
lispValue* lispValuePop(lispValue* value, int i);
lispValue* lispValueBuiltInHead(lispValue* value);
lispValue* lispValueBuiltInTail(lispValue* value);
lispValue* lispValueBuiltInList(lispValue* value);
lispValue* lispValueBuiltInEval(lispValue* value);
void lispValueExpressionPrint(lispValue* value, char open, char close);





#endif
