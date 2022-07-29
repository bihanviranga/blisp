#include <stdio.h>

#include "lval.h"

lval lval_num(long num) {
  lval val;
  val.type = LVAL_NUM;
  val.num = num;
  return val;
}

lval lval_err(int type) {
  lval val;
  val.type = LVAL_ERR;
  val.err = type;
  return val;
}

void lval_print(lval val) {
  switch (val.type) {
    case LVAL_NUM:
      printf("%li", val.num);
      break;

    case LVAL_ERR:
      lval_print_err(val);
      break;
  }
}

void lval_println(lval val) {
  lval_print(val);
  printf("\n");
}

void lval_print_err(lval val) {
  printf("[ERROR] ");
  switch (val.err) {
    case LERR_DIV_ZERO:
      printf("Division by zero");
      break;
    case LERR_BAD_OP:
      printf("Invalid operator");
      break;
    case LERR_BAD_NUM:
      printf("Invalid number");
      break;
  }
}
