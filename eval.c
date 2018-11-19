#include "eval.h"
#include "mpc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long eval(mpc_ast_t* sentence) {
	/*
	This is a structure which we get after parsing the input. Might be helpful to understand what's going on in the function evaluating it
	typedef struct mpc_ast_t {
	char* tag;
	char* contents;
	mpc_state_t state;
	int children_num;
	struct mpc_ast_t** children;
	} mpc_ast_t;*/

	//if the sentence is a number, return the number
	if (strstr(sentence->tag, "number")) {
		return atoi(sentence->contents); // The contents are always chars so we convert it to integer
	}
	//If it's not a number, we are looking for a operator which is always a second child of the expresion(the first one is regular expresion mark, opening parentheeses '('	)
	char* operator = sentence->children[1]->contents;
	//if there is - and onlt 1 numner return -number
	if (!strcmp("-", operator) && !strstr(sentence->children[3]->tag, "expresion")) {
		return  -1 * atoi(sentence->children[2]->contents);
	}
	//the third child can be either number or another expresion, so we recursively call eval on it

	long sum = eval(sentence->children[2]);
	//iterate through remaining n children starting from position 3 and combine everything
	int i = 3;
	while (strstr(sentence->children[i]->tag, "expresion")) {
		sum = eval_operator(sum, operator, eval(sentence->children[i]));
		i++;
	}
	return sum;

}

long eval_operator(long current_sum, char* op, long next_number) {
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
		while (strstr(sentence->children[i]->tag, "expresion")) {
			sum_of_internal_nodes = number_of_nodes(sentence->children[i]);
			sum_of_the_node += sum_of_internal_nodes;
			i++;
		}

	}
	return sum_of_the_node;
}