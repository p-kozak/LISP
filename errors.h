#ifndef errors_h
#define errors_h

#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	int type;
	double number;
	int error;
} lisp_value;

/* Create Enumeration of Possible lval Types */
enum { LISP_VALUE_NUMBER, LISP_VALUE_ERROR };

//Create enumeration of possible error types
enum{ LISP_VALUE_DIVIDE_ZERO, LISP_VALUE_BAD_OPERATOR, LISP_VALUE_BAD_NUMBER};

lisp_value lisp_value_number(double x);
lisp_value lisp_value_error(int x);
void print_lisp_value(lisp_value v);
void print_lisp_value_newline(lisp_value v);


#endif
