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
