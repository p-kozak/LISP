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

/*This functions supports variable lists of arguments. It can be used to indicate errors. Works in a similar manner to printf*/
lispValue* lispValueError(char* error, ...) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_ERROR;
	/*c stdarg.h variable argument list struct type. Initialisation*/
	va_list va; 
	va_start(va, error);

	/*Allcoate memor for error. Hopefully errors won't be longer than 1024*/
	value->error = malloc(1024);

	/*Printing the error string, max 1023 characters
	Write formatted data from variable argument list to sized buffer*/
	vsnprintf(value->error, 1023, error, va);
	/*Reallocate error to number of bits actually used*/
	value->error = realloc(value->error, strlen(value->error) + 1);
	/*Free the va list*/
	va_end(va);
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

lispValue* lispValueFunction(lispBuiltIn function) {
	lispValue* value = malloc(sizeof(lispValue));
	value->type = LISP_VALUE_FUNCTION;
	value->count = 0;
	value->function = function;
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
		case LISP_VALUE_FUNCTION:
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
		case LISP_VALUE_FUNCTION:
			printf("<function>"); //just print a generic string
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
	return errno != ERANGE ? lispValueNumber(x) : lispValueError("Invalid number. Got %s", sentence->contents);
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

lispValue* lispValueBuiltInHead(lispEnvironment* environment, lispValue* value){
	//head - takes quoted expression and return only the first element as quoted expression

	//check for errors
	LISP_ASSERT(value, value->count ==1,"Function 'head' was passed too many arguments. Got %i, expected 1", value->count);
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

lispValue* lispValueBuiltInTail(lispEnvironment* environment, lispValue* value){
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

lispValue* lispValueBuiltInList(lispEnvironment* environment, lispValue* value){
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

lispValue* lispValueBuiltInJoin(lispEnvironment* environment, lispValue* value){
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


//lispValue* lispValueBuiltInLen(lispValue* value);

lispValue* lispValueCopy(lispValue* value) {
	lispValue* newValue = malloc(sizeof(lispValue));
	newValue->type = value->type;
	switch (newValue->type) {
	case LISP_VALUE_NUMBER:
		newValue->number = value->number;
		break;
	case LISP_VALUE_FUNCTION:
		newValue->function = value->function;
		break;
	case LISP_VALUE_ERROR:
		newValue->error = malloc(strlen(value->error)+1);
		strcpy(newValue->error, value->error);
		break;
	case LISP_VALUE_SYMBOL:
		newValue->symbol = malloc(strlen(value->symbol) + 1);
		strcpy(newValue->symbol, value->symbol);
		break;
	case LISP_VALUE_SYMBOLIC_EXPRESSION:
		newValue->count = value->count;
		newValue->cell = malloc(sizeof(lispValue) * value->count);
		for (int i = 0; i < newValue->count; i++) {
			//recursive copy
			newValue->cell[i] = lispValueCopy(value->cell[i]);
		}
		break;
	case LISP_VALUE_QUOTED_EXPRESSION: 
		newValue->count = value->count;
		newValue->cell = malloc(sizeof(lispValue) * value->count);
		for (int i = 0; i < newValue->count; i++) {
			//recursive copy
			newValue->cell[i] = lispValueCopy(value->cell[i]);
		}
		break;

	}

	return newValue;
}


lispEnvironment* lispEnvironmentNew(void) {
	lispEnvironment* environment = malloc(sizeof(lispEnvironment));
	environment->count = 0;
	environment->symbols = NULL;
	environment->values = NULL;
	
	return environment;
}

void lispEnvironmentDelete(lispEnvironment* environment) {
	for (int i = 0; i < environment->count; i++) {
		lispValueDelete(environment->values[i]);
		free(environment->symbols[i]);
	}
	free(environment->symbols);
	free(environment->values);
	free(environment);
	return;
}

lispValue* lispEnvironmentGet(lispEnvironment * environment, lispValue * value){
	//this function checks if the variable of the same name already exists in the environment. If it does, it returns a copy of the lispValue from environment
	for (int i = 0; i < environment->count; i++) {
		if (!strcmp(environment->symbols[i], value->symbol)) {
			return lispValueCopy(environment->values[i]);
		}
	}
	//Otherwise, return an error
	return lispValueError("Unbound symbol %s", value->symbol);
}

void lispEnvironmentPut(lispEnvironment* environment, lispValue* symbolDummy, lispValue* functionDummy) {
	//Firstly, iterate over all elements of the environment to check if oldValue is already there
	for (int i = 0; i < environment->count; i++) {
		if (!strcmp(environment->symbols[i], symbolDummy->symbol)) {
			//If variable with the same symbol as oldValue is there, replace it with newValue
			lispValueDelete(environment->values[i]);
			environment->values[i] = lispValueCopy(functionDummy);
			return;
		}
	}
	//If oldValue does not exist in the environment, then reallocate memory and place it in the environment
	environment->count++;
	environment->values = realloc(environment->values, sizeof(lispValue)*environment->count);
	environment->symbols = realloc(environment->symbols, sizeof(char*)*environment->count);

	environment->values[environment->count - 1] = lispValueCopy(functionDummy);
	environment->symbols[environment->count - 1] = malloc(strlen(symbolDummy->symbol) + 1);
	strcpy(environment->symbols[environment->count - 1], symbolDummy->symbol);
	
	return;
}


char* lispValueReturnType(int type) {
	switch (type) {
	case LISP_VALUE_ERROR:
		return "Error";
	case LISP_VALUE_NUMBER:
		return "Number";
	case LISP_VALUE_FUNCTION:
		return "Function";
	case LISP_VALUE_QUOTED_EXPRESSION:
		return "Quoted expression";
	case LISP_VALUE_SYMBOL:
		return "SYMBOL";
	case LISP_VALUE_SYMBOLIC_EXPRESSION:
		return "Symbolic expression";
	default:
		return "Unknown type";
	}
}

