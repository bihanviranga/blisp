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

void lval_print(lval val) {
  switch (val.type) {
    case LVAL_NUM:
      printf("%li", val.num);
      break;

    case LVAL_ERR:
      printf("[ERROR] %s", val.err);
      break;
  }
}

void lval_println(lval val) {
  lval_print(val);
  printf("\n");
}

void lval_del(lval* val) {
  switch(val->type) {
    // Do nothing for numbers
    case LVAL_NUM:  break;

    // lval types that use strings
    case LVAL_ERR:  free(val->err);  break;
    case LVAL_SYM:  free(val->sym);  break;

    // For S-Expressions, delete its child elements
    case LVAL_SEXPR:
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
  if (strcmp(ast->tag, "sexpr") == 0) val = lval_sexpr();

  // Populate the above list with valid expressions
  for (int i = 0; i < ast->children_num; i++) {
    if (strcmp(ast->children[i]->contents, "(") == 0) continue;
    if (strcmp(ast->children[i]->contents, ")") == 0) continue;
    if (strcmp(ast->tag, "regex") == 0) continue;

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
