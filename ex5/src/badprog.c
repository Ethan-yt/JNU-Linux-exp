/*badprog.c 错误地访问内存*/
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
  char *p;
  int i;
  p = malloc(30);
  strcpy(p, "not 30 bytes");

  if (argc == 2) {
    if (strcmp(argv[1], "-b") == 0)
      p[50] = 'a';
    else if (strcmp(argv[1], "-f") == 0) {
      free(p);
      p[0] = 'b';
    }
  }
  printf("p=<%s>\n", p);
  /*free(p)*/
  return 0;
}