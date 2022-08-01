#ifndef BLISP_LVAL_H
#define BLISP_LVAL_H

#include "mpc.h"

// Represents a number or an error.
typedef struct lval {
  // lval type as defined in the relevant enum
  int type;
  // numeric value, if the lval represents a number
  long num;
  // error message, if the lval represents an error
  char* err;
  // symbol, if the lval represents a symbol
  char* sym;

  // If the lval is an s-expression, it contains a list of other values.
  // The following fields are for the location and count of such values:

  // count of values
  int count;
  // location of values
  struct lval** cell;
} lval;

// Represents the type for lval.type
enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };

// Create a new lval from a number
lval* lval_num(long num);

// Create a new lval with the given error message
lval* lval_err(char* msg);

// Create a new lval from a symbol
lval* lval_sym(char* sym);

// Create a new lval for an S-Expression
lval* lval_sexpr();

// Print an lval
void lval_print(lval val);

// Print an lval followed by a newline character
void lval_println(lval val);

// Free the memory used by the lval
void lval_del(lval* lval);

// Read a number from the AST
lval* lval_read_num(mpc_ast_t* ast);

// Read the AST recursively and create a containing lval
lval* lval_read(mpc_ast_t* ast);

// Add the lval y to lval x's cells list
lval* lval_add(lval* x, lval* y);

#endif
