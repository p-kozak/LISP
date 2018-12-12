#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "mpc.h"
#include "general.h"

#define LISP_ASSERT(argument, condition, error){ \
				if(!condition) { \
					lispValueDelete(argument)}; \
					return lispValueError("error");\
				}\
			}

static char input[2048];


int main(int argc, char **argv){

	/* The following parsers are used to create grammar for spotting expressions in so called prefix notation - operator before number, let's say + 4 4 instead of 4+4,
	or * (+ 4 4) ( * 4 4) instead of (4+4)*(4*4) */
	mpc_parser_t* Number = mpc_new("number");
	mpc_parser_t* Symbol = mpc_new("symbol");
	mpc_parser_t* Sym_expression = mpc_new("sym_expression");
	mpc_parser_t* Quoted_expression = mpc_new("quoted_expression");
	mpc_parser_t* Expression = mpc_new("expression");
	mpc_parser_t* Lisp = mpc_new("lisp");

/*
.	Any character is required.
a	The character a is required.
[abcdef]	Any character in the set abcdef is required.
[a-f]	Any character in the range a to f is required.
a?	The character a is optional.
a*	Zero or more of the character a are required.
a+	One or more of the character a are required.
^	The start of input is required.
$	The end of input is required.*/
// /-?[0-9]+[.][0-9]+/  |

/*Quoted expressions:
list - take one or more arguments and return new Quoted Expression containing them
head - takes quoted expression and return only the first element as quoted expression
tail - takes quoted expression and return a quoted expression with first element removed
join - takes one ore more quoted expressions and returns them as conjoined quoted expression
eval - takes quoted expression and evaluates it as it was a symbolic expression
*/

	mpca_lang(MPCA_LANG_DEFAULT,
	"                          			                                                                  \
        number 		       		:  /-?[0-9]+[.][0-9]+/  |	/-?[0-9]+/ 			 													;		\
        symbol              : '+' | '-' | '*' | '/' | \"list\" | 																						\
														\"head\" | \"tail\" | \"join\" | \"eval\"                                            ;   \
        sym_expression 			: '(' <expression>* ')'                															;   \
				quoted_expression		: '{' <expression>* '}'																						;	\
        expression          : <number> | < symbol> | <sym_expression>  | <quoted_expression>                   ;   \
        lisp                : /^/ <expression>* /$/                     												;   \
		",
		Number, Symbol, Sym_expression, Quoted_expression, Expression, Lisp);

	puts("Lisp version bugged");
	puts("To exit, press Ctrl+c");

	while(1){
		fputs("lisp> ", stdout);
		fgets(input, 2048, stdin);


		//The following code calls mpc_parse function on parser Lisp. Result of the parse is copied to the r and 1 is returned on success, 0 on failure
		mpc_result_t r;
		if(mpc_parse("<stdin>", input, Lisp, &r)){
			lispValue* result = lispValueRead(r.output);
			//lispValue* result = lispValueEval(lispValueRead(r.output));
			lispValuePrintNewline(result);

			//mpc_ast_print(r.output);

			//printf("You used %d numbers in your equation \n", numberOfNodes(r.output));
			mpc_ast_delete(r.output);
			lispValueDelete(result);

		}else {
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
	}


	mpc_cleanup(6, Number, Symbol, Sym_expression, Quoted_expression, Expression, Lisp);
	return 0;
}
