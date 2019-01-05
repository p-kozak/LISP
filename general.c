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
	strcpy(value->error, x);
	return value;
}

lispValue* lispValueSymbol(char* s) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOL;
	value->symbol = malloc(strlen(s) + 1);
	strcpy(value->symbol, s);
	return value;
}

lispValue* lispValueSymbolicExpression(void) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_SYMBOLIC_EXPRESSION;
	value->count = 0;
	value->cell = NULL;
	return value;
}

lispValue* lispValueQuotedExpression(void){
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_QUOTED_EXPRESSION;
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
			//for quoted and symbolic expressions the same deletion code will be executed as they
			//are essentialy the same structure
		case LISP_VALUE_QUOTED_EXPRESSION:
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
		case LISP_VALUE_QUOTED_EXPRESSION:
			lispValueExpressionPrint(value, '{','}');
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
	lispValue* x = NULL;

	//if tag is '>' or sym_expression then it's an s-expression
	if (!strcmp(sentence->tag, ">")  || strcmp(sentence->tag, "sym_expression")) {
		x = lispValueSymbolicExpression();
	}
	if(strstr(sentence->tag, "quoted_expression")){
		x = lispValueQuotedExpression();
	}
	//then add all children to the the cell
	int i;
	for(i = 0; i < sentence->children_num; i++) {
		if (!strcmp(sentence->children[i]->contents,"(") || !strcmp(sentence->children[i]->contents, ")") || !strcmp(sentence->children[i]->tag, "regex")){
			continue;
			//here was the nasty bug which took me 8h to debug. i simply called wrong addresses in  above if...
		}
		if (!strcmp(sentence->children[i]->contents,"{") || !strcmp(sentence->children[i]->contents, "}")){
			continue;
		}
		x = lispValueAddToCell(x, lispValueRead(sentence->children[i]));

	}
	return x;
}

lispValue* lispValueAddToCell(lispValue* value, lispValue* x){

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

lispValue* lispValueBuiltInHead(lispValue* value){
	//head - takes quoted expression and return only the first element as quoted expression

	//check for errors
	LISP_ASSERT(value, value->count ==1,"Function 'head' was passed too many arguments");
	LISP_ASSERT(value, value->cell[0]->type==LISP_VALUE_QUOTED_EXPRESSION,"Function 'head' was passed an invalid type");
	LISP_ASSERT(value, value->cell[0]->count != 0,"Function 'head' was passed an empty quoted expression {}");

	//otherwise, take the first argument
	lispValue* newValue = lispValueTake(value, 0);
	//delete all non-head elements and return
	while(newValue->count > 1){
		lispValueDelete(lispValuePop(newValue,1));
	}
	return newValue;
}

lispValue* lispValueBuiltInTail(lispValue* value){
	//tail - takes quoted expression and return a quoted expression with first element removed

	LISP_ASSERT(value, value->count ==1,"Function 'tail' was passed too many arguments");
	LISP_ASSERT(value, value->cell[0]->type==LISP_VALUE_QUOTED_EXPRESSION,"Function 'tail' was passed an invalid type");
	LISP_ASSERT(value, value->cell[0]->count != 0,"Function 'tail' was passed an empty quoted expression {}");

	//take the value of Quoted expression
	lispValue* newValue = lispValueTake(value,0);
	//dlete the first element
	lispValueDelete(lispValuePop(newValue,0));
	return newValue;
}

lispValue* lispValueBuiltInList(lispValue* value){
	//list - take one or more arguments and return new Quoted Expression containing them

	value->type=LISP_VALUE_QUOTED_EXPRESSION;
	return value;
}


lispValue* lispValueJoin(lispValue* value, lispValue* toAdd){
	//repetively pop the first element of toAdd number and add it to the cell of target lisp value
	while(toAdd->count){
		value = lispValueAddToCell(value, lispValuePop(toAdd,0));
	}
	//new delete toAdd and return value
	lispValueDelete(toAdd);
	return value;
}

lispValue* lispValueBuiltInJoin(lispValue* value){
	//first, check if all arguments are q-expressions
	for(int i=0; i < value->count; i++){
		LISP_ASSERT(value, value->cell[i]->type == LISP_VALUE_QUOTED_EXPRESSION, "Function 'join' was passed incorrect type");
	}
	// now recursivelly join all other arguments
	lispValue* newValue = lispValuePop(value,0);

	while(value->count){
		newValue = lispValueJoin(newValue, lispValuePop(value, 0));
	}

	lispValueDelete(value);
	return newValue;
}