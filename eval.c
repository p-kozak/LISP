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

lispValue* lispValueEvalSymbolicExpression(lispEnvironment* lispValue* value){
	//first evaluate all children
	int i;
	for(i = 0; i < value->count; i++){
		value->cell[i]= lispValueEval(value->cell[i]);
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

	//Make sure thay first element is a symbol
	lispValue* first = lispValuePop(value,0);
	if(first->type != LISP_VALUE_SYMBOL){
		lispValueDelete(value);
		lispValueDelete(first);
		return lispValueError("Symbolic expression doesn't start with symbol");
	}

	//print result
	lispValue* result = lispValueBuiltIn(value, first->symbol);
	lispValueDelete(first);
	return result;
}


lispValue* lispValueEval(lispEnvironment* environment, lispValue* value){
	if (value->type == LISP_VALUE_SYMBOL) {
		lispValue* newValue = lispValueGet(environment, value);
		lispValueDelete(value);
		return newValue;
	}
	if(value->type == LISP_VALUE_SYMBOLIC_EXPRESSION ){
		return lispValueEvalSymbolicExpression(value);
	}
	//other types, just return
	return value;
}

lispValue* lispValueBuiltInOperator(lispValue * value, char* op){
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

lispValue* lispValueBuiltInEval(lispValue* value){
	//eval - takes quoted expression and evaluates it as it was a symbolic expression
	LISP_ASSERT(value, value->count==1, "Function 'eval' was passed too many arguments");
	LISP_ASSERT(value, value->cell[0]->type == LISP_VALUE_QUOTED_EXPRESSION, "Function 'eval' was passed an invalid type");

	//take first element, change it's type to symbolic expression and evaluates
	lispValue* newValue = lispValueTake(value,0);
	newValue->type == LISP_VALUE_SYMBOLIC_EXPRESSION;
	return lispValueEval(newValue);
}


lispValue* lispValueBuiltIn(lispValue* value, char* function){
	if(!strcmp("list", function)){
		return lispValueBuiltInList(value);
	}
	if(!strcmp("head", function)){
		return lispValueBuiltInHead(value);
	}
	if(!strcmp("tail", function)){
		return lispValueBuiltInTail(value);
	}
	if(!strcmp("join", function)){
		return lispValueBuiltInJoin(value);
	}
	if(!strcmp("eval", function)){
		return lispValueBuiltInEval(value);
	}
	/*if (!strcmp("len", function)) {
		return lispValueBuiltInLen(value);
	}*/
	if(strstr("+-/*", function)){
		return lispValueBuiltInOperator(value, function);
	}
	lispValueDelete(value);
	return lispValueError("Unknown function");
}
