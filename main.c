#include <stdio.h>

static char input_buffer[2048];

int main(int argc, char** argv) {
  puts("blisp 0.0.1");
  puts("Press ctrl+c to exit\n");

  while (1) {
    fputs("blisp> ", stdout);
    fgets(input_buffer, 2048, stdin);
    printf("Read: %s", input_buffer);
  }

  return 0;
}
