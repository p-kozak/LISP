#ifndef general_h
#define general_h

#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LISP_ASSERT(argument, condition, error){ \
				if(!(condition)) { \
					lispValueDelete(argument); \
					return lispValueError(error);\
				}\
			}

//lispValue an lispEnvorinment are cyclic dependant on each other, therefore this forward declaration
typedef struct lispValue lispValue;
typedef struct lispEnvironment lispEnvironment;

//Predefined pointer to a function lispBuiltin. It takes pointers to lispEnvironemnt and lispValue, returns pointer to lispValue.
//This is quite convoluted syntax
typedef lispValue*(*lispBuiltIn)(lispEnvironment*, lispValue*);

struct lispValue{
	int type;
	double number;
	//Error and symbol types can have some string data
	char* error;
	char* symbol;
	//The function pointer:
	lispBuiltIn function;
	//Counter and a pointer to list of another symbolic expressions
	int count;
	struct lispValue** cell;
};
//This structure is supposed to specify relations between names and values of the variables
struct lispEnvironment{
	int count;
	char** symbols;
	lispValue** values;

};
//Below there is a function pointer.
//lispBuiltIn is a function which takes lispEnvironemnt and lispValue and return lispValue. Difficult and messy syntax
/* Create Enumeration of Possible lispValue Types */
enum {LISP_VALUE_NUMBER, LISP_VALUE_ERROR, LISP_VALUE_SYMBOL, LISP_VALUE_FUNCTION,LISP_VALUE_SYMBOLIC_EXPRESSION, LISP_VALUE_QUOTED_EXPRESSION};

//Create enumeration of possible error types now redundant as errors will soon be strings - gives more flexibility
//enum{ LISP_VALUE_DIVIDE_ZERO, LISP_VALUE_BAD_OPERATOR, LISP_VALUE_BAD_NUMBER};

lispValue* lispValueNumber(double x);
lispValue* lispValueError(char* x);
lispValue* lispValueSymbol(char* s);
lispValue* lispValueSymbolicExpression(void);
lispValue* lispValueQuotedExpression(void);
lispValue* lispValueFunction(lispBuiltIn function);
void lispValueDelete(lispValue* value);
void lispValuePrint(lispValue* value);
void lispValuePrintNewline(lispValue* value);

lispValue* lispValueReadNumber(mpc_ast_t* sentence);
lispValue* lispValueRead(mpc_ast_t* sentence);
lispValue* lispValueAddToCell(lispValue* value, lispValue* x);
lispValue* lispValueTake(lispValue* value, int i);
lispValue* lispValuePop(lispValue* value, int i);

lispValue* lispValueBuiltInHead(lispEnvironment* environment, lispValue* value);
lispValue* lispValueBuiltInTail(lispEnvironment* environment, lispValue* value);
lispValue* lispValueBuiltInList(lispEnvironment* environment, lispValue* value);
lispValue* lispValueBuiltInJoin(lispEnvironment* environment, lispValue* value);
lispValue* lispValueJoin(lispValue* value, lispValue* toAdd);
lispValue* lispValueBuiltInLen(lispEnvironment* environment, lispValue* value);
lispValue* lispValueCopy(lispValue* value);

lispEnvironment* lispEnvironmentNew(void);
void lispEnvironmentDelete(lispEnvironment* environement);
lispValue* lispEnvironmentGet(lispEnvironment* environment, lispValue* value);
void lispEnvironmentPut(lispEnvironment* environment, lispValue* symbolDummy, lispValue* functionDummy);


void lispValueExpressionPrint(lispValue* value, char open, char close);





#endif
