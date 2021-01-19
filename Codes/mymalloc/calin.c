#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){

  size_t * init = sbrk(0);
  printf("%p\n", init );
  char *  whatever = malloc(1024*1024*1024*5);

   init = sbrk(0);
  printf("%p\n", init );



  return 0;
}
