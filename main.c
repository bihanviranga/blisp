#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"

int main(int argc, char** argv) {
  // Create parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Blisp = mpc_new("blisp");

  // Define the parsers
  // NOTE: The support for decimals in <number> was added later
  // and might not be 100% correct.
  mpca_lang(
    MPCA_LANG_DEFAULT,
    "                                                   \
      number:   /-?[0-9]+([.][0-9]+)?/ ;                \
      operator: '+' | '-' | '*' | '/' ;                 \
      expr:     <number> | '(' <operator> <expr>+ ')' ; \
      blisp:    /^/ <operator> <expr>+ /$/ ;            \
    ",
    Number,
    Operator,
    Expr,
    Blisp
  );

  // Version and exit information
  puts("blisp 0.0.1");
  puts("Press ctrl+c to exit\n");

  // REPL
  while (1) {
    char* input = readline("blisp> ");
    add_history(input);

    // Attempt to parse the user input
    // Print the AST if successful
    // Else print the error
    mpc_result_t result;
    if (mpc_parse("<stdin>", input, Blisp, &result)) {
      mpc_ast_print(result.output);
      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  // Undefine and delete the parsers
  mpc_cleanup(4, Number, Operator, Expr, Blisp);

  return 0;
}
