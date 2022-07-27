#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

int main(int argc, char** argv) {
  puts("blisp 0.0.1");
  puts("Press ctrl+c to exit\n");

  while (1) {
    char* input = readline("blisp> ");
    add_history(input);
    printf("Read: %s\n", input);
    free(input);
  }

  return 0;
}
