#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

#include "mpc.h"
#include "blisp.h"
#include "lval.h"

// Evalute unary operations
lval* eval_unary_op(char* operator, lval* x) {
  if (strcmp(operator, "-") == 0) return lval_num(-x->num);

  return lval_err("'%c' is not a unary operator");
}

// Evaluate binary operations
lval* eval_binary_op(char* operator, lval* x, lval* y) {
  if (x->type == LVAL_ERR) return x;
  if (y->type == LVAL_ERR) return y;

  if (strcmp(operator, "+") == 0) return lval_num(x->num + y->num);
  if (strcmp(operator, "-") == 0) return lval_num(x->num - y->num);
  if (strcmp(operator, "*") == 0) return lval_num(x->num * y->num);
  if (strcmp(operator, "^") == 0) return lval_num(pow(x->num, y->num));

  // Operators below this point require that the second operand is not zero
  if (y->num == 0) return lval_err("Division by zero");

  if (strcmp(operator, "/") == 0) return lval_num(x->num / y->num);
  if (strcmp(operator, "%") == 0) return lval_num(x->num % y->num);

  return lval_err("Unrecognized operator '%c'");
}

lval* eval(mpc_ast_t* t) {
  // Numbers can be evaluated directly
  if (strstr(t->tag, "number")) {
    errno = 0;
    long value = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(value) : lval_err("Bad number");
  }

  // The operator is the second child
  // The first child is always the 'regex' tag which comes from
  // the mpc library.
  char* operator = t->children[1]->contents;

  // Third child (first operand) is always provided
  lval* result = eval(t->children[2]);

  // The rest of the children are iterated over and the result
  // is combined.
  int i = 3;
  // If there's only one more child, that means the ending tag,
  // which means this is probably a unary operation.
  if (i + 1 == t->children_num) {
    result = eval_unary_op(operator, result);
  } else {
    // Otherwise it is evaluted as a binary operation
    while (strstr(t->children[i]->tag, "expr")) {
      result = eval_binary_op(operator, result, eval(t->children[i]));
      i++;
    }
  }

  return result;
}

int main(int argc, char** argv) {
  // Create parsers
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr"); // S-Expression
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Blisp = mpc_new("blisp");

  // Define the parsers
  mpca_lang(
    MPCA_LANG_DEFAULT,
    "                                                   \
      number:   /-?[0-9]+/ ;                            \
      symbol:   '+' | '-' | '*' | '/' | '%' | '^' ;     \
      sexpr:    '(' <expr>* ')' ;                       \
      expr:     <number> | <symbol> | <sexpr> ;         \
      blisp:    /^/ <expr>* /$/ ;                       \
    ",
    Number,
    Symbol,
    Sexpr,
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
  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Blisp);

  return 0;
}
