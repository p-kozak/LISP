#include "general.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

lispValue* lispValueNumber(double x) {
	puts("Creating number");
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_NUMBER;
	value->number = x;
	return value;
}

lispValue* lispValueError(char* x) {
	puts("Creating error");
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_ERROR;
	value->error = malloc(strlen(x)+1);
	strcpy(value->error, x);
	return value;
}

lispValue* lispValueSymbol(char* s) {
	puts("Creating symbol");
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOL;
	value->symbol = malloc(strlen(s) + 1);
	strcpy(value->symbol, s);
	return value;
}

lispValue* lispValueSymbolicExpression(void) {
	puts("Creating symbolic expression");
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOLIC_EXPRESSION;
	value->count = 0;
	value->cell = NULL;
	return value;
}

void lispValueDelete(lispValue* value) {
	int i;
	switch (value->type) {
		case LISP_VALUE_NUMBER:
			break;
		case LISP_VALUE_ERROR:
			free(value->error);
			break;
		case LISP_VALUE_SYMBOL:
			free(value->symbol);
			break;
		case LISP_VALUE_SYMBOLIC_EXPRESSION:
			for (i = 0; i < value->count; i++) {
				lispValueDelete(value->cell[i]);
			}
			free(value->cell);
			break;
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
		case LISP_VALUE_SYMBOLIC_EXPRESSION:
			lispValueExpressionPrint(value,'(',')');
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
	if (strstr(sentence->tag, "number")) {
		return lispValueReadNumber(sentence);
	}
	if (strstr(sentence->tag, "symbol")) {
		return lispValueSymbol(sentence->contents);
	}
	//if tag is '>' or symbolic_expression then it's an s-expression

	lispValue* x = NULL;
	if (!strcmp(sentence->tag, ">")  || !strcmp(sentence->tag, "symbolic_expression")) {
		x = lispValueSymbolicExpression();
	}
	//then add all children to the the cell
	int i;
	for(i = 0; i < sentence->children_num; i++) {
		if (!strcmp(sentence->children[i]->contents, "(") || !strcmp(sentence->children[i]->contents, ")") || !strcmp(sentence->children[i]->tag, "regex")){
			continue;
			//here was the nasty bug which took me 8h to debug. i simply called wrong addresses in  above if...
		}
		x = lispValueAddToCell(x, lispValueRead(sentence->children[i]));
	}
	return x;
}

lispValue* lispValueAddToCell(lispValue* value, lispValue* x){
	puts("Adding to cell ");
	if(x->type == LISP_VALUE_NUMBER){
		printf("%f\n" , x->number);
	}else if(x->type == LISP_VALUE_SYMBOL){
		printf("%s\n", x->symbol);
	}else{
		puts("shit");
	}

	value->count++;
	//reallocate memory of cell so it can handle new number of lisp_values
	value->cell = realloc(value->cell, sizeof(lispValue*) * value->count);
	value->cell[value->count-1] = x;
	return value;
}

void lispValueExpressionPrint(lispValue* value, char open, char close){
	putchar(open);
	int i;
	for(i = 0; i < value->count; i++){
		lispValuePrint(value->cell[i]);
		//put a space on each element except the last one
		if(i != (value->count-1)){
			putchar(' ');
		}
	}
	putchar(close);
}

lispValue* lispValuePop(lispValue* value, int i) {
	//this functions extracts i-th element of the expression and shifts elements after i backwards
	lispValue* x = value->cell[i];

	//shift memory
	memmove(&value->cell[i], &value->cell[i+1], sizeof(lispValue*) * (value->count-i-1));

	//decrease the count as there is one less element
	value->count--;

	//reallocate the used memory
	value->cell = realloc(value->cell, sizeof(lispValue*) * value->count);

	return x;
}

lispValue* lispValueTake(lispValue* value, int i) {
	//this function takes i-th element, deletes the list and returns only this element
	lispValue* x = lispValuePop(value,i);
	lispValueDelete(value);
	return x;
}
