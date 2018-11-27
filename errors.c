#include "errors.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

lisp_value lisp_value_number(double x) {
	lisp_value v;
	v.type = LISP_VALUE_NUMBER;
	v.number = x;
	return v; 
}

lisp_value lisp_value_error(int x) {
	lisp_value v;
	v.type = LISP_VALUE_ERROR;
	v.error = x;
	return v;
}

//pritnting errors
void print_lisp_value(lisp_value v) {
	switch (v.type) {
		case LISP_VALUE_NUMBER:
			printf("%f", v.number);
			break;
		case LISP_VALUE_ERROR:
			if (v.error == LISP_VALUE_DIVIDE_ZERO) {
				printf("ERROR: You attempted division by 0!");
			}
			if (v.error == LISP_VALUE_BAD_OPERATOR) {
				printf("ERROR: Invalid operator!");
			}
			if (v.error == LISP_VALUE_BAD_NUMBER) {
				printf("ERROR: Invalid Number!");
			}
			break;
	}
}

//or printing errors followed by new line character

void print_lisp_value_newline(lisp_value v){
	print_lisp_value(v);
	putchar('\n');
}