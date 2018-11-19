#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "mpc.h"

static char input[2048];


int main(int argc, char **argv){
	/* The following parsers are used to create grammar for spotting expressions in so called Polish notation - operator before number, let's say + 4 4 instead of 4+4,
	or * (+ 4 4) ( * 4 4) instead of (4+4)*(4*4) */
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expresion = mpc_new("expresion");
	mpc_parser_t* Lisp = mpc_new("lisp");

/*.	Any character is required.
a	The character a is required.
[abcdef]	Any character in the set abcdef is required.
[a-f]	Any character in the range a to f is required.
a?	The character a is optional.
a*	Zero or more of the character a are required.
a+	One or more of the character a are required.
^	The start of input is required.
$	The end of input is required.*/

	mpca_lang(MPCA_LANG_DEFAULT,
		"																\
		number 		: /-?[0-9]+/					 				;	\
		operator 	: '+' |'-' |'*'|'/'| '%' | \"min\" | \"max\" | '^'						;	\
		expresion 	: <number> | '(' <operator> <expresion>+ ')'	; 	\
		lisp 		: /^/ <operator> <expresion>+  /$/				;  	\
		",
		Number, Operator, Expresion, Lisp);

	puts("Lisp v0.0.1");
	puts("To exit, press Ctrl+c");

	while(1){
		fputs("lisp> ", stdout);
		fgets(input, 2048, stdin);

		//The following code calls mpc_parse function on parser Lisp. Result of the parse is copied to the r and 1 is returned on success, 0 on failure
		mpc_result_t r; 
		if(mpc_parse("<stdin>", input, Lisp, &r)){
			long result = eval(r.output);
			printf("%li\n", result);
			printf("You used %d numbers in your equation \n", number_of_nodes(r.output));
			mpc_ast_delete(r.output);
		}else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
	}


	mpc_cleanup(4, Number, Operator, Expresion, Lisp);
	return 0; 	
}
