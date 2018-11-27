#include "eval.h"
#include "errors.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
long eval(mpc_ast_t* sentence) {
	
	This is a structure which we get after parsing the input. Might be helpful to understand what's going on in the function evaluating it
	typedef struct mpc_ast_t {
	char* tag;
	char* contents;
	mpc_state_t state;
	int children_num;
	struct mpc_ast_t** children;
	} mpc_ast_t; 

	//if the sentence is a number, return the number
	if (strstr(sentence->tag, "number")) {
		return atoi(sentence->contents); // The contents are always chars so we convert it to integer
	}
	//If it's not a number, we are looking for a operator which is always a second child of the expression(the first one is regular expression mark, opening parentheeses '('	)
	char* operator = sentence->children[1]->contents;
	//if there is - and onlt 1 numner return -number
	if (!strcmp("-", operator) && !strstr(sentence->children[3]->tag, "expression")) {
		return  -1 * atoi(sentence->children[2]->contents);
	}
	//the third child can be either number or another expression, so we recursively call eval on it

	long sum = eval(sentence->children[2]);
	//iterate through remaining n children starting from position 3 and combine everything
	int i = 3;
	while (strstr(sentence->children[i]->tag, "expression")) {
		sum = eval_operator(sum, operator, eval(sentence->children[i]));
		i++;
	}
	return sum;

}
*/

//eval for lisp_value

lisp_value eval(mpc_ast_t* sentence) {
	if (strstr(sentence->tag, "number")) {
		//check for error in coversion
		errno = 0;
		double x = strtol(sentence->contents, NULL, 10); //converts initial part of the string to long int - in our case, the whole expression
		return errno != ERANGE ? lisp_value_number(x) : lisp_value_error(LISP_VALUE_BAD_NUMBER);
	}
	//If it's not a number, we are looking for a operator which is always a second child of the expression(the first one is regular expression mark, opening parentheeses '('	)
	char* operator = sentence->children[1]->contents;
	//if there is - and onlt 1 numner return -number
	if (!strcmp("-", operator) && !strstr(sentence->children[3]->tag, "expression")) {
		double y = strtol(sentence->children[2]->contents, NULL, 10);
		return errno != ERANGE ? lisp_value_number(-1 * y) : lisp_value_error(LISP_VALUE_BAD_NUMBER);
	}

	lisp_value sum = eval(sentence->children[2]);
	//iterate through remaining n children starting from position 3 and combine everything
	int i = 3;
	while (strstr(sentence->children[i]->tag, "expression")) {
		sum = eval_operator(sum, operator, eval(sentence->children[i]));
		i++;
	}
	return sum;

}

/*long eval_operator(long current_sum, char* op, long next_number) {
	if (!strcmp("+", op))
		return current_sum + next_number;
	if (!strcmp("-", op))
		return current_sum - next_number;
	if (!strcmp("*", op))
		return current_sum * next_number;
	if (!strcmp("/", op))
		return current_sum / next_number;
	if (!strcmp("%", op))
		return current_sum % next_number;
	if (!strcmp("min", op))
		return current_sum < next_number ? current_sum : next_number;
	if (!strcmp("max", op))
		return current_sum > next_number ? current_sum : next_number;
	if (!strcmp("^", op))
		return pow(current_sum, next_number);

	return 0;

} */

//eval version able to handle lisp_value structure

lisp_value eval_operator(lisp_value current_value, char* operator, lisp_value new_value) {
	//if either of the values is a known error, return it
	if (current_value.type == LISP_VALUE_ERROR)
		return current_value;
	if (new_value.type == LISP_VALUE_ERROR)
		return new_value;

	//otherwise, do magic... maths, I mean

	if (!strcmp("+", operator))
		return lisp_value_number(current_value.number + new_value.number);
	if (!strcmp("-", operator))
		return lisp_value_number(current_value.number - new_value.number);
	if (!strcmp("*", operator))
		return lisp_value_number(current_value.number * new_value.number);;
	if (!strcmp("/", operator))
		return new_value.number == 0 ? lisp_value_error(LISP_VALUE_DIVIDE_ZERO): lisp_value_number(current_value.number/new_value.number);
	//if (!strcmp("%", operator))
		//return lisp_value_number(current_value.number % new_value.number);;
	if (!strcmp("min", operator))
		return current_value.number < new_value.number ? current_value : new_value;
	if (!strcmp("max", operator))
		return current_value.number > new_value.number ? current_value : new_value;
	if (!strcmp("^", operator))
		return lisp_value_number(pow(current_value.number,new_value.number));

	//if none of the tests pases
	return lisp_value_error(LISP_VALUE_BAD_OPERATOR);
}

int number_of_nodes(mpc_ast_t* sentence) {
	int sum_of_the_node = 0;
	int sum_of_internal_nodes = 0;
	int i = 2;

	if (strstr(sentence->tag, "number")) {
		sum_of_the_node++;
		return sum_of_the_node;
	}
	else {
		while (strstr(sentence->children[i]->tag, "expression")) {
			sum_of_internal_nodes = number_of_nodes(sentence->children[i]);
			sum_of_the_node += sum_of_internal_nodes;
			i++;
		}

	}
	return sum_of_the_node;
}