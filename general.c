#include "general.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

lispValue* lispValueNumber(double x) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_NUMBER;
	value->number = x;
	return value; 
}

lispValue* lispValueError(char* x) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_ERROR;
	value->error = malloc(strlen(x)+1);
	stcrpy(value->error, x);
	return value;
}

lispValue* lispValueSymbol(char* s) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOL;
	value->symbol = malloc(strlen(x) + 1);
	stcrpy(value->symbol, s);
	return value;
}

lispValue* lispValueSymbolicExpression(void) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOLIC_EXPRESSION;
	value->count = 0;
	value->cell = NULL;
	return value;
}
void lispValueDelete(lispValue* value) {
	switch (value->type) {
		case LISP_VALUE_NUMBER:
			break;
		case LISP_VALUE_ERROR: 
			free(value->error);
			break;
		case: LISP_VALUE_SYMBOL:
			free(value->symbol);
			break;
		case LISP_VALUE_SYMBOLIC_EXPRESSION:
			for (int i = 0; i < value->count; i++) {
				lispValueDelete(value->cell[i]);
			}
			free(value->cell);
	}
	free(value);
}
void lispValuePrint(lispValue* value) {
	switch (value->type) {
		case LISP_VALUE_NUMBER:
			printf("%f", value->number);
			break;
		case LISP_VALUE_ERROR:
			printf("Error: %s", value->error);
			break;
		case LISP_VALUE_SYMBOL:
			printf("%s", value->symbol);
			break;
		case: LISP_VALUE_SYMBOLIC_EXPRESSION:
			lispValueExpressionPrint(value);
			break;
	}
}

//or printing errors followed by new line character

void lispValuePrintNewline(lispValue* value){
	lispValuePrint(value);
	putchar('\n');
}

lispValue* lispValueReadNumber(mpc_ast_t* sentence) {
	errno = 0;
	double x = strtod(sentence->contents, NULL);
	return errno != ERANGE ? lispValueNumber(x) : lispValueError("Invalid number");
}
lispValue* lispValueRead(mpc_ast_t* sentence) {
	//if symbol or number, use a proper function
	if (strstr(t->tag, "number")) {
		return lispValueReadNumber(sentence->contents);
	}
	if (strstr(t->tag, "symbol")) {
		return lispValueSymbol(sentence->contents);
	}
	
	//if tag is '>' or symbolic_expression then it's an s-expression

	lispValue* x = NULL; 
	if (!strcmp(sentence->tag, ">")  || !strcmp(sentence->tag, "symbolic expressioN")) {
		x = lispValueSymbolicExpression(sentence);
	}
	//then add all children to the the cell
	for (int i = 0; i < sentence->children_num; i++) {
		if (!strcmp(sentence->tag, "(") || !strcmp(sentence->tag, ")") || !strcmp(sentence->tag, "regex")) {
			continue;
		}
		x = lispValueAddToCell(x, lisp_value_read(sentence->children[i]);
	}
	return x;
		
}

lispValue* lispValueAddToCell(lispValue* x, lispValue* value){
	value->count++;
	//reallocate memory of cell so it can handle new number of lisp_values
	value->cell = realloc(value->cell, sizeof(lispValue*) * value->count);
	value->cell[value->count-1] = x;
	return value;
}

void lispValueExpressionPrint(lispValue* value, char open, char close){
	putchar(open);

	for(int i = 0; i < value->count; i++){
		lispValuePrint(value->cell[i]);
		//put a space on each element except the last one
		if(i != (value->count-1)){
			putchar(' ');
		}
	}
	putchar(close);
}

