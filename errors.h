#ifndef errors_h
#define errors_h

#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct lisp_value{
	int type;
	double number;
	//Error and symbol types can have some string data
	char* error;
	char* symbol;
	//Counter a pointer to list of another symbolic expressions
	int count;
	struct lisp_expression** cell;
} lisp_value;

/* Create Enumeration of Possible lisp_value Types */
enum { LISP_VALUE_NUMBER, LISP_VALUE_ERROR, LISP_VALUE_SYMBOL, LISP_VALUE_SYMBOLIC_EXPRESSION};

//Create enumeration of possible error types now redundant as errors will soon be strings - gives more flexibility
//enum{ LISP_VALUE_DIVIDE_ZERO, LISP_VALUE_BAD_OPERATOR, LISP_VALUE_BAD_NUMBER};

lisp_value* lisp_value_number(double x);
lisp_value* lisp_value_error(char* x);
lisp_value* lisp_value_symbol(char* s);
lisp_value* lisp_value_symbolic_expression(void);
void lisp_value_delete(lisp_value* v);
void print_lisp_value(lisp_value v);
void print_lisp_value_newline(lisp_value v);



#endif
