#include "eval.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//eval for lispValue



int numberOfNodes(mpc_ast_t* sentence) {
	int sum_of_the_node = 0;
	int sum_of_internal_nodes = 0;
	int i = 2;

	if (strstr(sentence->tag, "number")) {
		sum_of_the_node++;
		return sum_of_the_node;
	}
	else {
		while (strstr(sentence->children[i]->tag, "expression")) {
			sum_of_internal_nodes = numberOfNodes(sentence->children[i]);
			sum_of_the_node += sum_of_internal_nodes;
			i++;
		}

	}
	return sum_of_the_node;
}

lispValue* lispValueEvalSymbolicExpression(lispEnvironment* environment, lispValue* value){
	//first evaluate all children
	int i;
	for(i = 0; i < value->count; i++){
		value->cell[i]= lispValueEval(environment, value->cell[i]);
	}

	//check for errors
	for(i = 0; i < value->count; i++){
		if(value->cell[i]->type == LISP_VALUE_ERROR){
			return lispValueTake(value,i);
		}
	}

	//Take care of empty expressions
	if(value->count == 0){
		return value;
	}

	//And single expressions
	if(value->count == 1){
		return lispValueTake(value,0);
	}

	//Make sure thay first element is a function after evaluation
	lispValue* first = lispValuePop(value,0);
	if(first->type != LISP_VALUE_FUNCTION){
		lispValueDelete(value);
		lispValueDelete(first);
		return lispValueError("Symbolic expression doesn't start with function");
	}

	//print result
	lispValue* result = first->function(environment, value);
	lispValueDelete(first);
	return result;
}


lispValue* lispValueEval(lispEnvironment* environment, lispValue* value){
	if (value->type == LISP_VALUE_SYMBOL) {
		lispValue* newValue = lispEnvironmentGet(environment, value);
		lispValueDelete(value);
		return newValue;
	}
	if(value->type == LISP_VALUE_SYMBOLIC_EXPRESSION ){
		return lispValueEvalSymbolicExpression(environment, value);
	}
	//other types, just return
	return value;
}

lispValue* lispValueBuiltInOperator(lispEnvironment* environment, lispValue * value, char* op){
	//check if all arguments are numbers
	int i;
	for(i = 0; i < value->count; i++){
		if (value->cell[i]->type != LISP_VALUE_NUMBER) {
			printf("the bad type of cell[%d] is %d and it's %s \n", i, value->cell[i]->type, value->cell[i]->symbol);
			puts(value->cell[i]->symbol);
			//puts(value->cell[i]->number);
			lispValueDelete(value);
			return lispValueError("Arguments have to be numbers");
		}
	}

	//pop the first element(regex)
	lispValue* x = lispValuePop(value, 0);
	//if there are no addtional elements(as we just extracted first and only element of value) and operator is -, just negate it
	if(!strcmp(op, "-") && !value->count){
		x->number = -x->number;
	}

	//unless there are no arguments remaining
	while(value->count>0){
		//pop the next element
		lispValue* y = lispValuePop(value,0);

		if(!strcmp("+",op)){
			x->number += y->number;
		}
		if(!strcmp("-",op)){
			x->number -= y->number;
		}
		if(!strcmp("*",op)){
			x->number *= y->number;
		}
		if(!strcmp("/",op)){
			if(y->number == 0){
				lispValueDelete(x);
				lispValueDelete(y);
				x = lispValueError("You cannot divide by zero");
				break;
			}else{
				x->number /= y->number;
			}
		}
		lispValueDelete(y);
	}
	lispValueDelete(value);
	return x;
}

//Here are the indiviudal built in maths functions

lispValue* builtInAdd(lispEnvironment* environment, lispValue* value) {
	return lispValueBuiltInOperator(environment, value, "+");
}
lispValue* builtInSub(lispEnvironment* environment, lispValue* value) {
	return lispValueBuiltInOperator(environment, value, "-");
}

lispValue* builtInMul(lispEnvironment* environment, lispValue* value) {
	return lispValueBuiltInOperator(environment, value, "*");
}

lispValue* builtInDiv(lispEnvironment* environment, lispValue* value) {
	return lispValueBuiltInOperator(environment, value, "/");
}

lispValue* lispValueBuiltInEval(lispEnvironment* environment, lispValue* value){
	//eval - takes quoted expression and evaluates it as it was a symbolic expression
	LISP_ASSERT(value, value->count==1, "Function 'eval' was passed too many arguments");
	LISP_ASSERT(value, value->cell[0]->type == LISP_VALUE_QUOTED_EXPRESSION, "Function 'eval' was passed an invalid type");

	//take first element, change it's type to symbolic expression and evaluates
	lispValue* newValue = lispValueTake(value,0);
	newValue->type == LISP_VALUE_SYMBOLIC_EXPRESSION;
	return lispValueEval(environment, newValue);
}


lispValue* lispValueBuiltIn(lispEnvironment* environment, lispValue* value, char* function){
	if(!strcmp("list", function)){
		return lispValueBuiltInList(environment, value);
	}
	if(!strcmp("head", function)){
		return lispValueBuiltInHead(environment, value);
	}
	if(!strcmp("tail", function)){
		return lispValueBuiltInTail(environment, value);
	}
	if(!strcmp("join", function)){
		return lispValueBuiltInJoin(environment, value);
	}
	if(!strcmp("eval", function)){
		return lispValueBuiltInEval(environment, value);
	}
	/*if (!strcmp("len", function)) {
		return lispValueBuiltInLen(value);
	}*/
	if(strstr("+-/*", function)){
		return lispValueBuiltInOperator(environment, value, function);
	}
	lispValueDelete(value);
	return lispValueError("Unknown function");
}

lispValue* builtInDef(lispEnvironment* environment, lispValue* value){
	LISP_ASSERT(value, value->cell[0]->type == LISP_VALUE_QUOTED_EXPRESSION, "Function def was passed an incorrect type");
	//The first element of the input is the list of symbols
	lispValue* symbols = value->cell[0];

	//Make sure that all elements of the list above are symbols
	for(int i=0; i < symbols->cell[i]->count; i++){
		LISP_ASSERT(value, symbols->cell[i]->type == LISP_VALUE_SYMBOL, "Non symbols cannot be defined");
	}
	//Make sure that there are as ,amy symbols as arguments
	LISP_ASSERT(value, symbols->count == value->count-1, "Function 'def' cannot define: incorrect number of values");

	for(int i = 0; i < symbols->count; i++){
			lispEnvironmentPut(environment, symbols->cell[i], value->cell[i+1]);
	}
	lispValueDelete(value);
	return lispValueSymbolicExpression();
}


void lispEnvironmentAddBuiltIn(lispEnvironment* environment, char* name, lispBuiltIn function) {
	//This function adds another functions to the environment so they can be executed on the variables from this environment
	/*This works as lispBuiltIn is defined as
	lispValue*(*lispBuiltIn)(lispEnvironment*, lispValue*)
	this means it can take any function taking the same arguments and returning pointer to lispValue */
	lispValue* symbolDummy = lispValueSymbol(name);
	lispValue* functionDummy = lispValueFunction(function);
	lispEnvironmentPut(environment, symbolDummy, functionDummy);
	lispValueDelete(symbolDummy);
	lispValueDelete(functionDummy);
	return;
}
void lispEnvironmentAddBuiltIns(lispEnvironment* environment) {
	//Q-expressions lists functions
	lispEnvironmentAddBuiltIn(environment, "list", lispValueBuiltInList);
	lispEnvironmentAddBuiltIn(environment, "head", lispValueBuiltInHead);
	lispEnvironmentAddBuiltIn(environment, "tail", lispValueBuiltInTail);
	lispEnvironmentAddBuiltIn(environment, "join", lispValueBuiltInJoin);
	lispEnvironmentAddBuiltIn(environment, "eval", lispValueBuiltInEval);

	//Maths functions
	lispEnvironmentAddBuiltIn(environment, "+", builtInAdd);
	lispEnvironmentAddBuiltIn(environment, "-", builtInSub);
	lispEnvironmentAddBuiltIn(environment, "/", builtInDiv);
	lispEnvironmentAddBuiltIn(environment, "*", builtInMul);

	//other functions
	lispEnvironmentAddBuiltIn(environment,"def", builtInDef);
	return;
}
