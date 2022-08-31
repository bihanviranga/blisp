#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lval.h"

lval* lval_num(long num) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_NUM;
  val->num = num;
  return val;
}

lval* lval_err(char* msg) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_ERR;
  val->err = malloc(strlen(msg) + 1);
  strcpy(val->err, msg);
  return val;
}

lval* lval_sym(char* sym) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_SYM;
  val->sym = malloc(strlen(sym) + 1);
  strcpy(val->sym, sym);
  return val;
}

lval* lval_sexpr() {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_SEXPR;
  val->count = 0;
  val->cell = NULL;
  return val;
}

lval* lval_qexpr() {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_QEXPR;
  val->count = 0;
  val->cell = NULL;
  return val;
}

lval* lval_fun(lbuiltin func) {
  lval* val = malloc(sizeof(lval));
  val->type = LVAL_FUN;
  val->fun = func;
  return val;
}

void lval_print(lval* val) {
  switch (val->type) {
    case LVAL_NUM:
      printf("%li", val->num);
      break;

    case LVAL_ERR:
      printf("[ERROR] %s", val->err);
      break;

    case LVAL_SYM:
      printf("%s", val->sym);
      break;

    case LVAL_SEXPR:
      lval_expr_print(val, '(', ')');
      break;

    case LVAL_QEXPR:
      lval_expr_print(val, '{', '}');
      break;

    case LVAL_FUN:
      printf("<function>");
      break;
  }
}

void lval_println(lval* val) {
  lval_print(val);
  printf("\n");
}

void lval_del(lval* val) {
  switch(val->type) {
    // Do nothing for numbers and function pointers
    case LVAL_NUM:  break;
    case LVAL_FUN:  break;

    // lval types that use strings
    case LVAL_ERR:  free(val->err);  break;
    case LVAL_SYM:  free(val->sym);  break;

    // For S and Q expressions, delete its child elements
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      for (int i = 0; i < val->count; i++) {
        lval_del(val->cell[i]);
      }
      // and free the memory allocated to the pointer array
      free(val->cell);
      break;
  }

  // Free the memory used by the lval itself
  free(val);
}

lval* lval_read_num(mpc_ast_t* ast) {
  errno = 0;
  long val = strtol(ast->contents, NULL, 10);
  return errno != ERANGE ? lval_num(val) : lval_err("Invalid number");
}

lval* lval_read(mpc_ast_t* ast) {
  if (strstr(ast->tag, "number")) return lval_read_num(ast);
  if (strstr(ast->tag, "symbol")) return lval_sym(ast->contents);

  // If root ('>') or sexpr then create an empty list
  lval* val = NULL;
  if (strcmp(ast->tag, ">") == 0) val = lval_sexpr();
  if (strstr(ast->tag, "sexpr")) val = lval_sexpr();
  if (strstr(ast->tag, "qexpr")) val = lval_qexpr();

  // Populate the above list with valid expressions
  for (int i = 0; i < ast->children_num; i++) {
    if (strcmp(ast->children[i]->contents, "(") == 0) continue;
    if (strcmp(ast->children[i]->contents, ")") == 0) continue;
    if (strcmp(ast->children[i]->contents, "{") == 0) continue;
    if (strcmp(ast->children[i]->contents, "}") == 0) continue;
    if (strcmp(ast->children[i]->tag, "regex") == 0) continue;

    val = lval_add(val, lval_read(ast->children[i]));
  }

  return val;
}

lval* lval_add(lval* x, lval* y) {
  x->count++;
  x->cell = realloc(x->cell, sizeof(lval*) * x->count);
  x->cell[x->count - 1] = y;
  return x;
}

void lval_expr_print(lval* val, char open, char close) {
  putchar(open);

  for (int i = 0; i < val->count; i++) {
    // Print lval in cell
    lval_print(val->cell[i]);

    // Don't print trailing space for last element
    if (i != (val->count-1)) {
      putchar(' ');
    }
  }

  putchar(close);
}

lval* lval_eval_sexpr(lval* val) {
  // Evaluate children
  for (int i = 0; i < val->count; i++) {
    val->cell[i] = lval_eval(val->cell[i]);
  }

  // Check for errors
  for (int i = 0; i < val->count; i++) {
    if (val->cell[i]->type == LVAL_ERR) {
      return lval_take(val, i);
    }
  }

  // Handle empty expressions
  if (val->count == 0) {
    return lval_take(val, 0);
  }

  // Handle single expressions
  if (val->count == 1) {
    return lval_take(val, 0);
  }

  // Ensure the first cell is a symbol
  lval* first = lval_pop(val, 0);
  if (first->type != LVAL_SYM) {
    lval_del(first);
    lval_del(val);
    return lval_err("S-expression must start with a symbol");
  }

  // Call builtin with operator
  lval* result = builtin(val, first->sym);
  lval_del(first);

  return result;
}

lval* lval_eval(lval* val) {
  // S-Expressions are evaluated separately
  if (val->type == LVAL_SEXPR) {
    return lval_eval_sexpr(val);
  }

  // Other lval types remain the same
  return val;
}

lval* lval_pop(lval* val, int i) {
  lval* target = val->cell[i];

  // Shift the memory
  memmove(&(val->cell[i]), &(val->cell[i+1]), sizeof(lval*) * (val->count-i-1));

  val->count--;

  // Reallocate the used memory
  val->cell = realloc(val->cell, sizeof(lval*) * val->count);

  return target;
}

lval* lval_take(lval* val, int i) {
  lval* target = lval_pop(val, i);
  lval_del(val);
  return target;
}

lval* lval_join(lval* x, lval* y) {
  // Add all cells in y to x
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  lval_del(y);
  return x;
}

lval* lval_copy(lval* val) {
  lval* res = malloc(sizeof(lval));
  res->type = val->type;

  switch(val->type) {
    // Functions and numbers can be copied directly
    case LVAL_NUM: res->num = val->num; break;
    case LVAL_FUN: res->fun = val->fun; break;

    case LVAL_ERR:
      res->err = malloc(strlen(val->err) + 1);
      strcpy(res->err, val->err);
      break;

    case LVAL_SYM:
      res->sym = malloc(strlen(val->sym) + 1);
      strcpy(res->sym, val->sym);
      break;

    // In lists, copy each sub-expression
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      res->count = val->count;
      res->cell = malloc(sizeof(lval*) * res->count);
      for (int i = 0; i < res->count; i++) {
        res->cell[i] = lval_copy(val->cell[i]);
      }
      break;
  }

  return res;
}

lval* builtin_op(lval* val, char* op) {
  // Ensure all args are numbers
  for (int i = 0; i < val->count; i++) {
    if (val->cell[i]->type != LVAL_NUM) {
      lval_del(val);
      return lval_err("Expected a numerical value to operate on");
    }
  }

  lval* first = lval_pop(val, 0);

  // If no args and op is substract then perform unary negation
  if ((strcmp(op, "-") == 0) && val->count == 0) {
    first->num = -(first->num);
  }

  while (val->count > 0) {
    lval* second = lval_pop(val, 0);

    if (strcmp(op, "+") == 0) { first->num += second->num; }
    if (strcmp(op, "-") == 0) { first->num -= second->num; }
    if (strcmp(op, "*") == 0) { first->num *= second->num; }
    if (strcmp(op, "^") == 0) { first->num = pow(first->num, second->num); }

    // Operators below this point (div and mod) require that the second operand
    // is not zero
    if (second->num == 0) {
      lval_del(first);
      lval_del(second);
      first = lval_err("Division by zero");
      break;
    }

    if (strcmp(op, "/") == 0) { first->num /= second->num; }
    if (strcmp(op, "%") == 0) { first->num %= second->num; }
    lval_del(second);
  }
  lval_del(val);
  return first;
}

lval* builtin_head(lval* val) {
  LASSERT(val, val->count == 1,
    "Function 'head' passed too many arguments");
  LASSERT(val, val->cell[0]->type == LVAL_QEXPR,
    "Function 'head' passed incorrect types");
  LASSERT(val, val->cell[0]->count != 0,
    "Function 'head' passed {}");

  lval* res = lval_take(val, 0);

  while (res->count > 1) {
    lval_del(lval_pop(res, 1));
  }

  return res;
}

lval* builtin_tail(lval* val) {
  LASSERT(val, val->count == 1,
    "Function 'tail' passed too many arguments");
  LASSERT(val, val->cell[0]->type == LVAL_QEXPR,
    "Function 'tail' passed incorrect types");
  LASSERT(val, val->cell[0]->count != 0,
    "Function 'tail' passed {}");

  lval* res = lval_take(val, 0);
  lval_del(lval_pop(res, 0));

  return res;
}

lval* builtin_list(lval* val) {
  val->type = LVAL_QEXPR;
  return val;
}

lval* builtin_eval(lval* val) {
  LASSERT(val, val->count == 1,
    "Function 'eval' passed too many arguments");
  LASSERT(val, val->cell[0]->type == LVAL_QEXPR,
    "Function 'eval' passed incorrect type");

  lval* res = lval_take(val, 0);
  res->type = LVAL_SEXPR;
  return lval_eval(res);
}

lval* builtin_join(lval* val) {
  for (int i = 0; i < val->count; i++) {
    LASSERT(val, val->cell[i]->type == LVAL_QEXPR,
      "Function 'join' passed incorrect types");
  }

  lval* res = lval_pop(val, 0);

  while (val->count) {
    res = lval_join(res, lval_pop(val, 0));
  }

  lval_del(val);
  return res;
}

lval* builtin(lval* val, char* func) {
  if (strcmp("head", func) == 0)  return builtin_head(val);
  if (strcmp("tail", func) == 0)  return builtin_tail(val);
  if (strcmp("list", func) == 0)  return builtin_list(val);
  if (strcmp("eval", func) == 0)  return builtin_eval(val);
  if (strcmp("join", func) == 0)  return builtin_join(val);
  if (strstr("+-*/%^", func))     return builtin_op(val, func);

  lval_del(val);
  return lval_err("Unknown function");
}

lenv* lenv_new() {
  lenv* env = malloc(sizeof(lenv));
  env->count = 0;
  env->syms = NULL;
  env->vals = NULL;
  return env;
}

void lenv_del(lenv* env) {
  for (int i = 0; i < env->count; i++) {
    free(env->syms[i]);
    lval_del(env->vals[i]);
  }
  free(env->syms);
  free(env->vals);
  free(env);
}
