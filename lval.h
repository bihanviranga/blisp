#ifndef BLISP_LVAL_H
#define BLISP_LVAL_H

// Represents a number or an error.
typedef struct {
  int type;
  long num;
  int err;
} lval;

// Represents the type for lval.type
enum { LVAL_NUM, LVAL_ERR };

// Represents the type for lval.err
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Create a new lval from a number
lval lval_num(long num);

// Create a new lval with the given error type
lval lval_err(int type);

// Print an lval
void lval_print(lval val);

// Print an lval error
void lval_print_err(lval val);

#endif
