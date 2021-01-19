#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
  int x = 42;
int main(){

//  int pid = getpid();
  int mother = getpid();
  printf(" process Mother is :(%d) and x is: %d\n ", getpid() , x);

  int pid = fork();
if(pid == mother){
  x=55;
  printf(" I am the mother (%d) and x is %d\n", mother , x );
}
//  printf(" process Mother is :(%d) and x is: %d\n ", pid , x);
else{
  x=60;
printf("i am the child (%d) and x is: %d\n", getpid(), x);
}

}
