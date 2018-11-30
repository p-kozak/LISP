#include "errors.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

lisp_value* lisp_value_number(double x) {
	lisp_value* v = malloc(sizeof(lisp_value));
	v->type = LISP_VALUE_NUMBER;
	v->number = x;
	return v; 
}

lisp_value* lisp_value_error(char* x) {
	lisp_value* v = malloc(sizeof(lisp_value));
	v->type = LISP_VALUE_ERROR;
	v->error = malloc(strlen(x)+1);
	stcrpy(v->error, x)
	return v;
}

lisp_value* lisp_value_symbol(char* s) {
	lisp_value* v = malloc(sizeof(lisp_value));
	v->type = LISP_VALUE_SYMBOL;
	v->symbol = malloc(strlen(x) + 1);
	stcrpy(v->symbol, s);
	return v;
}

lisp_value* lisp_value_symbolic_expression(void) {
	lisp_value* v = malloc(sizeof(lisp_value));
	v->type = LISP_VALUE_SYMBOLIC_EXPRESSION;
	v->count = 0;
	v->cell = NULL;
	return v;
}
void lisp_value_delete(lisp_value* v) {
	switch (v->type) {
		case LISP_VALUE_NUMBER:
			break;
		case LISP_VALUE_ERROR: 
			free(v->error);
			break;
		case: LISP_VALUE_SYMBOL:
			free(v->symbol);
			break;
		case LISP_VALUE_SYMBOLIC_EXPRESSION:
			for (int i = 0; i < v->count; i++) {
				lisp_value_delete(v->cell[i]);
			}
			free(cell);
	}
	free(v);
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

lisp_value* lisp_value_read_number(mpc_ast_t* sentence) {
	errno = 0;
	x = strtod(sentence->contents, NULL);
	return errno != ERANGE ? lisp_value_number(x) : lisp_value_error("Invalid number");
}
lisp_value* lisp_value_read(mpc_ast_t* sentence) {
	//if symbol or number, use a proper function
	if (strstr(t->tag, "number")) {
		return lisp_value_read_number(sentence->contents);
	}
	if (strstr(t->tag, "symbol")) {
		return lisp_value_symbol
	}
	
}