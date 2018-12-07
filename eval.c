#include "eval.h"
#include "general.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//eval for lispValue

lispValue eval(mpc_ast_t* sentence) {
	if (strstr(sentence->tag, "number")) {
		//check for error in coversion
		errno = 0;
		double x = strtod(sentence->contents, NULL); //converts initial part of the string to long int - in our case, the whole expression
		return errno != ERANGE ? lispValueNumber(x) : lispValueError(LISP_VALUE_BAD_NUMBER);
	}
	//If it's not a number, we are looking for a operator which is always a second child of the expression(the first one is regular expression mark, opening parentheeses '('	)
	char* operator = sentence->children[1]->contents;
	//if there is - and onlt 1 numner return -number
	if (!strcmp("-", operator) && !strstr(sentence->children[3]->tag, "expression")) {
		double y = strtod(sentence->children[2]->contents, NULL);
		return errno != ERANGE ? lispValueNumber(-1 * y) : lispValueError(LISP_VALUE_BAD_NUMBER);
	}

	lispValue sum = eval(sentence->children[2]);
	//iterate through remaining n children starting from position 3 and combine everything
	int i = 3;
	while (strstr(sentence->children[i]->tag, "expression")) {
		sum = evalOperator(sum, operator, eval(sentence->children[i]));
		i++;
	}
	return sum;

}


lispValue evalOperator(lispValue current_value, char* operator, lispValue new_value) {
	//if either of the values is a known error, return it
	if (current_value.type == LISP_VALUE_ERROR)
		return current_value;
	if (new_value.type == LISP_VALUE_ERROR)
		return new_value;

	//otherwise, do magic... maths, I mean

	if (!strcmp("+", operator))
		return lispValueNumber(current_value.number + new_value.number);
	if (!strcmp("-", operator))
		return lispValueNumber(current_value.number - new_value.number);
	if (!strcmp("*", operator))
		return lispValueNumber(current_value.number * new_value.number);;
	if (!strcmp("/", operator))
		return new_value.number == 0 ? lispValueError(LISP_VALUE_DIVIDE_ZERO): lispValueNumber(current_value.number/new_value.number);
	//if (!strcmp("%", operator))
		//return lispValueNumber(current_value.number % new_value.number);;
	if (!strcmp("min", operator))
		return current_value.number < new_value.number ? current_value : new_value;
	if (!strcmp("max", operator))
		return current_value.number > new_value.number ? current_value : new_value;
	if (!strcmp("^", operator))
		return lispValueNumber(pow(current_value.number,new_value.number));

	//if none of the tests pases
	return lispValueError(LISP_VALUE_BAD_OPERATOR);
}

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

lispValue* lispValueEvalSymbolicExpression(lispValue* value){
	//first evaluate all children
	for(int i = 0; i < value->count; i++){
		value->cell[i]= lispValueEval(value->cell[i])
	}

	//check for errors
	for(int i = 0; i<value->count; i++){
		if(value>cell[i]->type == LISP_VALUE_ERROR){
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
	lispValue* first = lispValuePop(v,0);
	if(first->type != LISP_VALUE_SYMBOL){
		lispValueDelete(value);
		lispValueDelete(first);
	}
	return lispValueError("Symbolic expression doesn't start with symbol");

	//print result
	lispValue* result = lispValueBuiltInOperator(value, first->symbol);
	lispValueDelete(first);
	return result;
}


lispValue* lispValueEval(lispValue* value){
	//evaluate symbolic expressions
	if(value->type == LISP_VALUE_SYMBOLIC_EXPRESSION){
		return lispValueEvalSymbolicExpression(value);
	}
	//other types, just return
	return value;
}
