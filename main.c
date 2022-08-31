#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

#include "mpc.h"
#include "blisp.h"
#include "lval.h"

int main(int argc, char** argv) {
  // Create parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr"); // S-Expression
  mpc_parser_t* Qexpr = mpc_new("qexpr"); // Q-Expression
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Blisp = mpc_new("blisp");

  // Define the parsers
  /*
   * NOTE: on the 'symbol' regex below.
   * We need to ignore some characters inside the regex.
   * To do that, we need to use the backslash.
   * However, since this is a c-string, the backslash itself
   * has to be ignored.
   * Therefore, there are two backslashes everywhere that needs one.
   * The real regex looks like follows:
   *    /[a-zA-Z0-9_+\-*%^\/\\=<>!&]+/
   */
  mpca_lang(
    MPCA_LANG_DEFAULT,
    "                                                     \
      number:   /-?[0-9]+/ ;                              \
      symbol:   /[a-zA-Z0-9_+\\-*%^\\/\\\\=<>!&]+/ ;      \
      sexpr:    '(' <expr>* ')' ;                         \
      qexpr:    '{' <expr>* '}' ;                         \
      expr:     <number> | <symbol> | <sexpr> | <qexpr> ; \
      blisp:    /^/ <expr>* /$/ ;                         \
    ",
    Number,
    Symbol,
    Sexpr,
    Qexpr,
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
    mpc_result_t mpc_result;
    if (mpc_parse("<stdin>", input, Blisp, &mpc_result)) {
#ifdef BLISP_PRINT_AST
      mpc_ast_print(mpc_result.output);
#endif
      lval* result = lval_eval(lval_read(mpc_result.output));
      lval_println(result);
      lval_del(result);
      mpc_ast_delete(mpc_result.output);
    } else {
      mpc_err_print(mpc_result.error);
      mpc_err_delete(mpc_result.error);
    }

    free(input);
  }

  // Undefine and delete the parsers
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Blisp);

  return 0;
}
